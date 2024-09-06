#include "TCPSocket.hpp"

#include <stdexcept>
#include <utility>
#include <sstream>


BaseSocket::BaseSocket()
{
	if (active == 0)
	{
#ifdef _WIN32
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			throw std::runtime_error("Error initializing Winsock.");
		}
#else
#endif
	}
	++active;
}

void BaseSocket::close()
{
	--active;
	if (active == 0)
	{
#ifdef _WIN32
		WSACleanup();
#else
#endif
	}
}


TCPSocket::TCPSocket() : BaseSocket()
{
	initSocket();
}

TCPSocket::TCPSocket(int socket) : BaseSocket()
{
	socket_ = socket;
}

void TCPSocket::initSocket()
{
	socket_ = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_ == -1)
	{
		throw std::runtime_error("Error creating socket.");
	}
}


void TCPSocket::bind(const std::string &host, int port)
{
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);

#ifdef _WIN32
	server_address.sin_addr.S_un.S_addr = inet_addr(host.c_str());
#else
	server_address.sin_addr.s_addr = inet_addr(host.c_str());
#endif

	if (::bind(socket_, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
	{
		throw std::runtime_error("Error binding socket.");
	}
}

void TCPSocket::listen(int backlog = SOMAXCONN)
{
	if (::listen(socket_, backlog) == -1)
	{
		throw std::runtime_error("Error listening on socket.");
	}
}

AcceptedSocket TCPSocket::accept()
{
	sockaddr_in add;
	int addrlen = sizeof(add);
	int accepted_sock = ::accept(socket_, (struct sockaddr *)&add, &addrlen);
	if (accepted_sock == -1)
	{
		throw AcceptError();
	}
	return AcceptedSocket {TCPSocket(accepted_sock), translateAddress(add)};
}

int TCPSocket::receive(char *buffer, int bufferSize)
{
	int bytesRead = recv(socket_, buffer, bufferSize, 0);
	if (bytesRead == -1)
	{
		throw std::runtime_error("Error receiving data from socket.");
	}
	return bytesRead;
}

void TCPSocket::send(const char *data, int dataSize)
{
	if (::send(socket_, data, dataSize, 0) == -1)
	{
		throw std::runtime_error("Error sending data to socket.");
	}
}

void TCPSocket::send(const std::string &data)
{
	send(data.c_str(), data.size());
}

void TCPSocket::close()
{
#ifdef _WIN32
	closesocket(socket_);
#else
	::close(socket_);
#endif
	BaseSocket::close();
}

Address TCPSocket::getAddr()
{
	sockaddr_in localAddr{};
	int addrLen = sizeof(localAddr);
	if (getsockname(socket_, reinterpret_cast<sockaddr *>(&localAddr), &addrLen) == 0)
	{
		return translateAddress(localAddr);
	}
	throw std::runtime_error("Error getting local address and port.");
}

void TCPSocket::connect(const std::string &host, int port)
{
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);

#ifdef _WIN32
	server_address.sin_addr.S_un.S_addr = inet_addr(host.c_str());
#else
	server_address.sin_addr.s_addr = inet_addr(host.c_str());
#endif

	if (::connect(socket_, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
	{
		throw std::runtime_error("Error connecting to server.");
	}
}

#ifdef _WIN32
TCPSocket::TCPSocket(WSAPROTOCOL_INFOW wsaProtocolInfo)
{
	int duplicatedSocket = WSASocketW(
		FROM_PROTOCOL_INFO,
		FROM_PROTOCOL_INFO,
		FROM_PROTOCOL_INFO,
		&wsaProtocolInfo,
		0, 0);
	socket_ = duplicatedSocket;

	if (duplicatedSocket == INVALID_SOCKET)
	{
		int errorCode = WSAGetLastError();
		std::stringstream ss;

		ss << "WSASocketW failed with error: " << errorCode << std::endl;
		throw std::runtime_error(ss.str());
	}
}

WSAPROTOCOL_INFOW TCPSocket::transfer(int pid)
{
	WSAPROTOCOL_INFOW wsaProtocolInfo;
	int duplicateSocket = WSADuplicateSocketW(socket_, pid, &wsaProtocolInfo);
	if (duplicateSocket == SOCKET_ERROR)
	{
		throw std::runtime_error("Cannot transfer socket\n");
	}
	return wsaProtocolInfo;
}
#endif // _WIN32

Address TCPSocket::translateAddress(sockaddr_in a)
{
	return {std::string(inet_ntoa(a.sin_addr)), ntohs(a.sin_port)};
}

short BaseSocket::active = 0;

