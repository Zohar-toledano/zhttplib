#include <server/HTTPServer.hpp>

void HTTPServer::Logger(HTTPRequest *req, HTTPResponse *resp)
{
	std::cout << req->path << " " << resp->getStatus() << std::endl;
}
void HTTPServer::HandleNotfound(HTTPRequest *req, HTTPResponse *resp)
{
	resp->setStatus(404);
	resp->body = "Not Found";
}
void HTTPServer::HandleServerError(HTTPRequest *req, HTTPResponse *resp)
{
	resp->setStatus(500);
	resp->body = "Internal Server Error";
}

void HTTPServer::at(std::string routePath, RouteHandler handler, bool standard)
{
	router.add(routePath, handler, standard);
}
void HTTPServer::handleSocket(AcceptedSocket accSock)
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
