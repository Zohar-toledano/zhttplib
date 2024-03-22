#include "server/HTTPServerProps.h"

// Example function exported from the DLL
using namespace ZServer;

extern "C" __declspec(dllexport) HTTPServerProps *GetProps()
{
    static auto props = new HTTPServerProps();
    props->routeMap.add(
        R"(/)",
        [](HTTPRequest *req, HTTPResponse *resp)
        {
            resp->headers["Content-Type"] = "text/plain";
            resp->body = "test";
            resp->setStatus(200);
        },
        false);
    return props;
}

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
