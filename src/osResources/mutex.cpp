#include <osResources/mutex.hpp>

void Mutex::create(LPTSTR name)
{
	hMutex = CreateMutex(NULL, FALSE, name);
	check();
}
void Mutex::open(LPTSTR name)
{
	hMutex = OpenMutex(SYNCHRONIZE, FALSE, name);
	check();
}
void Mutex::check()
{
	if (hMutex == nullptr)
	{
		throw std::exception("mutex creation failed");
	}
}
void Mutex::acquire(int howLong)
{
	WaitForSingleObject(hMutex, howLong);
}
void Mutex::release()
{
	ReleaseMutex(hMutex);
}
void Mutex::close()
{
	CloseHandle(hMutex);
}
