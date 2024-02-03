#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include <http/httpReqRes.h>
#include <regex>
namespace ZServer {

struct Regex
{
public:
	std::regex regex;
	std::string str;
	Regex(std::string pattern) :str(pattern), regex(std::regex(pattern)) {}
	Regex() {};
	bool operator==(Regex r)
	{
		return r.str == this->str;
	}
};

struct RouteMapEntry {
public:
	std::string value;
	bool isParam;
	bool isStandart;
	Regex regexParam;
	std::vector<RouteMapEntry*> childrens{};
	RouteHandler handler;
	RouteMapEntry(){}
	RouteMapEntry(std::string component, RouteHandler handler,bool isStandart) :handler(handler),isStandart(isStandart) {

		if (component.starts_with("<") && component.ends_with(">"))
		{
			isParam = true;
			value = component.substr(1, component.size() - 2);
			int pos;

			if ((pos = value.find(":")) != std::string::npos)
			{
				std::string typeStr = value.substr(pos + 1, value.size() - 1);
				regexParam = typeStr;
				value = value.substr(0, pos);
			}
			else
			{
				regexParam = Regex(R"(.*)");
			}
		}
		else
		{
			isParam = false;
			value = component;
		}
	}

	bool isMatch(std::string param, std::string* result = nullptr)
	{
		// add regex
		if (isParam)
		{

			std::smatch matches;
			// Check if the input string matches the received regex pattern and get the matches
			if (std::regex_match(param, matches, regexParam.regex)) {
				if (matches.size() == 1) {
					if (result)
					{
						*result = matches[0];

					}
					return true;
				}
			}
			return false;
		}
		else
		{
			return param == value;
		}
	}
};

typedef std::vector<RouteMapEntry*> RouteList;
struct RoteMatch
{
	bool isStandart;
	bool found;
	StringMap routeParams{};
	RouteHandler handler;
	RoteMatch():found(false),handler(nullptr){}

	RoteMatch(
		StringMap routeParams,
		RouteHandler handler = nullptr,
		bool isStandart = false
	) :
		isStandart(isStandart),
		routeParams(routeParams),
		handler(handler),
		found(true)
	{

	}
};
class RouteMap
{
	RouteList Routes{};
public:
	void add(std::string route, RouteHandler handler,bool isStandart)
	{
		RouteList* currentRouteList = &Routes;
		std::vector<std::string> splitedRoute = splitRoute(route);
		int count = 1;
		for (auto part : splitedRoute)
		{
			bool lastPart = (count == splitedRoute.size());
			auto tempEntry = RouteMapEntry(
				part,
				lastPart ? handler : nullptr,
				lastPart ? isStandart : false
			);
			RouteMapEntry* entry = getOrCreateRouteMapEntry(
				currentRouteList,
				tempEntry
			);
			currentRouteList = &entry->childrens;
			count++;
		}
	}

	RouteMapEntry* getOrCreateRouteMapEntry(RouteList* lst, RouteMapEntry entry)
	{
		for (RouteMapEntry* routeEntry : *lst)
		{
			if (routeEntry->isParam)
			{
				if (entry.isParam)
				{
					if (
						entry.regexParam == routeEntry->regexParam &&
						entry.value == routeEntry->value
						)
					{
						return routeEntry;
					}
				}

			}
			else
			{
				if (!entry.isParam)
				{
					if (entry.value == routeEntry->value)
					{
						return routeEntry;
					}
				}
			}
		}
		auto ep = new RouteMapEntry();
		*ep = entry;
		lst->push_back(ep);
		return lst->back();
	}

	RoteMatch getRouteHandler(std::string route)
	{
		// has a bug!!!
		RoteMatch res;
		RouteList* currentRouteList = &Routes;
		int depth = 0;
		std::vector<std::string> splittedRoute = splitRoute(route);
		//std::unordered_map<std::string, std::string> params {};
		std::string paramBuffer;
		while (true)
		{
			std::string part = splittedRoute.at(depth);
			depth++;
			for (auto er : *currentRouteList)
			{

				if (er->isMatch(part, &paramBuffer))
				{
					currentRouteList = &er->childrens;
					if (er->isParam)
					{
						res.routeParams[er->value] = paramBuffer;
					}
					if (splittedRoute.size() == depth)
					{
						if (er->handler)
						{
							res.handler = er->handler;
							res.isStandart = er->isStandart;
							res.found = true;
							return res;

						}
					}
					break;
				}
			}
			if (splittedRoute.size() == depth) return res;


		}
	}

	static std::vector<std::string> splitRoute(std::string route)
	{
		std::vector<std::string> res;
		int startIdx = 0;
		int endIdx = 0;
		if (route.starts_with("/"))
		{
			startIdx = 1;
		}
		if (route.ends_with("/"))
		{
			route = route.substr(0, route.length() - 1);
		}
		if (route =="")
		{
			res.push_back("");
		}

		
		while (startIdx < route.length())
		{
			endIdx = route.find("/", startIdx);
			endIdx = endIdx > 0 ? endIdx : route.length();
			if (endIdx == std::string::npos)
			{
				break;
			}
			std::string word = route.substr(startIdx, endIdx - startIdx);
			res.push_back(word);
			startIdx = endIdx + 1;
		}
		return res;
	}

};

}
