#include <server/HTTPServer.hpp>

int main(int argc, char *argv[])
{
	// int newargc = getCorrectArgc(argc, argv); // now can use the argv and newargc to get the correct args

	HTTPServer s;
	s.at("/", [](HTTPRequest *req, HTTPResponse *res)
		 { res->body = "Hello World"; });
	s.at("/time", [](HTTPRequest *req, HTTPResponse *res)
		 { 
		time_t now = time(0);
		tm *ltm = localtime(&now);
		res->body = "Current time: " + std::to_string(ltm->tm_hour) + ":" + std::to_string(ltm->tm_min) + ":" + std::to_string(ltm->tm_sec); });
	s.workerNum = 5;
	s.port = 8080;
	s.address = "0.0.0.0";
	return s.start(argc, argv);
}
