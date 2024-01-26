#pragma once
#include "HTTPReqRes.h"
#include <fstream>

namespace ZServer
{
    void DefaultLogger(HTTPRequest *req, HTTPResponse *resp);
    void DefaultHandleNotfound(HTTPRequest *req, HTTPResponse *res);
    void DefaultHandleServerError(HTTPRequest *req, HTTPResponse *res);

    void sendFile(HTTPResponse *res, std::string fullPath);

} // namespace ZServer

