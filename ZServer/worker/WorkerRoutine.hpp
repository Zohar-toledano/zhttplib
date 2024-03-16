#pragma once

#include "../WorkersBase.hpp"
#include <osResources/SharedObject.hpp>
#include <osResources/Socket.h>
#include "../Objects.hpp"
#include <fstream>

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
		std::cout << "slave: " << id << " is getting socket " << *socketID << std::endl;
		char buf[PIPE_BUFSIZE];
		WSAPROTOCOL_INFOW wsaProtocolInfo;

		memcpy(buf, socketID, sizeof(sharedQueueType));
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
		// std::stringstream ss;
		// ss<<"worker_"<<id<<".txt";
		// std::ofstream outFile(ss.str());
		// TODO: add timeout
		// TODO: add stop mechanism
		while (true)
		{
			if (so->pop(socket_int))
			{
				// std::stringstream strstr;
				// strstr<<socket_int<<std::endl;
				// outFile<<strstr.str();
				// outFile.flush();

				SocketTCP sock = getSocket(&socket_int);
				handleSocket(sock);
				continue;
			}
			Sleep(50);
		}
	}
	void handleSocket(SocketTCP client_socket)
	{
		std::cout << "slave: " << id << " is handling socket " << client_socket.sockfd << std::endl;
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
