#include <client/client.hpp>

int getDefaultPort(HTTP_VERSION version)
{
	switch (version)
	{
	case HTTP_VERSION::HTTP_1_0:
	case HTTP_VERSION::HTTP_1_1:
	case HTTP_VERSION::HTTP_2_0:
		return 80;
	}
}

URL::URL(std::string url)
{
	std::regex url_regex(R"(^(?:([^:/?#]+):)?(?://([^:/?#]*)(?::(\d+))?)?([^?#]*)(?:\?([^#]*))?(?:#(.*))?)");
	std::smatch match;

	if (std::regex_match(url, match, url_regex))
	{
		protocol = match[1].str();
		host = match[2].str(); // Extract host without port
		route = match[4].str();
		query = match[5].str();
		fragment = match[6].str();
		int qm;
		if ((qm = fragment.find("?")) != -1)
		{
			query = fragment.substr(qm + 1);
			fragment = fragment.substr(0, qm);
		}

		// Extract port separately
		try
		{
			port = std::stoi(match[3].str());
		}
		catch (const std::invalid_argument &e)
		{
			std::cerr << "Invalid port number in URL: " << url << std::endl;
		}
	}
	else
	{
		std::cerr << "Invalid URL format: " << url << std::endl;
	}
}

HTTPResponse HTTPClient::Request(
	std::string url,
	HTTP_METHOD method,
	HTTPHeaders headers,
	std::string body,
	StringMap URL_params,
	HTTP_VERSION version)
{
	URL url_(url);
	// TODO validations

	HTTPRequest req(
		url_.route,
		method,
		headers,
		body,
		URL_params,
		version);
	if (url_.port == -1)
	{
		url_.port == getDefaultPort(version);
	}

	auto sock_ = TCPSocket();
	sock_.connect(url_.host, url_.port);
	sock_.send(req.toText());

	// TODO recv
	return HTTPResponse();
}

HTTPResponse HTTPClient::request(
	std::string url,
	HTTP_METHOD method,
	HTTPHeaders headers,
	std::string body,
	StringMap URL_params)
{
	HTTPHeaders h;
	for (auto const &[key, value] : this->headers)
	{
		h[key] = value;
	}
	for (auto const &[key, value] : headers)
	{
		h[key] = value;
	}
	return HTTPClient::Request(
		url,
		method,
		h,
		body,
		URL_params,
		this->version);
}
