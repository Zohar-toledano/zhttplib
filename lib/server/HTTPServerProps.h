#pragma once

#include <list>

#include "Routemap.h"
#include "Utils.h"

namespace ZServer {
	struct HTTPServerProps
	{
		enum class MiddlewarePosition {
			Before,
			After
		};
		RouteMap routeMap{};
		RouteHandler logger = DefaultLogger;
		std::list<RouteHandler> middlewareBefore {};
		std::list<RouteHandler> middlewareAfter {};
		RouteHandler HandleNotfound = DefaultHandleNotfound;
		RouteHandler HandleServerError = DefaultHandleServerError;

	};
}