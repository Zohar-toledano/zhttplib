#include <http/HTTPRequest.hpp>
HTTPRequest::HTTPRequest() : HTTPBase()
{
}
HTTPRequest::HTTPRequest(
	std::string route,
	HTTP_METHOD method = HTTP_METHOD::GET,
	HTTPHeaders headers = {},
	std::string body = "",
	StringMap URL_params = {},
	HTTP_VERSION version = HTTP_VERSION::HTTP_1_1)
{

	// TODO GET params to/from str
	// TODO change send in socket file
	this->path = route;
	this->method = method;
	this->headers = headers;
	this->body = body;
	this->protocol = version;
}
std::string HTTPRequest::toText()
{
	std::stringstream ss;
	ss << HTTPMethodToStr(method)
	   << " " << path << " " << HTTPVersionToStr(protocol) << "\r\n";
	ss << headersToString(headers) << "\r\n";
	ss << body;
	return ss.str();
}
HTTP_METHOD HTTPRequest::getMethod(std::string *text)
{
	size_t space = text->find(" ");
	if (space == std::string::npos)
		throw StrNotFoundError(" ", *text);

	std::string method = text->substr(0, space);
	*text = text->substr(space + 1);
	return strToHTTPMethod(method);
}

std::string HTTPRequest::getPath(std::string *text)
{

	size_t newline_pos = text->find('\n');

	if (newline_pos == std::string::npos)
		throw StrNotFoundError("\\n", *text);

	size_t space;
	try
	{
		space = text->rfind(' ', newline_pos);
	}
	catch (...)
	{
		throw ParseError("Parse Error");
	}

	if (space == std::string::npos)
		throw StrNotFoundError(" ", *text);

	std::string path = text->substr(0, space);
	*text = text->substr(space + 1);
	return path;
}

HTTP_VERSION HTTPRequest::getProtocol(std::string *text)
{
	size_t space = text->find('\n');

	if (space == std::string::npos)
		throw StrNotFoundError("\\n", *text);

	std::string proto = text->substr(0, space);
	*text = text->substr(space + 1);
	return strToHTTPVersion(proto);
}

HTTPRequest HTTPRequest::fromText(std::string httpRequestString)
{
	HTTPRequest request;
	bool headerParseEnded;
	try
	{
		request.method = getMethod(&httpRequestString);
		request.path = getPath(&httpRequestString);
		request.protocol = getProtocol(&httpRequestString);
		request.headers = parseHeaders(&httpRequestString, &headerParseEnded);
		request.body = httpRequestString.substr(2);
	}
	catch (const ParseError &e)
	{
		throw HTTPRequestParseError();
	}
	return request;
}

HTTPRequest HTTPRequest::requestFromSocket(TCPSocket client_socket)
{
	char buffer[BUFFER_SIZE];
	int bytesReceived = 0;
	std::string data;
	HTTPRequest req {};
	int count = 0;
	int contentLength = -1;
	bool ended = false;

	while ((bytesReceived = client_socket.receive(buffer, BUFFER_SIZE)) > 0)
	{
		data += std::string(buffer, bytesReceived);
		if (count == 0)
		{
			if (bytesReceived == 0)
			{
				throw NotHttpError();
			}

			req.method = HTTPRequest::getMethod(&data);
			req.path = HTTPRequest::getPath(&data);
			req.protocol = HTTPRequest::getProtocol(&data);
		}
		HTTPHeaders headers = HTTPBase::parseHeaders(&data, &ended);
		for (auto const &[key, value] : headers)
		{
			req.headers[key] = value;
		}

		if (ended)
		{
			if (contentLength == -1)
			{
				if (req.headers.count("Content-Length"))
				{
					auto cl = req.headers["Content-Length"];
					if (std::all_of(cl.begin(), cl.end(), ::isdigit))
					{
						contentLength = std::stoi(cl);
					}
					else
					{
						break;
					}
				}
				else
				{
					break;
				}
				contentLength -= data.length() - 2; // the \r\n an the end
			}
			else
			{
				// contentLength -= bytesReceived;
				if (contentLength <= 0)
				{
					break;
				}
			}
		}

		if (bytesReceived < BUFFER_SIZE)
		{
			break;
		}
		count++;
	}
	if (data.length() > 0)
	{
		req.body = data.substr(2); // the \r\n at the end
	}

	return req;
}

void HTTPRequest::print()
{
	std::cout << "HTTPRequest object:\n"
			  << toText();
}
