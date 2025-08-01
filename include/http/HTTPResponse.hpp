#pragma once
#include <http/HTTPBase.hpp>
class HTTPResponse : public HTTPBase
{
	int status;
	TCPSocket *target_socket = nullptr;
	bool sent = false;

public:
	HTTPResponse(
		std::string body = "",
		HTTPHeaders headers = {},
		TCPSocket *target_socket = nullptr,
		int status = DEFAULT_STATUS,
		HTTP_VERSION protocol = HTTP_VERSION::HTTP_1_1);

	std::string getTextStatus();

	int getStatus();

	void setStatus(int status);

	std::string toText();

	virtual void print();

	void send();
	void send(int contentLength);
	void sendMore(char *buf, int size);
	static const std::unordered_map<int, std::string> statusToStringMap;
};
