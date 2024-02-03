#pragma once

#include <Windows.h>
#include <string>
#include <iostream>

#ifndef PIPE_BUFSIZE
#define PIPE_BUFSIZE 1024
#endif
struct Pipe
{
	HANDLE hPipe = INVALID_HANDLE_VALUE;
	Pipe() = default;
	void create(std::string pipeName)
	{
		hPipe = CreateNamedPipeA(
			pipeName.c_str(),			// pipe name
			PIPE_ACCESS_DUPLEX,			// read/write access
			PIPE_TYPE_MESSAGE |			// message type pipe
				PIPE_READMODE_MESSAGE | // message-read mode
				PIPE_WAIT,				// blocking mode
			PIPE_UNLIMITED_INSTANCES,	// max. instances
			PIPE_BUFSIZE,				// output buffer size
			PIPE_BUFSIZE,				// input buffer size
			0,							// client time-out
			NULL);
		if (hPipe == INVALID_HANDLE_VALUE)
		{
			std::cerr << "CreateNamedPipe failed, GLE=" << GetLastError() << std::endl;
			exit(1);
		}
	}
	void connect(std::string pipeName)
	{
		hPipe = CreateFileA(
			pipeName.c_str(), // pipe name
			GENERIC_READ |	  // read and write access
				GENERIC_WRITE,
			0,			   // no sharing
			NULL,		   // default security attributes
			OPEN_EXISTING, // opens existing pipe
			0,			   // default attributes
			NULL);		   // no template file
		if (hPipe == INVALID_HANDLE_VALUE)
		{
			std::cerr << "CreateFileA failed, GLE=" << GetLastError() << std::endl;
			exit(1);
		}
	}
	bool listen()
	{
		return ConnectNamedPipe(hPipe, NULL) ? true : (GetLastError() == ERROR_PIPE_CONNECTED);
	}
	int read(char *buffer, int bufsize)
	{
		DWORD cbRead;
		if (ReadFile(
				hPipe,	 // pipe handle
				buffer,	 // buffer to receive reply
				bufsize, // size of buffer
				&cbRead, // number of bytes read
				NULL)	 // not overlapped
		)
			return cbRead;
		else
		{
			return -1;
		}
	}
	int write(char *buffer, int bufsize)
	{
		DWORD size;
		if (WriteFile(
				hPipe,	 // pipe handle
				buffer,	 // message
				bufsize, // message length
				&size,	 // bytes written
				NULL)	 // not overlapped
		)
			return size;
		else
			return -1;
	}
	void close()
	{
		CloseHandle(hPipe);
	}
};

