#include <Windows.h>
#include <iostream>
// #include "server/HTTPServerProps.h"

// Example function exported from the DLL
// using namespace ZServer;
extern "C" __declspec(dllexport) void HelloWorld()
{
    MessageBoxA(NULL, "Hello World from DLL!", "DLL Message", MB_OK);
}

// extern "C" __declspec(dllexport) HTTPServerProps* GetProps()
// {
// 	auto props = new HTTPServerProps();
//     return props;

// }

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
