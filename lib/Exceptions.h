#pragma once
#include <exception>
#include <string>
#include "base.hpp"
namespace ZServer
{
	class ZBasicException : public std::exception
	{
		std::string text;

	public:
		ZBasicException(std::string text = "") : std::exception(), text(text) {}

		const char *what() const noexcept override
		{
			if (text.length() == 0)
			{
				return "ZServer Basic exception"; // Custom error message
			}
			return text.c_str();
		}
	};

	// parsing
	class ParseError : public ZBasicException
	{
	public:
		ParseError(std::string t) : ZBasicException(t) {}
	};

	class StrNotFoundError : public ParseError
	{
	public:
		StrNotFoundError(std::string c, std::string text) : ParseError("Can't find \"" + c + "\" in " + text + "!\n") {}
	};

	class HTTPRequestParseError : public ParseError
	{
	public:
		HTTPRequestParseError() : ParseError("Cant Parse HTTP request") {}
	};

	// sockets
	class SocketError : public ZBasicException
	{
	public:
		SocketError(std::string t = "General SocketError") : ZBasicException(t) {}
	};
	class SocketNotFoundError : public SocketError
	{
	public:
		SocketNotFoundError() : SocketError("The socket not found!") {}
	};

	class SocketAddrInfoFailed : public SocketError
	{
	public:
		SocketAddrInfoFailed() : SocketError("Cant Get address info o socket!") {}
	};

	class SocketEstablishFailed : public SocketError
	{
	public:
		SocketEstablishFailed(int errorCode) : SocketError("Failed creating socket: " + std::string(intToStr(errorCode))) {}
	};

	class SocketListenError : public SocketError
	{
	public:
		SocketListenError(int errorCode) : SocketError("Failed to listen on socket: " + std::string(intToStr(errorCode))) {}
	};

	class SocketBindError : public SocketError
	{
	public:
		SocketBindError(int errorCode) : SocketError("Failed to bind socket: " + std::string(intToStr(errorCode))) {}
	};
	class HTTPError : public ZBasicException
	{
	public:
		HTTPError(std::string t) : ZBasicException(t) {}
	};
	class NotHttpError : public HTTPError
	{
	public:
		NotHttpError() : HTTPError("The protocol is not http") {}
	};
}
