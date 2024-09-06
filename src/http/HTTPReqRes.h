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

class HTTPData
{
public:
	HTTPHeaders headers{};
	std::string body{};
	HTTP_VERSION protocol;

	HTTPData(HTTP_VERSION protocol = HTTP_VERSION::HTTP_1_1) : protocol(protocol)
	{
	}

	static HTTPHeaders parseHeaders(std::string *text, bool *ended)
	{
		HTTPHeaders headers = HTTPHeaders();
		size_t startLine = 0;
		size_t endLine = 0;
		size_t headersEnd = text->find("\n\n");
		if (headersEnd == std::string::npos)
		{
			headersEnd = text->length();
		}
		size_t total = 0;
		while (total < headersEnd)
		{
			endLine = text->find("\r\n");
			total += endLine + 2;
			if (endLine == std::string::npos)
			{
				*ended = false;
				return headers;
			}
			size_t nekodotaim = text->find(":");
			if (nekodotaim == std::string::npos)
			{
				continue;
			}
			if (total != headersEnd)
			{
				std::string key = text->substr(0, nekodotaim);
				std::string value = text->substr(nekodotaim + 1, endLine - nekodotaim - 1);
				if (value.starts_with(" "))
				{
					value = value.substr(1);
				}

				headers[key] = value;
				// std::cout << "key: " << key << " value: " << value << "\n";
				*text = text->substr(endLine + 2);
			}
			else
			{
				*ended = false;
				return headers;
			}
		}
		*ended = true;
		return headers;
	}
	static std::string headersToString(HTTPHeaders headers)
	{
		std::stringstream ss;
		for (auto [key, value] : headers)
		{
			ss << key << ": " << value << "\n";
		}
		return ss.str();
	}
	virtual std::string toText() { return ""; }
	virtual void print() { std::cout << "HTTPData object"; }
	static const StringMap contentTypeByFileExt;

private:
};

class HTTPRequest : public HTTPData
{
public:
	HTTP_METHOD method;
	std::string path;
	StringMap pathParams{};
	StringMap params{};

	HTTPRequest() : HTTPData()
	{
	}
	HTTPRequest(
		std::string route,
		HTTP_METHOD method = HTTP_METHOD::GET,
		HTTPHeaders headers = {},
		std::string body = "",
		StringMap URL_params = {},
		HTTP_VERSION version = HTTP_VERSION::HTTP_1_1)
	{

		// TODO GET params to/from str
		// TODO change send in socket file
		this->path = route;
		this->method = method;
		this->headers = headers;
		this->body = body;
		this->protocol = version;
	}
	virtual std::string toText() override
	{
		std::stringstream ss;
		ss << HTTPMethodToStr(method)
		   << " " << path << " " << HTTPVersionToStr(protocol) << "\r\n";
		ss << headersToString(headers) << "\r\n";
		ss << body;
		return ss.str();
	}
	static HTTP_METHOD getMethod(std::string *text)
	{
		size_t space = text->find(" ");
		if (space == std::string::npos)
			throw StrNotFoundError(" ", *text);

		std::string method = text->substr(0, space);
		*text = text->substr(space + 1);
		return strToHTTPMethod(method);
	}

	static std::string getPath(std::string *text)
	{

		size_t newline_pos = text->find('\n');

		if (newline_pos == std::string::npos)
			throw StrNotFoundError("\\n", *text);

		size_t space;
		try
		{
			space = text->rfind(' ', newline_pos);
		}
		catch (...)
		{
			throw ParseError("Parse Error");
		}

		if (space == std::string::npos)
			throw StrNotFoundError(" ", *text);

		std::string path = text->substr(0, space);
		*text = text->substr(space + 1);
		return path;
	}

	static HTTP_VERSION getProtocol(std::string *text)
	{
		size_t space = text->find('\n');

		if (space == std::string::npos)
			throw StrNotFoundError("\\n", *text);

		std::string proto = text->substr(0, space);
		*text = text->substr(space + 1);
		return strToHTTPVersion(proto);
	}

	static HTTPRequest fromText(std::string httpRequestString)
	{
		HTTPRequest request;
		bool headerParseEnded;
		try
		{
			request.method = getMethod(&httpRequestString);
			request.path = getPath(&httpRequestString);
			request.protocol = getProtocol(&httpRequestString);
			request.headers = parseHeaders(&httpRequestString, &headerParseEnded);
			request.body = httpRequestString.substr(2);
		}
		catch (const ParseError &e)
		{
			throw HTTPRequestParseError();
		}
		return request;
	}

	static HTTPRequest requestFromSocket(TCPSocket client_socket)
	{
		char buffer[BUFFER_SIZE];
		int bytesReceived = 0;
		std::string data;
		HTTPRequest req;
		int count = 0;
		int contentLength = -1;
		bool ended = false;

		while ((bytesReceived = client_socket.receive(buffer, BUFFER_SIZE)) > 0)
		{
			data += std::string(buffer, bytesReceived);
			if (count == 0)
			{
				if (bytesReceived == 0)
				{
					throw NotHttpError();
				}

				req.method = HTTPRequest::getMethod(&data);
				req.path = HTTPRequest::getPath(&data);
				req.protocol = HTTPRequest::getProtocol(&data);
			}
			HTTPHeaders headers = HTTPData::parseHeaders(&data, &ended);
			for (auto const &[key, value] : headers)
			{
				req.headers[key] = value;
			}

			if (ended)
			{
				if (contentLength == -1)
				{
					if (req.headers.count("Content-Length"))
					{
						auto cl = req.headers["Content-Length"];
						if (std::all_of(cl.begin(), cl.end(), ::isdigit))
						{
							contentLength = std::stoi(cl);
						}
						else
						{
							break;
						}
					}
					else
					{
						break;
					}
					contentLength -= data.length() - 2; // the \r\n an the end
				}
				else
				{
					// contentLength -= bytesReceived;
					if (contentLength <= 0)
					{
						break;
					}
				}
			}

			if (bytesReceived < BUFFER_SIZE)
			{
				break;
			}
			count++;
		}
		if (data.length() > 0)
		{
			req.body = data.substr(2); // the \r\n at the end
		}

		return req;
	}

	virtual void print() override
	{
		std::cout << "HTTPRequest object:\n"
				  << toText();
	}
};

class HTTPResponse : public HTTPData
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
		HTTP_VERSION protocol = HTTP_VERSION::HTTP_1_1) : HTTPData(protocol),
														  status(status)
	{
		this->body = body;
		this->headers = headers;
		this->setStatus(status);
		this->target_socket = target_socket;
	}

	std::string getTextStatus()
	{

		return statusToStringMap.at(status);
	}

	int getStatus() { return status; }

	void setStatus(int status)
	{
		if (statusToStringMap.find(status) != statusToStringMap.end())
		{
			this->status = status;
		}
		else
		{
			// to be changed
			throw status;
		}
	}

	std::string toText() override
	{
		std::stringstream ss;
		ss << HTTPVersionToStr(protocol) << " " << status << " " << getTextStatus() << "\n";
		ss << headersToString(headers) << "\n";
		ss << body;
		return ss.str();
	}

	virtual void print() override
	{
		std::cout << "HTTPResponse object:\n"
				  << toText();
	}

	void send()
	{
		send(body.length());
	}
	void send(int contentLength)
	{
		if (!sent)
		{
			headers["Content-Length"] = intToStr(contentLength);
			if (target_socket)
			{
				target_socket->send(toText());
				sent = true;
			}
			else
			{
				// raise exception
				throw SocketNotFoundError();
			}
		}
	}
	void sendMore(char *buf, int size)
	{
		if (sent)
		{
			if (target_socket)
				target_socket->send(buf, size);
			else
			{
				// raise exception
				throw SocketNotFoundError();
			}
		}
	}
	static const std::unordered_map<int, std::string> statusToStringMap;
};

typedef std::function<void(HTTPRequest *, HTTPResponse *)> RouteHandler;
