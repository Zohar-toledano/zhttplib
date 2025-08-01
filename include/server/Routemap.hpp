#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <regex>

#include <http/HTTP.hpp>

struct Regex {
public:
    std::regex regex;
    std::string str;
    Regex(std::string pattern);
    Regex();
    bool operator==(Regex r);
};

struct RouteMapEntry {
public:
    std::string value;
    bool isParam;
    bool isStandart;
    Regex regexParam;
    std::vector<RouteMapEntry*> childrens{};
    RouteHandler handler;

    RouteMapEntry();
    RouteMapEntry(std::string component, RouteHandler handler, bool isStandart);

    bool isMatch(std::string param, std::string* result = nullptr);
};

typedef std::vector<RouteMapEntry*> RouteList;

struct RoteMatch {
    bool isStandart;
    bool found;
    StringMap routeParams{};
    RouteHandler handler;

    RoteMatch();
    RoteMatch(StringMap routeParams, RouteHandler handler = nullptr, bool isStandart = false);
};

class RouteMap {
    RouteList Routes{};

public:
    void add(std::string route, RouteHandler handler, bool isStandart);
    RouteMapEntry* getOrCreateRouteMapEntry(RouteList* lst, RouteMapEntry entry);
    RoteMatch getRouteHandler(std::string route);
    static std::vector<std::string> splitRoute(std::string route);
};
