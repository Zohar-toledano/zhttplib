#pragma once
#include <functional>
#include <unordered_map>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "../osResources/TCPSocket.hpp"
#include "../Exceptions.hpp"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif // BUFFER_SIZE

#ifndef DEFAULT_PROTOCOL
#define DEFAULT_PROTOCOL "HTTP/1.1"
#endif // DEFAULT_PROTOCOL

#define DEFAULT_STATUS 200

enum class HTTP_METHOD
{
	GET,
	POST,
	OPTIONS,
	PUT,
	PATCH,
	DELETE_,
	ALL
};
HTTP_METHOD strToHTTPMethod(std::string method);
std::string HTTPMethodToStr(HTTP_METHOD method);
enum class HTTP_VERSION
{
	HTTP_1_0,
	HTTP_1_1,
	HTTP_2_0
};
HTTP_VERSION strToHTTPVersion(std::string version);
std::string HTTPVersionToStr(HTTP_VERSION version);
typedef std::unordered_map<std::string, std::string> StringMap;
typedef StringMap HTTPHeaders;

class HTTPBase
{
public:
	HTTPHeaders headers{};
	std::string body{};
	HTTP_VERSION protocol;

	HTTPBase(HTTP_VERSION protocol = HTTP_VERSION::HTTP_1_1);

	static HTTPHeaders parseHeaders(std::string *text, bool *ended);
	static std::string headersToString(HTTPHeaders headers);
	virtual std::string toText() = 0;
	virtual void print();
	static const StringMap contentTypeByFileExt;

};