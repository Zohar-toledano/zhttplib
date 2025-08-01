#pragma once
#include <http/HTTPBase.h>
class HTTPRequest : public HTTPBase
{
public:
	HTTP_METHOD method;
	std::string path;
	StringMap pathParams{};
	StringMap params{};

	HTTPRequest();
	HTTPRequest(
		std::string route,
		HTTP_METHOD method,
		HTTPHeaders headers,
		std::string body,
		StringMap URL_params,
		HTTP_VERSION version);
	virtual std::string toText() override;
	static HTTP_METHOD getMethod(std::string *text);

	static std::string getPath(std::string *text);

	static HTTP_VERSION getProtocol(std::string *text);

	static HTTPRequest fromText(std::string httpRequestString);
	static HTTPRequest requestFromSocket(TCPSocket client_socket);
	virtual void print() override;
};
