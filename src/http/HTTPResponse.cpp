#include <http/HTTPResponse.hpp>

HTTPResponse::HTTPResponse(
	std::string body,
	HTTPHeaders headers,
	TCPSocket *target_socket,
	int status,
	HTTP_VERSION protocol) : HTTPBase(protocol),
													  status(status)
{
	this->body = body;
	this->headers = headers;
	this->setStatus(status);
	this->target_socket = target_socket;
}

std::string HTTPResponse::getTextStatus()
{

	return statusToStringMap.at(status);
}

int HTTPResponse::getStatus() { return status; }

void HTTPResponse::setStatus(int status)
{
	if (statusToStringMap.find(status) != statusToStringMap.end())
	{
		this->status = status;
	}
	else
	{
		// to be changed
		throw status;
	}
}

std::string HTTPResponse::toText()
{
	std::stringstream ss;
	ss << HTTPVersionToStr(protocol) << " " << status << " " << getTextStatus() << "\n";
	ss << headersToString(headers) << "\n";
	ss << body;
	return ss.str();
}

void HTTPResponse::print()
{
	std::cout << "HTTPResponse object:\n"
			  << toText();
}

void HTTPResponse::send()
{
	send(body.length());
}
void HTTPResponse::send(int contentLength)
{
	if (!sent)
	{
		headers["Content-Length"] = intToStr(contentLength);
		if (target_socket)
		{
			target_socket->send(toText());
			sent = true;
		}
		else
		{
			// raise exception
			throw SocketNotFoundError();
		}
	}
}
void HTTPResponse::sendMore(char *buf, int size)
{
	if (sent)
	{
		if (target_socket)
			target_socket->send(buf, size);
		else
		{
			// raise exception
			throw SocketNotFoundError();
		}
	}
}
