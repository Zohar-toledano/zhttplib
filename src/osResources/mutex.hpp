#pragma once
#include <windows.h>
#include <exception>
class Mutex
{
	HANDLE hMutex;

	void create(LPTSTR name)
	{
		hMutex = CreateMutex(NULL, FALSE, name);
		check();
	}
	void open(LPTSTR name)
	{
		hMutex = OpenMutex(SYNCHRONIZE, FALSE, name);
		check();
	}
	void check()
	{
		if (hMutex == nullptr)
		{
			throw std::exception("mutex creation failed");
		}
	}
	void acquire(int howLong = INFINITE)
	{
		WaitForSingleObject(hMutex, howLong);
	}
	void release()
	{
		ReleaseMutex(hMutex);
	}
	void close()
	{
		CloseHandle(hMutex);
	}
};
