#pragma once

#include <http/HTTPRequest.hpp>
#include <http/HTTPResponse.hpp>

typedef std::function<void(HTTPRequest *, HTTPResponse *)> RouteHandler;
