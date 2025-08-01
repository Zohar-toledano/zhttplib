#pragma once
#include <sstream>
#include <argparse.hpp>
#include <osResources/TCPSocket.hpp>
#include <osResources/Process.hpp>
#include <osResources/Pipe.hpp>
#include <osResources/SharedMemory.hpp>
#include <http/http.h>
#include <server/ThreadPool.hpp>
#include <server/Routemap.hpp>

#include <utils.hpp>

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
	ServerSharedMemory()=default;
	ServerSharedMemory(short *workersNum);

protected:
	virtual void initVars() override;
	virtual int calcSize() override;
	virtual void unInitVars() override;
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

	std::string to_string(char *filepath) const;
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

	SocketServer() = default;
	virtual void handleSocket(AcceptedSocket sock);
	int start(int argc, char *argv[]);

protected:
	int masterLoop(int argc, char *argv[]);
	int workerLoop(int argc, char *argv[]);
	void startWorkers(char *filePath);
	inline int getLeastBusyWorker();

	inline void sendSocketToWorker(AcceptedSocket AC, int workerId);

public:
	static std::string getRandomName(std::string prefix = "");

	void close();
};
