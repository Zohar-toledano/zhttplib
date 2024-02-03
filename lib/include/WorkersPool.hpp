#pragma once

#include <thread>

#include "WorkersBase.hpp"
#include "SharedObject.hpp"
#include "Objects.hpp"
#include "Socket.h"

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
		SharedObject<sharedQueueType, SomeStruct> *so) : WorkerBase(id)
	{
		std::stringstream ss;
		ss << executable
		   << " "
		   << "slave"
		   << " "
		   << "--id"
		   << " "
		   << id
		   << " "
		   << "-m"
		   << " "
		   << sharedMemoryName;
		pipe.create(getPipeName(id));
		pipeThread = std::thread(&WorkerManager::PipeHandler, this);
		so = so;
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
				MasterSlaveMessage msg;
				memcpy(&msg, buf, sizeof(msg));
				handleIncomingMessage(msg);
				memcpy(buf, &msg, sizeof(msg));
				pipe.write(buf, sizeof(msg));
			}
			else
				break;
		}
	}

	void handleIncomingMessage(MasterSlaveMessage &msg)
	{
		// move socket to worker process position
		// std::cout << "message from " << id << " (pid:" << p.pi.dwProcessId << ")"
		// 		  << ": sockID: " << msg.socketID << std::endl;
		// msg.socketID *= 10;
	}
};

class WorkersPool
{
	int numberOfSlaves;
	std::string sharedMemoryName;
	std::string targetExecutable;
	WorkerManager *workers = nullptr;

public:
	SharedObject<sharedQueueType, SomeStruct> *so;

	WorkersPool(std::string file_name, int &slaves, std::string &memoryName) : numberOfSlaves(slaves),
																			   sharedMemoryName(memoryName),
																			   targetExecutable(file_name)
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
			new (&workers[id]) WorkerManager(targetExecutable, id, sharedMemoryName, so);
		}
	}
	void addTask(sharedQueueType t)
	{
		so->push(t);
	}
};
