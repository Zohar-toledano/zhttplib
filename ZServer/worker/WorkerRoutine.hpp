#pragma once

// #include <osResources/SharedObject.hpp>

#include <server/HTTPServerProps.h>
#include "../Objects.hpp"
#include "../WorkersBase.hpp"
// #include <fstream>
#define PROC_NAME "GetProps"

using namespace ZServer;

class WorkerRoutine : public WorkerBase
{
public:
	SharedObject<sharedQueueType, SomeStruct> *so;
	HMODULE hModule;
	HTTPServerProps *props;

	WorkerRoutine(int id, std::string sharedMemoryName, std::string dllPath) : WorkerBase(id)
	{
		so = new SharedObject<sharedQueueType, SomeStruct>(shared_mem_size, sharedMemoryName.c_str(), false);
		pipe.connect(WorkerBase::getPipeName(id));
		ZServer::SocketTCP::init();

		typedef HTTPServerProps *(__stdcall * GetPropsFunction)(void);

		hModule = LoadLibraryA(dllPath.c_str());
		if (hModule == NULL)
		{
			std::cerr << "Failed to load DLL. Error code: " << GetLastError() << std::endl;
		}

		GetPropsFunction GetProps = (GetPropsFunction)GetProcAddress(hModule, PROC_NAME);

		if (GetProps == NULL)
		{
			std::cerr << "Failed to get address of GetProps function. Error code: " << GetLastError() << std::endl;
		}

		props = GetProps(); //gets server props
	}
	~WorkerRoutine()
	{
		delete so;
		delete props;
		FreeLibrary(hModule);
	}

	SocketTCP getSocket(sharedQueueType *socketID)
	{
		// requests ownership over socket from master process!
		// std::cout << "slave: " << id << " is getting socket " << *socketID << std::endl;
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
		if (!client_socket.isValid())
		{
			std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
			client_socket.close();
			return;
		}
		HTTPResponse resp{"", {}, &client_socket};
		HTTPRequest req;
		bool exception_caught = true;

		try
		{
			req = HTTPRequest::requestFromSocket(client_socket);
			exception_caught = false; // This needs to be the last statement in the try block

		}
		catch (ZBasicException &e)
		{
			resp.setStatus(400);
			resp.body = e.what();
		}
		if (!exception_caught)
		{
			RoteMatch match = props->routeMap.getRouteHandler(req.path);
			req.pathParams = match.routeParams; // resp.setStatus(200);
			if (match.found)
			{
				try
				{
					match.handler(&req, &resp);
				}
				catch (...)
				{
					props->HandleServerError(&req, &resp);
				}
			}
			else
			{
				props->HandleNotfound(&req, &resp);
			}

		}
		props->logger(&req, &resp);
		resp.send();
		client_socket.close();

	}
};
