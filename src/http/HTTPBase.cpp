#include <http/HTTPBase.hpp>

const StringMap HTTPBase::contentTypeByFileExt = {
	{"aac", "audio/aac"},
	{"abw", "application/x-abiword"},
	{"arc", "application/x-freearc"},
	{"avif", "image/avif"},
	{"avi", "video/x-msvideo"},
	{"azw", "application/vnd.amazon.ebook"},
	{"bin", "application/octet-stream"},
	{"bmp", "image/bmp"},
	{"bz", "application/x-bzip"},
	{"bz2", "application/x-bzip2"},
	{"cda", "application/x-cdf"},
	{"csh", "application/x-csh"},
	{"css", "text/css"},
	{"csv", "text/csv"},
	{"doc", "application/msword"},
	{"docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
	{"eot", "application/vnd.ms-fontobject"},
	{"epub", "application/epub+zip"},
	{"gz", "application/gzip"},
	{"gif", "image/gif"},
	{"htm, .html", "text/html"},
	{"ico", "image/vnd.microsoft.icon"},
	{"ics", "text/calendar"},
	{"jar", "application/java-archive"},
	{"jpeg, .jpg", "image/jpeg"},
	{"js", "text/javascript"},
	{"json", "application/json"},
	{"jsonld", "application/ld+json"},
	{"mid, .midi", "audio/midi, audio/x-midi"},
	{"mjs", "text/javascript"},
	{"mp3", "audio/mpeg"},
	{"mp4", "video/mp4"},
	{"mpeg", "video/mpeg"},
	{"mpkg", "application/vnd.apple.installer+xml"},
	{"odp", "application/vnd.oasis.opendocument.presentation"},
	{"ods", "application/vnd.oasis.opendocument.spreadsheet"},
	{"odt", "application/vnd.oasis.opendocument.text"},
	{"oga", "audio/ogg"},
	{"ogv", "video/ogg"},
	{"ogx", "application/ogg"},
	{"opus", "audio/opus"},
	{"otf", "font/otf"},
	{"png", "image/png"},
	{"pdf", "application/pdf"},
	{"php", "application/x-httpd-php"},
	{"ppt", "application/vnd.ms-powerpoint"},
	{"pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
	{"rar", "application/vnd.rar"},
	{"rtf", "application/rtf"},
	{"sh", "application/x-sh"},
	{"svg", "image/svg+xml"},
	{"tar", "application/x-tar"},
	{"tif, .tiff", "image/tiff"},
	{"ts", "video/mp2t"},
	{"ttf", "font/ttf"},
	{"txt", "text/plain"},
	{"vsd", "application/vnd.visio"},
	{"wav", "audio/wav"},
	{"weba", "audio/webm"},
	{"webm", "video/webm"},
	{"webp", "image/webp"},
	{"woff", "font/woff"},
	{"woff2", "font/woff2"},
	{"xhtml", "application/xhtml+xml"},
	{"xls", "application/vnd.ms-excel"},
	{"xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
	{"xml", "application/xml"},
	{"xul", "application/vnd.mozilla.xul+xml"},
	{"zip", "application/zip"},
	// {"3gp", "video/3gpp; audio/3gpp if it doesn't contain video"},
	// {"3g2", "video/3gpp2; audio/3gpp2 if it doesn't contain video"},
	{"7z", "application/x-7z-compressed"}};

HTTP_METHOD strToHTTPMethod(std::string method)
{
	std::transform(method.begin(), method.end(), method.begin(), ::toupper);
	if (method == "GET")
		return HTTP_METHOD::GET;
	if (method == "POST")
		return HTTP_METHOD::POST;
	if (method == "OPTIONS")
		return HTTP_METHOD::OPTIONS;
	if (method == "PUT")
		return HTTP_METHOD::PUT;
	if (method == "PATCH")
		return HTTP_METHOD::PATCH;
	if (method == "DELETE")
		return HTTP_METHOD::DELETE_;
}
std::string HTTPMethodToStr(HTTP_METHOD method)
{
	switch (method)
	{
	case HTTP_METHOD::GET:
		return "GET";
	case HTTP_METHOD::POST:
		return "POST";
	case HTTP_METHOD::OPTIONS:
		return "OPTIONS";
	case HTTP_METHOD::PUT:
		return "PUT";
	case HTTP_METHOD::PATCH:
		return "PATCH";
	case HTTP_METHOD::DELETE_:
		return "DELETE";
	}
}

HTTP_VERSION strToHTTPVersion(std::string version)
{
	std::transform(version.begin(), version.end(), version.begin(), ::toupper);
	if (version == "HTTP/1.0")
		return HTTP_VERSION::HTTP_1_0;
	if (version == "HTTP/1.1")
		return HTTP_VERSION::HTTP_1_1;
	if (version == "HTP/2.0")
		return HTTP_VERSION::HTTP_2_0;
}

std::string HTTPVersionToStr(HTTP_VERSION version)
{
	switch (version)
	{
	case HTTP_VERSION::HTTP_1_0:
		return "HTTP/1.0";

	case HTTP_VERSION::HTTP_1_1:
		return "HTTP/1.0";

	case HTTP_VERSION::HTTP_2_0:
		return "HTTP/1.0";
	}
}

HTTPBase::HTTPBase(HTTP_VERSION protocol) : protocol(protocol)
{
}

HTTPHeaders HTTPBase::parseHeaders(std::string *text, bool *ended)
{
	HTTPHeaders headers = HTTPHeaders();
	size_t startLine = 0;
	size_t endLine = 0;
	size_t headersEnd = text->find("\n\n");
	if (headersEnd == std::string::npos)
	{
		headersEnd = text->length();
	}
	size_t total = 0;
	while (total < headersEnd)
	{
		endLine = text->find("\r\n");
		total += endLine + 2;
		if (endLine == std::string::npos)
		{
			*ended = false;
			return headers;
		}
		size_t nekodotaim = text->find(":");
		if (nekodotaim == std::string::npos)
		{
			continue;
		}
		if (total != headersEnd)
		{
			std::string key = text->substr(0, nekodotaim);
			std::string value = text->substr(nekodotaim + 1, endLine - nekodotaim - 1);
			if (value.starts_with(" "))
			{
				value = value.substr(1);
			}

			headers[key] = value;
			// std::cout << "key: " << key << " value: " << value << "\n";
			*text = text->substr(endLine + 2);
		}
		else
		{
			*ended = false;
			return headers;
		}
	}
	*ended = true;
	return headers;
}
std::string HTTPBase::headersToString(HTTPHeaders headers)
{
	std::stringstream ss;
	for (auto [key, value] : headers)
	{
		ss << key << ": " << value << "\n";
	}
	return ss.str();
}
void HTTPBase::print() { std::cout << "HTTPBase object"; }
