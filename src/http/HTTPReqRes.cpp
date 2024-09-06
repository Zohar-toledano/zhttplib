#include "HTTPReqRes.h"
const std::unordered_map<int, std::string> HTTPResponse::statusToStringMap{
	{100, "Continue"},
	{101, "Switching protocols"},
	{102, "Processing"},
	{103, "Early Hints"},
	// 2xx Succesful
	{200, "OK"},
	{201, "Created"},
	{202, "Accepted"},
	{203, "Non-Authoritative Information"},
	{204, "No Content"},
	{205, "Reset Content"},
	{206, "Partial Content"},
	{207, "Multi-Status"},
	{208, "Already Reported"},
	{226, "IM Used"},
	// 3xx Redirection
	{300, "Multiple Choices"},
	{301, "Moved Permanently"},
	{302, "Found (Previously \" Moved Temporarily \")"},
	{303, "See Other"},
	{304, "Not Modified"},
	{305, "Use Proxy"},
	{306, "Switch Proxy"},
	{307, "Temporary Redirect"},
	{308, "Permanent Redirect"},
	// 4xx Client Error
	{400, "Bad Request"},
	{401, "Unauthorized"},
	{402, "Payment Required"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{405, "Method Not Allowed"},
	{406, "Not Acceptable"},
	{407, "Proxy Authentication Required"},
	{408, "Request Timeout"},
	{409, "Conflict"},
	{410, "Gone"},
	{411, "Length Required"},
	{412, "Precondition Failed"},
	{413, "Payload Too Large"},
	{414, "URI Too Long"},
	{415, "Unsupported Media Type"},
	{416, "Range Not Satisfiable"},
	{417, "Expectation Failed"},
	{418, "I'm a Teapot"},
	{421, "Misdirected Request"},
	{422, "Unprocessable Entity"},
	{423, "Locked"},
	{424, "Failed Dependency"},
	{425, "Too Early"},
	{426, "Upgrade Required"},
	{428, "Precondition Required"},
	{429, "Too Many Requests"},
	{431, "Request Header Fields Too Large"},
	{451, "Unavailable For Legal Reasons"},
	// 5xx Server Error
	{500, "Internal Server Error"},
	{501, "Not Implemented"},
	{502, "Bad Gateway"},
	{503, "Service Unavailable"},
	{504, "Gateway Timeout"},
	{505, "HTTP Version Not Supported"},
	{506, "Variant Also Negotiates"},
	{507, "Insufficient Storage"},
	{508, "Loop Detected"},
	{510, "Not Extended"},
	{511, "Network Authentication Required"}};

const StringMap HTTPData::contentTypeByFileExt = {
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
