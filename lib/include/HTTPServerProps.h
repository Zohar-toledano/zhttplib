#pragma once

#include <list>

#include "HTTPReqRes.h"
#include "Routemap.h"


namespace ZServer {
	struct HTTPServerProps
	{
		enum class MiddlewarePosition {
			Before,
			After
		};
		RouteMap routeMap{};
		RouteHandler logger;
		std::list<RouteHandler> middlewareBefore {};
		std::list<RouteHandler> middlewareAfter {};
		RouteHandler HandleNotfound;
		RouteHandler HandleServerError;




	};
}