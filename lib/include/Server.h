#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <list>
#include <vector>

#include <json.hpp>
#include "./Exceptions.h"
#include "./HTTPReqRes.h"
#include "./HTTPServerProps.h"
#include "./Routemap.h"
// #include "./TWorker.h"
#include "./Utils.h"

#include "./Socket.h"
#include "WorkersPool.hpp"
namespace ZServer
{
	using json = nlohmann::json;

	class Server
	{
	private:
		SocketTCP sock;
		int port;

	public:
		Server(int port) : port(port)
		{
			sock = SocketTCP();
			sock.bind(port);
		}

		~Server()
		{
			sock.close();
		}

		virtual void handleClientJoin(SocketTCP client_socket)
		{
			// pass to worker threads!

			// now just handle request:
			handleClientSocket(client_socket);
		}

		virtual void run()
		{
			sock.listen();
			std::cout << "Server is running on port " << port << "..." << std::endl;

			while (true)
			{
				SocketTCP client = sock.accept();
				handleClientJoin(client);
			}
		}

		virtual void handleClientSocket(SocketTCP client_socket)
		{
			if (!client_socket.isValid())
			{
				std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
				client_socket.close();
				return;
			}
			std::cout << "hi\n";
			char t[500];
			client_socket.recv(t, 500);
			std::cout << t << "\n";

			client_socket.close();
		}
	};

	class HTTPServer : public Server
	{
	private:
		HTTPServerProps props;
		WorkersPool *pool;

	public:
		HTTPServer(
			int port,
			std::string sharedMemoryName,
			std::string targetExecutable,
			int childNum = 2
			) : Server(port)
		{
			this->pool = new WorkersPool(targetExecutable,childNum, sharedMemoryName);
			props.logger = DefaultLogger;
			props.HandleNotfound = DefaultHandleNotfound;
			props.HandleServerError = DefaultHandleServerError;
		}
		~HTTPServer()
		{
			delete this->pool;
		}
		virtual void handleClientJoin(SocketTCP client_socket)
		{
			// TODO: handle full queue
			pool->addTask(client_socket);
		}

		void at(std::string routePath, RouteHandler handler, bool standard = true)
		{
			props.routeMap.add(routePath, handler, standard);
		}

		virtual void run() override
		{
			pool->createWorkers();
			Server::run();
		}

		void setLogger(RouteHandler lg)
		{
			props.logger = lg;
		}

		void addMiddleware(RouteHandler md, HTTPServerProps::MiddlewarePosition pos)
		{
			if (pos == HTTPServerProps::MiddlewarePosition::Before)
			{
				props.middlewareBefore.push_back(md);
			}
			else
			{
				props.middlewareAfter.push_back(md);
			}
		}
	};
}
