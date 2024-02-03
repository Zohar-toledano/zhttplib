#pragma once

#include "WorkersBase.hpp"
#include <osResources/SharedObject.hpp>
#include "Objects.hpp"

class WorkerRoutine : public WorkerBase
{
public:
	SharedObject<sharedQueueType, SomeStruct> *so;
	WorkerRoutine(int id, std::string sharedMemoryName) : WorkerBase(id)
	{
		so = new SharedObject<sharedQueueType, SomeStruct>(shared_mem_size, sharedMemoryName.c_str(), false);
		pipe.connect(WorkerBase::getPipeName(id));
	}
	~WorkerRoutine()
	{
		delete so;
	}

	void requestOwnershipOverSocket(sharedQueueType * socketID)
	{
		char buf[PIPE_BUFSIZE];
		MasterSlaveMessage msg(*socketID);
		memcpy(buf, &msg, sizeof(msg));
		std::cout<<"slave: " << id << " send message to master with code: "<<pipe.write(buf, PIPE_BUFSIZE)<<std::endl;
		pipe.read(buf, PIPE_BUFSIZE);
		memcpy(&msg, buf, sizeof(msg));
		// WSASocket(msg.socketID.family, msg.socketID.type, msg.socketID.protocol, NULL, 0, WSA_FLAG_OVERLAPPED);
	}
	void workerMain()
	{
		std::cout << "slave: " << id << std::endl;
		sharedQueueType socket;
		// TODO: add timeout
		// TODO: add stop mechanism
		while (true)
		{
			if (so->pop(socket))
			{
				requestOwnershipOverSocket(&socket);
				handleSocket(socket);
				// break;
			}
			Sleep(50);
		}
	}
	void handleSocket(sharedQueueType socket)
	{
		std::cout << "slave: " << id << " got socket: " << std::endl;
		socket.close();
	}
};

