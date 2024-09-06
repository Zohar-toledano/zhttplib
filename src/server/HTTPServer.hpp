#pragma once
#include <iostream>
#include "SocketServer.hpp"

class HTTPServer : public SocketServer
{
	RouteMap router{};

public:
	virtual void Logger(HTTPRequest *req, HTTPResponse *resp)
	{
		std::cout << req->path << " " << resp->getStatus() << std::endl;
	}
	virtual void HandleNotfound(HTTPRequest *req, HTTPResponse *resp)
	{
		resp->setStatus(404);
		resp->body = "Not Found";
	}
	virtual void HandleServerError(HTTPRequest *req, HTTPResponse *resp)
	{
		resp->setStatus(500);
		resp->body = "Internal Server Error";
	}

	void at(std::string routePath, RouteHandler handler, bool standard = true)
	{
		router.add(routePath, handler, standard);
	}
	virtual void handleSocket(AcceptedSocket accSock) override
	{
		HTTPResponse resp{"", {}, &accSock.socket};
		HTTPRequest req;
		try
		{
			req = HTTPRequest::requestFromSocket(accSock.socket);
		}
		catch (ZBasicException &e)
		{
			resp.setStatus(400);
			resp.body = e.what();
			resp.send();
			return;
		}

		RoteMatch match = router.getRouteHandler(req.path);
		req.pathParams = match.routeParams;

		// callMiddlewares(&req, &resp, HTTPServerProps::MiddlewarePosition::Before);

		if (match.found)
		{
			try
			{
				match.handler(&req, &resp);
			}
			catch (...)
			{
				HandleServerError(&req, &resp);
			}
		}
		else
		{
			HandleNotfound(&req, &resp);
		}
		// callMiddlewares(&req, &resp, HTTPServerProps::MiddlewarePosition::After);

		Logger(&req, &resp);
		resp.send();
		accSock.socket.close();
	}
};