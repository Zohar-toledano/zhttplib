#include <iostream>

#include "argparse.hpp" // before windows.h

#include "server/Server.h"
#include "WorkerRoutine.hpp"

struct ExecuteArgs : public argparse::Args
{
	int &id = kwarg("id", "An ID for slave").set_default(0);
	std::string &memory = kwarg("m,memory", "Shared memory name");
	std::string &dllPath = kwarg("d,dll", "The dll path.");
};
int main(int argc, char *argv[])
{

	auto args = argparse::parse<ExecuteArgs>(argc, argv);
	std::cout << args.dllPath;

	typedef void (*GW)(void);

	HMODULE hModule = LoadLibraryA(args.dllPath.c_str());
	if (hModule == NULL)
	{
		std::cerr << "Failed to load DLL. Error code: " << GetLastError() << std::endl;
		return 1;
	}

	GW GetWelcomeMessage = (GW)GetProcAddress(hModule, "HelloWorld");
	if (GetWelcomeMessage == NULL)
	{
		std::cerr << "Failed to get address of HelloWorld function. Error code: " << GetLastError() << std::endl;
		FreeLibrary(hModule);
		return 1;
	}

	GetWelcomeMessage();
	WorkerRoutine w(args.id, args.memory);
	// w.workerMain();
}
