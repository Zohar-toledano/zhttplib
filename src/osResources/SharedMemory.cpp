#include <osResources/SharedMemory.hpp>


void SharedMemory::initVars()
{
}
int SharedMemory::calcSize()
{
	return 0;
}
void SharedMemory::unInitVars()
{
}

void SharedMemory::map(int size)
{
	baseAddress = MapViewOfFile(
		hFileMapping,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		size);
	initVars();
}

void *SharedMemory::getBaseAddress()
{
	return baseAddress;
}
void SharedMemory::create(LPCTSTR name)
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

void SharedMemory::open(LPCTSTR name)
{
	hFileMapping = OpenFileMapping(
		FILE_MAP_READ | FILE_MAP_WRITE,
		FALSE,
		name);
	map(calcSize());
}
void SharedMemory::bloff()
{
	baseAddress = malloc(calcSize());
	initVars();
}
void SharedMemory::close()
{
	UnmapViewOfFile(baseAddress);
	CloseHandle(hFileMapping);
	unInitVars();
}
