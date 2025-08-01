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
	void create(std::string pipeName);
	void open(std::string pipeName);
	bool listen();
	int read(void *buffer, int bufsize);
	int write(void *buffer, int bufsize);
	void close();
};

