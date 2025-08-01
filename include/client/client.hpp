#pragma once
#include <regex>

#include <osResources/TCPSocket.hpp>
#include <http/HTTP.hpp>

int getDefaultPort(HTTP_VERSION version);
struct URL
{
	int port = -1;
	std::string protocol;
	std::string host;
	std::string route;
	std::string query;
	std::string fragment;

	URL(std::string url);
};
class HTTPClient
{
public:
	HTTPHeaders headers{};
	HTTP_VERSION version = HTTP_VERSION::HTTP_1_1;
	HTTPClient()=default;
	static HTTPResponse Request(
		std::string url,
		HTTP_METHOD method = HTTP_METHOD::GET,
		HTTPHeaders headers = {},
		std::string body = "",
		StringMap URL_params = {},
		HTTP_VERSION version = HTTP_VERSION::HTTP_1_1);

	HTTPResponse request(
		std::string url,
		HTTP_METHOD method = HTTP_METHOD::GET,
		HTTPHeaders headers = {},
		std::string body = "",
		StringMap URL_params = {});

};
