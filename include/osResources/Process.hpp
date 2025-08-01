#pragma once

#include <Windows.h>
#include <string>
#include <iostream>

struct Process
{
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	Process() = default;
	Process(std::string command);
	void wait(DWORD time);
	void close();
	inline int getPID()
	{
		return (int)pi.dwProcessId;
	}
};
