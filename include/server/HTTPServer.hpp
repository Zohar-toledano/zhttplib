#pragma once
#include <iostream>
#include "SocketServer.hpp"

class HTTPServer : public SocketServer
{
	RouteMap router{};

public:
	virtual void Logger(HTTPRequest *req, HTTPResponse *resp);
	virtual void HandleNotfound(HTTPRequest *req, HTTPResponse *resp);
	virtual void HandleServerError(HTTPRequest *req, HTTPResponse *resp);

	void at(std::string routePath, RouteHandler handler, bool standard = true);
	virtual void handleSocket(AcceptedSocket accSock) override;
};