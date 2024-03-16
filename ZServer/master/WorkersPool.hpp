#pragma once

#include <thread>
#include "../Objects.hpp"
#include "../WorkersBase.hpp"
#include <osResources/SharedObject.hpp>
#include <osResources/Socket.h>
using namespace ZServer;

class WorkerManager : public WorkerBase
{
	/*
		A class that group some resources for worker process
	*/
public:
	Process p;
	SharedObject<sharedQueueType, SomeStruct> *so;
	std::thread pipeThread;

public:
	WorkerManager(
		std::string executable,
		int id,
		std::string sharedMemoryName,
		SharedObject<sharedQueueType, SomeStruct> *so,
		std::string targetDLL
		) : WorkerBase(id)

	{
		pipe.create(getPipeName(id));
		pipeThread = std::thread(&WorkerManager::PipeHandler, this);
		so = so;

		std::stringstream ss;
		ss << executable
		   << " "
		   << "--id"
		   << " "
		   << id
		   << " "
		   << "-m"
		   << " "
		   << sharedMemoryName
		   << " "
		   << "-d"
		   << " "
		   << targetDLL;
		p = Process(ss.str());
	}

	~WorkerManager()
	{
		pipeThread.join();
		pipe.close();
		p.close();
	}

	void PipeHandler()
	{
		pipe.listen();
		char buf[PIPE_BUFSIZE];
		while (true)
		{
			ZeroMemory(buf, sizeof(buf));

			if (pipe.read(buf, sizeof(buf)) > 0)
			{
				sharedQueueType msg;
				memcpy(&msg, buf, sizeof(sharedQueueType));
				SocketTCP socket(msg);
				WSAPROTOCOL_INFOW wsaProtocolInfo = socket.transfer(this->p.pi.dwProcessId);
				memcpy(buf, &wsaProtocolInfo, sizeof(wsaProtocolInfo));
				pipe.write(buf, BUFFERSIZE);
			}
			else
			{
				std::cout << "exiting\n";

				break;
			}
		}
	}
};

class WorkersPool
{
	int numberOfSlaves;
	std::string sharedMemoryName;
	std::string targetExecutable;
	std::string targetDLL;

	WorkerManager *workers = nullptr;

public:
	SharedObject<sharedQueueType, SomeStruct> *so;

	WorkersPool(std::string file_name, int &slaves, std::string &memoryName,std::string &dllPath) : numberOfSlaves(slaves),
																			   sharedMemoryName(memoryName),
																			   targetExecutable(file_name),
																			   targetDLL(dllPath)
	{
		so = new SharedObject<sharedQueueType, SomeStruct>(shared_mem_size, sharedMemoryName.c_str(), true);
		so->setData(SomeStruct());
	}

	~WorkersPool()
	{
		if (workers)
		{
			for (int i = 0; i < numberOfSlaves; ++i)
			{
				workers[i].~WorkerManager(); // Call the destructor for each object
			}
			free(workers);
		}

		delete so;
	}
	void createWorkers()
	{
		workers = (WorkerManager *)malloc(sizeof(WorkerManager) * numberOfSlaves);

		for (int id = 0; id < numberOfSlaves; id++)
		{
			// so->push(id * 10);
			new (&workers[id]) WorkerManager(targetExecutable, id, sharedMemoryName, so,targetDLL);
		}
	}
	void addTask(sharedQueueType t)
	{
		so->push(t);
	}
};
