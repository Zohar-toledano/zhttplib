#pragma once

#include "WorkersBase.hpp"
#include <osResources/SharedObject.hpp>
#include <osResources/Socket.h>
#include "Objects.hpp"

using namespace ZServer;

class WorkerRoutine : public WorkerBase
{
public:
	SharedObject<sharedQueueType, SomeStruct> *so;
	WorkerRoutine(int id, std::string sharedMemoryName) : WorkerBase(id)
	{
		so = new SharedObject<sharedQueueType, SomeStruct>(shared_mem_size, sharedMemoryName.c_str(), false);
		pipe.connect(WorkerBase::getPipeName(id));
		ZServer::SocketTCP::init();
	}
	~WorkerRoutine()
	{
		delete so;
	}

	SocketTCP getSocket(sharedQueueType *socketID)
	{
		// requests ownership over socket from master process!
		char buf[PIPE_BUFSIZE];
		MasterSlaveMessage msg(*socketID);
		WSAPROTOCOL_INFOW wsaProtocolInfo;

		memcpy(buf, &msg, sizeof(msg));
		pipe.write(buf, PIPE_BUFSIZE);
		pipe.read(buf, PIPE_BUFSIZE);
		memcpy(&wsaProtocolInfo, buf, sizeof(wsaProtocolInfo));
		SocketTCP socket(wsaProtocolInfo);
		return socket;
	}
	void workerMain()
	{
		std::cout << "slave: " << id << std::endl;
		sharedQueueType socket_int;
		// TODO: add timeout
		// TODO: add stop mechanism
		while (true)
		{
			if (so->pop(socket_int))
			{
				SocketTCP sock = getSocket(&socket_int);
				handleSocket(sock);
				continue;
			}
			Sleep(50);
		}
	}
	void handleSocket(SocketTCP client_socket)
	{
		if (!client_socket.isValid())
		{
			std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
			client_socket.close();
			return;
		}
		HTTPResponse resp{"", {}, &client_socket};
		HTTPRequest req;
		try
		{
			req = HTTPRequest::requestFromSocket(client_socket);
		}
		catch (ZBasicException &e)
		{
			resp.setStatus(400);
			resp.body = e.what();
			resp.send();
			return;
		}
		resp.setStatus(200);
		resp.body = "welcome to ZServer!!!\n";
		resp.send();
	}
};
