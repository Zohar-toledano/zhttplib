#pragma once
#include <windows.h>
#include <exception>
class Mutex
{
	HANDLE hMutex;

	void create(LPTSTR name);
	void open(LPTSTR name);
	void check();
	void acquire(int howLong = INFINITE);
	void release();
	void close();
};
