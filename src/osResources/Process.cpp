#include <osResources/Process.hpp>

Process::Process(std::string command)
{
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Start the child process.
	if (!CreateProcessA(NULL,					// No module name (use command line)
						(LPSTR)command.c_str(), // Command line
						NULL,					// Process handle not inheritable
						NULL,					// Thread handle not inheritable
						FALSE,					// Set handle inheritance to FALSE
						0,						// No creation flags
						NULL,					// Use parent's environment block
						NULL,					// Use parent's starting directory
						&si,					// Pointer to STARTUPINFO structure
						&pi)					// Pointer to PROCESS_INFORMATION structure
	)
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		exit(1);
	}
}
void Process::wait(DWORD time)
{
	WaitForSingleObject(pi.hProcess, time);
}
void Process::close()
{
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}
