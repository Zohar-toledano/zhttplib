#include <osResources/Pipe.hpp>
#include <sstream>
void Pipe::create(std::string pipeName)
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
		std::stringstream ss;
		ss << "CreateNamedPipe failed, GLE=" << GetLastError();
		throw std::runtime_error(ss.str());
	}
}
void Pipe::open(std::string pipeName)
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
		std::stringstream ss;
		ss << "CreateFileA failed, GLE=" << GetLastError();
		throw std::runtime_error(ss.str());
	}
}
bool Pipe::listen()
{
	return ConnectNamedPipe(hPipe, NULL) ? true : (GetLastError() == ERROR_PIPE_CONNECTED);
}
int Pipe::read(void *buffer, int bufsize)
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
int Pipe::write(void *buffer, int bufsize)
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
void Pipe::close()
{
	CloseHandle(hPipe);
}
