#pragma once

#include <windows.h>

class SharedMemory
{
	HANDLE hFileMapping;

protected:
	void *baseAddress;

protected:
	virtual void initVars();
	virtual int calcSize();
	virtual void unInitVars();

private:
	void map(int size);

public:
	SharedMemory() =default;
	void *getBaseAddress();
	void create(LPCTSTR name);

	void open(LPCTSTR name);
	void bloff();
	void close();
};
