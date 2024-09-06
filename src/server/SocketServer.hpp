#pragma once
#include <sstream>
#include "argparse.hpp"
#include "../osResources/TCPSocket.hpp"
#include "../osResources/Process.hpp"
#include "../osResources/Pipe.hpp"
#include "../osResources/SharedMemory.hpp"
#include "../http/HTTPReqRes.h"
#include "ThreadPool.hpp"
#include "Routemap.h"

#include "../utils.hpp"

#ifndef RANDOM_NAME_NAMESPACE
#define RANDOM_NAME_NAMESPACE "ZSERVER"
#endif // RANDOM_NAME_NAMESPACE

#ifndef PASS_TO_WORKERS_TRESHOLD
#define PASS_TO_WORKERS_TRESHOLD 3
#endif // PASS_TO_WORKERS_TRESHOLD

#ifndef RANDOM_NAME_LENGTH
#define RANDOM_NAME_LENGTH 10
#endif // RANDOM_NAME_LENGTH

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif // BUFFER_SIZE
#define PIPE_PATH "\\\\.\\pipe\\"

typedef std::function<void(AcceptedSocket)> SocketHandlerFunction;

struct WorkerSharedBuffer
{
	int pid;
	short id;
	int activeSockets;
	bool running;
};

struct WorkerProcessStruct
{
	Process proc;
	Pipe pipe;
};

struct MasterWorkerMessage
{
	Address sockAddress;
	WSAPROTOCOL_INFOW PI;
};

class ServerSharedMemory : public SharedMemory
{

public:
	bool *serverRunning;
	WorkerSharedBuffer *workersBuffer = nullptr;
	short *workersNum;
	ServerSharedMemory() : SharedMemory() {}
	ServerSharedMemory(short *workersNum) : SharedMemory(), workersNum(workersNum) {}

protected:
	virtual void initVars() override
	{
		serverRunning = (bool *)baseAddress;
		*serverRunning = true;
		workersBuffer = (WorkerSharedBuffer *)((char *)baseAddress + sizeof(bool));
	}
	virtual int calcSize() override
	{
		int size = 0;
		size += sizeof(bool);
		size += sizeof(WorkerSharedBuffer) * (*workersNum);
		return size;
	}
	virtual void unInitVars() override
	{
		workersBuffer = nullptr;
	}
};

struct WorkerArgs : public argparse::Args
{
	short &id = kwarg("id", "An ID for worker process");
	std::string &sharedMemoryName = kwarg("m,shared-memory", "Shared memory name");
	std::string &pipeName = kwarg("p,pipe-name", "Pipe name");
};

struct Args : public argparse::Args
{
	WorkerArgs &workerConfig = subcommand("worker");

	std::string to_string(char *filepath) const
	{
		std::stringstream ss;
		ss << filepath << " "
		   << "worker" << " "
		   << "--id" << " " << workerConfig.id << " "
		   << "--shared-memory" << " " << workerConfig.sharedMemoryName << " "
		   << "--pipe-name" << " " << workerConfig.pipeName;
		return ss.str();
	}
};

class SocketServer
{
protected:
	ServerSharedMemory mem;
	WorkerProcessStruct *workers = nullptr;
	ThreadPool<AcceptedSocket> *pool;

public:
	short workerNum = 0;
	int port;
	std::string address;
	TCPSocket serverSocket;

	SocketServer() {};
	virtual void handleSocket(AcceptedSocket sock) {}
	int start(int argc, char *argv[])
	{
		if (argc > 1)
		{
			return workerLoop(argc, argv);
		}
		else
		{
			return masterLoop(argc, argv);
		}
		return 1;
	}

protected:
	int masterLoop(int argc, char *argv[])
	{

		int handledByThisProcess = 0;
		std::mutex handledByThisProcessMutex;
		if (workerNum > 0)
			startWorkers(argv[0]);
		else {
			mem.workersNum = &workerNum;
			mem.bloff();
		}

		pool = new ThreadPool<AcceptedSocket>(
			[&handledByThisProcess, &handledByThisProcessMutex, this](AcceptedSocket accSock)
			{
				handleSocket(accSock);
				{
					std::lock_guard<std::mutex> lock(handledByThisProcessMutex);
					handledByThisProcess--;
				}
			});
		pool->Start();

		serverSocket.bind(address, port);
		serverSocket.listen(SOMAXCONN);
		while (*mem.serverRunning)
		{
			AcceptedSocket acceptedClientSocket;
			try
			{
				acceptedClientSocket = serverSocket.accept();
			}
			catch (AcceptError)
			{
				break;
			}
			if (workerNum == 0) goto one_process;
			if (handledByThisProcess < PASS_TO_WORKERS_TRESHOLD)
			{
				one_process:
				{
					std::lock_guard<std::mutex> lock(handledByThisProcessMutex);
					handledByThisProcess++;
				}
				pool->QueueTask(acceptedClientSocket);
			}
			else
			{
				sendSocketToWorker(acceptedClientSocket, getLeastBusyWorker());
			}
		}
		// cleaning up
		for (int i = 0; i < workerNum; i++)
		{
			mem.workersBuffer[i].running = false;
			workers[i].pipe.close();
		}
		serverSocket.close();
		pool->Stop();
		delete pool;
		mem.close();
		free(workers);
		return 0;
	}

	int workerLoop(int argc, char *argv[])
	{
		// parse command line argument supplied by master process
		Args args = argparse::parse<Args>(argc, argv);
		short id = args.workerConfig.id;
		int pid = GetCurrentProcessId();

		// connect to pipe
		Pipe pipe;
		pipe.open(args.workerConfig.pipeName);

		// open shared memory
		mem.workersNum = &workerNum;
		mem.open((LPCTSTR)args.workerConfig.sharedMemoryName.c_str());

		// the WorkerSharedBuffer belongs to current process
		WorkerSharedBuffer *sb;
		sb = &mem.workersBuffer[id];

		// create a thread pool and set the handler for it
		pool = new ThreadPool<AcceptedSocket>(
			[sb, &id, this](AcceptedSocket accSock)
			{
				// consider adding mutex here
				handleSocket(accSock);
				sb->activeSockets--;
			});
		pool->Start();

		MasterWorkerMessage msg;
		while (*mem.serverRunning)
		{
			if (pipe.read((void *)&msg, sizeof(MasterWorkerMessage)) == -1)
				break;				// get message from master
			TCPSocket sock(msg.PI); // get ownership over socket
			Address addr = msg.sockAddress;
			sb->activeSockets++;
			pool->QueueTask({sock, addr}); // add to pool
		}
		// wait for pool to finish
		pipe.close();
		pool->Stop();
		delete pool;
		mem.close();
		return 0;
	}

	void startWorkers(char *filePath)
	{
		std::string SharedMemoryName = getRandomName("mem");

		mem.workersNum = &workerNum;
		mem.create((LPCTSTR)SharedMemoryName.c_str());

		workers = (WorkerProcessStruct *)malloc(sizeof(WorkerProcessStruct) * workerNum);
		for (short i = 0; i < workerNum; i++)
		{
			Args args;
			args.workerConfig.id = i;
			args.workerConfig.sharedMemoryName = SharedMemoryName;
			args.workerConfig.pipeName = PIPE_PATH + getRandomName("pipe");

			std::string cmdLine = args.to_string(filePath);
			workers[i] = {
				Process(cmdLine),
				Pipe()};
			workers[i].pipe.create(args.workerConfig.pipeName);
			mem.workersBuffer[i] = {workers[i].proc.getPID(), 0, true};
		}
	}

	inline int getLeastBusyWorker()
	{
		int min = 0;
		for (int i = 1; i < workerNum; i++)
		{
			if (mem.workersBuffer[i].activeSockets < mem.workersBuffer[min].activeSockets)
				min = i;
		}
		return min;
	}

	inline void sendSocketToWorker(AcceptedSocket AC, int workerId)
	{
		WorkerProcessStruct *w = &workers[workerId];
		MasterWorkerMessage msg{AC.address, AC.socket.transfer(w->proc.getPID())};
		workers[workerId].pipe.write((void *)&msg, sizeof(msg));
	}

public:
	static std::string getRandomName(std::string prefix = "")
	{
		char buffer[RANDOM_NAME_LENGTH];
		ZeroMemory(buffer, RANDOM_NAME_LENGTH);
		fillBufferWithRandomChars(buffer, RANDOM_NAME_LENGTH);
		std::stringstream ss;
		ss << RANDOM_NAME_NAMESPACE << "__" << prefix << "_" << buffer;
		return ss.str();
	}

	void close()
	{
		*mem.serverRunning = false;
		serverSocket.close();
	}
};
