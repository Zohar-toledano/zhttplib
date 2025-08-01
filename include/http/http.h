#pragma once

#include <http/HTTPRequest.h>
#include <http/HTTPResponse.h>

typedef std::function<void(HTTPRequest *, HTTPResponse *)> RouteHandler;
