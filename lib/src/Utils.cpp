#include "Utils.h"
namespace ZServer
{
	void DefaultLogger(HTTPRequest *req, HTTPResponse *resp)
	{
		std::cout << req->path << " -> " << resp->getStatus() << std::endl;
	}
	void DefaultHandleNotfound(HTTPRequest *req, HTTPResponse *res)
	{
		res->setStatus(404);
	}

	void DefaultHandleServerError(HTTPRequest *req, HTTPResponse *res)
	{
		res->setStatus(500);
	}

	void sendFile(HTTPResponse *res, std::string fullPath)
	{
		std::ifstream file(
			fullPath.c_str(),
			std::ios::binary); // Open in binary mode for consistency
		if (!file.is_open())
		{
			res->setStatus(400);
			res->body = "cannot open file";
			res->send();
			return;
		}

		file.seekg(0, std::ios::end);				   // Seek to the end of the file
		int fileSize = static_cast<int>(file.tellg()); // Get size as int (check for overflow if needed)
		file.seekg(0, std::ios::beg);				   // Rewind to the beginning
		res->send(fileSize);
		char buffer[BUFFERSIZE];
		size_t bytesRead;

		while ((bytesRead = file.read(buffer, sizeof(buffer)).gcount()) > 0)
		{
			res->sendMore(buffer, bytesRead);
		}

		file.close();
	}

} // namespace ZServer
