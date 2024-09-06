#pragma once

#include <windows.h>

class SharedMemory
{
	HANDLE hFileMapping;

protected:
	void *baseAddress;

protected:
	virtual void initVars()
	{
	}
	virtual int calcSize()
	{
		return 0;
	}
	virtual void unInitVars()
	{
	}

private:
	void map(int size)
	{
		baseAddress = MapViewOfFile(
			hFileMapping,
			FILE_MAP_ALL_ACCESS,
			0,
			0,
			size);
		initVars();
	}

public:
	SharedMemory() {};
	void *getBaseAddress()
	{
		return baseAddress;
	}
	void create(LPCTSTR name)
	{
		auto size = calcSize();
		hFileMapping = CreateFileMapping(
			INVALID_HANDLE_VALUE,
			nullptr,
			PAGE_READWRITE,
			0,
			size,
			name);
		map(size);
	}

	void open(LPCTSTR name)
	{
		hFileMapping = OpenFileMapping(
			FILE_MAP_READ | FILE_MAP_WRITE,
			FALSE,
			name);
		map(calcSize());
	}
	void bloff()
	{
		baseAddress = malloc(calcSize());
		initVars();
	}
	void close()
	{
		UnmapViewOfFile(baseAddress);
		CloseHandle(hFileMapping);
		unInitVars();
	}
};
