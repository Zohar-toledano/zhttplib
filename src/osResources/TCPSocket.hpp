#pragma once
#include <string>
#include <exception>

#ifdef _WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

struct Address
{
	std::string host;
	int port;
};

struct AcceptedSocket;
class BaseSocket
{
protected:
	int socket_;

private:
	static short active;

public:
	BaseSocket();
	virtual void close();
};

class TCPSocket : public BaseSocket
{
public:
	TCPSocket();
	TCPSocket(int socket);
	void initSocket();
	void bind(const std::string &host, int port);
	void listen(int backlog);
	AcceptedSocket accept();
	int receive(char *buffer, int bufferSize);
	void send(const char *data, int dataSize);
	void send(const std::string &data);
	void close() override;
	Address getAddr();
	void connect(const std::string &host, int port);
	int getInt() { return socket_; }

#ifdef _WIN32
	TCPSocket(WSAPROTOCOL_INFOW wsaProtocolInfo);
	WSAPROTOCOL_INFOW transfer(int pid);
#endif // _WIN32
private:
	Address translateAddress(sockaddr_in a);
};

struct AcceptedSocket
{
	TCPSocket socket;
	Address address;
};
class AcceptError : public std::exception
{
public:
	std::string what()
	{
		return "Error accepting connection.";
	}
};