#pragma once

#define BUFFERSIZE 1024

#include <winsock2.h>
// #include <WS2tcpip.h>
#include <windows.h>

#include <string>
#include <sstream>
#include "Exceptions.h"
#include <iostream>
#ifdef _WIN32
/* Windows-specific headers */
#pragma comment(lib, "ws2_32.lib")
// #else
// /* POSIX-specific headers */
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <unistd.h>
#endif
namespace ZServer
{
	class SocketTCP
	{
		sockaddr_in address;
		static bool initalized;

	public:
		int sockfd;
		SocketTCP(int sock, sockaddr_in addr) : sockfd(sock), address(addr)
		{
		}
		SocketTCP(int sock) : sockfd(sock)
		{
		}
#ifdef _WIN32
		SocketTCP(WSAPROTOCOL_INFOW wsaProtocolInfo)
		{
			int duplicatedSocket = WSASocketW(
				FROM_PROTOCOL_INFO,
				FROM_PROTOCOL_INFO,
				FROM_PROTOCOL_INFO,
				&wsaProtocolInfo,
				0, 0);
			if (duplicatedSocket == INVALID_SOCKET)
			{
				int errorCode = WSAGetLastError();
				std::stringstream ss;

				ss << "WSASocketW failed with error: " << errorCode << std::endl;
				throw SocketError(ss.str());
			}
			else
			{
				sockfd = duplicatedSocket;
			}
		}

		WSAPROTOCOL_INFOW transfer(int pid)
		{
			WSAPROTOCOL_INFOW wsaProtocolInfo;
			int duplicateSocket = WSADuplicateSocketW(sockfd, pid, &wsaProtocolInfo);
			if (duplicateSocket == SOCKET_ERROR)
			{
				throw SocketError("Cannot transfer socket\n");
			}
			return wsaProtocolInfo;
		}
#endif // _WIN32
		SocketTCP()
		{
			init();
#ifdef _WIN32
			// WSADATA wsaData;
			// WSAStartup(MAKEWORD(2, 2), &wsaData);
			sockfd = socket(AF_INET, SOCK_STREAM, 0);
#else
			sockfd = socket(AF_INET, SOCK_STREAM, 0);
#endif
			memset(&address, 0, sizeof(address));
			address.sin_family = AF_INET;
			address.sin_addr.s_addr = INADDR_ANY;
		};

		void bind(int port)
		{
			address.sin_port = htons(port);
			::bind(sockfd, (struct sockaddr *)&address, sizeof(address));
		}
		SocketTCP accept()
		{
			sockaddr_in add;
			int addrlen = sizeof(add);
			int accepted_sock = ::accept(sockfd, (struct sockaddr *)&add, &addrlen);

			return SocketTCP(accepted_sock, add);
		}

		void listen(int backlog = SOMAXCONN)
		{
			::listen(sockfd, backlog);
		}

		size_t send(const char *buf, size_t len, int flags = 0)
		{
			return ::send(sockfd, buf, len, flags);
		}

		size_t send(std::string data)
		{
			return send(data.c_str(), data.size());
		}

		size_t recv(char *buf, size_t len, int flags = 0)
		{
			return ::recv(sockfd, buf, len, flags);
		}

		void close()
		{
#ifdef _WIN32
			closesocket(sockfd);
#else
			::close(sockfd);
#endif
		}

		bool isValid()
		{
			return true;
		}

		bool connect(const std::string &ip, int port)
		{
			// 			struct sockaddr_in server_address;
			// 			memset(&server_address, 0, sizeof(server_address));
			// 			server_address.sin_family = AF_INET;
			// 			server_address.sin_port = htons(port);
			// 			address = server_address;

			// #ifdef _WIN32
			// 			server_address.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
			// #else
			// 			server_address.sin_addr.s_addr = inet_addr(ip.c_str());
			// #endif

			// 			return ::connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) >= 0;
			return true;
		}
		static void init()
		{
			if (!initalized)
			{

#ifdef _WIN32
				WSADATA wsaData;
				if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
				{
					std::cerr << "WSAStartup failed.\n";
					exit(1);
				}
#else

#endif // _WIN32
				initalized = true;
			}
		}
	};

	// 	class SocketTCP
	// 	{
	// 		int sockfd;
	// 		sockaddr_in address;
	// 		static bool initalized;

	// 	public:
	// 		SocketTCP(int sock, sockaddr_in addr) : sockfd(sock), address(addr)
	// 		{
	// 		}
	// 		SocketTCP()
	// 		{
	// 			init();
	// #ifdef _WIN32
	// 			// WSADATA wsaData;
	// 			// WSAStartup(MAKEWORD(2, 2), &wsaData);
	// 			sockfd = socket(AF_INET, SOCK_STREAM, 0);
	// #else
	// 			sockfd = socket(AF_INET, SOCK_STREAM, 0);
	// #endif
	// 			memset(&address, 0, sizeof(address));
	// 			address.sin_family = AF_INET;
	// 			address.sin_addr.s_addr = INADDR_ANY;
	// 		};

	// 		void bind(int port)
	// 		{
	// 			address.sin_port = htons(port);
	// 			::bind(sockfd, (struct sockaddr *)&address, sizeof(address));
	// 		}

	// 		void listen(int backlog = SOMAXCONN)
	// 		{
	// 			::listen(sockfd, backlog);
	// 		}

	// 		SocketTCP accept()
	// 		{
	// 			sockaddr_in add;
	// 			int addrlen = sizeof(add);
	// 			int accepted_sock = ::accept(sockfd, (struct sockaddr *)&add, &addrlen);

	// 			return SocketTCP(accepted_sock, add);
	// 		}

	// 		size_t send(const char *buf, size_t len, int flags = 0)
	// 		{
	// 			return ::send(sockfd, buf, len, flags);
	// 		}

	// 		size_t send(std::string data)
	// 		{
	// 			return send(data.c_str(), data.size());
	// 		}

	// 		size_t recv(char *buf, size_t len, int flags = 0)
	// 		{
	// 			return ::recv(sockfd, buf, len, flags);
	// 		}

	// 		bool connect(const std::string &ip, int port)
	// 		{
	// 			struct sockaddr_in server_address;
	// 			memset(&server_address, 0, sizeof(server_address));
	// 			server_address.sin_family = AF_INET;
	// 			server_address.sin_port = htons(port);
	// 			address = server_address;

	// #ifdef _WIN32
	// 			server_address.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
	// #else
	// 			server_address.sin_addr.s_addr = inet_addr(ip.c_str());
	// #endif

	// 			return ::connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) >= 0;
	// 		}
	// 		~SocketTCP()
	// 		{
	// 			// close();
	// 			// closes before actually deleted!
	// 		}

	// 		void close()
	// 		{
	// #ifdef _WIN32
	// 			closesocket(sockfd);
	// #else
	// 			close(sockfd);
	// #endif
	// 		}

	// 		static void init()
	// 		{
	// 			if (!initalized)
	// 			{

	// #ifdef _WIN32
	// 				WSADATA wsaData;
	// 				if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	// 				{
	// 					std::cerr << "WSAStartup failed.\n";
	// 					exit(1);
	// 				}
	// #else
	// #ifdef __unix__
	// #endif // __unix__
	// #endif // _WIN32
	// 				initalized = true;
	// 			}
	// 		}

	// 		// 		static void uninit()
	// 		// 		{
	// 		// 			if (initalized)
	// 		// 			{

	// 		// #ifdef _WIN32
	// 		// 				WSACleanup();
	// 		// #else
	// 		// #ifdef __unix__

	// 		// #endif // __unix__
	// 		// #endif // _WIN32

	// 		// 				initalized = false;
	// 		// 			}
	// 		// 		}

	// 		bool isValid()
	// 		{
	// 			// #ifdef _WIN32
	// 			// 			if (sockfd == INVALID_SOCKET)
	// 			// 			{
	// 			// 				return false;
	// 			// 			}
	// 			// #else
	// 			// #ifdef __unix__

	// 			// #endif // __unix__
	// 			// #endif // _WIN32
	// 			// 			return true;
	// 			return true;
	// 		}
	// 	};

} // namespace ZServer
