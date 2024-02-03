#pragma once
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "Routemap.h"
#include "HTTPServerProps.h"
#define WORKER_SLEEP_TIME 10

namespace ZServer
{

	template <typename T>
	struct SharedQueue
	{
		std::mutex mutex;
		std::queue<T> queue;
		std::atomic_int size; // Tracks queue size for efficient popping
		void Push(T element)
		{
			std::lock_guard<std::mutex> lock(mutex);
			queue.push(element);
			size++;
		}

		bool Pop(T &outElement)
		{
			std::lock_guard<std::mutex> lock(mutex);
			if (queue.empty())
				return false;
			outElement = queue.front();
			queue.pop();
			size--;
			return true;
		}
	};

	class TWorker
	{
		SharedQueue<SocketTCP> *queue;
		HTTPServerProps *serverProps;
		std::thread thread;
		bool *runing;

	public:
		TWorker(SharedQueue<SocketTCP> *queue, HTTPServerProps *sp, bool *runing) : queue(queue), serverProps(sp), runing(runing)
		{
		}
		void start()
		{
			// create thread of mainLoop and store it in thread member variable.
			thread = std::thread(&TWorker::mainLoop, this);
		}
		virtual void mainLoop()
		{
			SocketTCP sock;
			while (*runing)
			{
				// lock mutex
				// check if something in queue
				// std::cout<<"1111\n";
				if (queue->Pop(sock))
				{
					handleClientSocket(sock); // dont get to sleep!
				}

				// if it is remove it from queue and pass it to handleClientSocket
				// free mutex

				// else
				// 	Sleep(WORKER_SLEEP_TIME);
			}
		}

		void callMiddlewares(HTTPRequest *req, HTTPResponse *res, HTTPServerProps::MiddlewarePosition pos)
		{
			if (pos == HTTPServerProps::MiddlewarePosition::Before)
			{
				for (auto md : serverProps->middlewareBefore)
				{
					md(req, res);
				}
			}
			else
			{
				for (auto md : serverProps->middlewareAfter)
				{
					md(req, res);
				}
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

			RoteMatch match = serverProps->routeMap.getRouteHandler(req.path);
			req.pathParams = match.routeParams;

			callMiddlewares(&req, &resp, HTTPServerProps::MiddlewarePosition::Before);

			if (match.found)
			{
				try
				{
					match.handler(&req, &resp);
				}
				catch (...)
				{
					serverProps->HandleServerError(&req, &resp);
				}
			}
			else
			{
				serverProps->HandleNotfound(&req, &resp);
			}
			callMiddlewares(&req, &resp, HTTPServerProps::MiddlewarePosition::After);

			serverProps->logger(&req, &resp);
			resp.send();

			client_socket.close();
		}
	};

	class TWorkersPool
	{
		bool runing;
		int threads;
		SharedQueue<SocketTCP> queue;
		HTTPServerProps *serverProps;
		std::vector<TWorker *> workers;

	public:
		TWorkersPool(HTTPServerProps *sp, int workersNum) : serverProps(sp)
		{
			createWorkers(workersNum);
		};
		TWorkersPool(){};

		~TWorkersPool() { destroyWorkers(); };

		void createWorkers(int threads)
		{
			if (workers.size() == 0)
			{

				for (int i = 0; i < threads; i++)
				{
					auto worker = new TWorker(&queue, serverProps, &runing);
					workers.push_back(worker);
				}
			}
		}
		void destroyWorkers()
		{
			stop();
			for (auto w : workers)
				delete w;

			workers = {};
		}
		void start()
		{
			runing = true;
			for (auto w : workers)
				w->start();
		}
		void stop()
		{
			runing = false;
		}
		void addTask(SocketTCP t)
		{
			queue.Push(t);
		}
	};
}
