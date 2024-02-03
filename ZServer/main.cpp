#define NOMINMAX
#include <iostream>
#include <sstream>
#include <string>
#include "argparse.hpp"

#include <server/Server.h>
using namespace ZServer;

#include <multiproccesing/WorkerRoutine.hpp>

struct MasterArgs : public argparse::Args
{
    int &slaves_num = kwarg("s,slaves", "Number of slaves for master process").set_default(1);
    std::string &memory = kwarg("m,memory", "Shared memory name");

    void welcome() override
    {
        std::cout << "Runs as a master." << std::endl;
    }
    int run(std::string path)
    {
        std::cout<<path;
        HTTPServer server(8080,memory,path,1);

        server.at(
            R"(/products/<id:\d+>/test)",
            [](HTTPRequest *req, HTTPResponse *resp)
            {
                resp->headers["Content-Type"] = "text/plain";
                resp->body = "test";
                std::cout << "test" << std::endl;
            });
        server.at(
            "/JSON",
            [](HTTPRequest *req, HTTPResponse *resp)
            {
                json ex3 = {
                    {"happy", {true, "false", 5}},
                    {"pi", 3.141},
                };
                resp->body = ex3.dump();
            });
        server.at(R"(/file)", [](HTTPRequest *req, HTTPResponse *resp)
                  { sendFile(
                        resp,
                        "C:\\Users\\Zohar\\Downloads\\Zserver\\lib\\include\\HTTPReqRes.h"); });

        server.addMiddleware(
            [](HTTPRequest *req, HTTPResponse *resp)
            { resp->headers["server"] = "ZServer"; },
            HTTPServerProps::MiddlewarePosition::Before);
        server.run();

        return 0;
    }
};

struct WorkerArgs : public argparse::Args
{
    int &id = kwarg("id", "An ID for slave").set_default(0);
    std::string &memory = kwarg("m,memory", "Shared memory name");

    void welcome() override
    {
        std::cout << "Runs as a slave." << std::endl;
    }
    int run() override
    {
        std::cout<<"running"<<std::endl;
        WorkerRoutine w(id, memory);
        w.workerMain();
        return 0;
    }
};

struct ExecuteArgs : public argparse::Args
{
    // both
    MasterArgs &master = subcommand("master");
    WorkerArgs &slave = subcommand("slave");

    bool &verbose = flag("v,verbose", "A flag to toggle verbose");
};

int main(int argc, char *argv[])
{

    auto args = argparse::parse<ExecuteArgs>(argc, argv);

    if (args.verbose)
        args.print(); // prints all variables

    if (args.master.is_valid)
    {
        args.master.run(args.path);
    }
    else if (args.slave.is_valid)
    {
        args.slave.run();
    }
    
    return 0;
}


// using namespace ZServer;

// int main()
// {
// 	HTTPClient client;
// 	client.request(
// 		"http://localhost:8080/dsadasd/dsadasd/sadasdasda4554?bla=324#fdsfsdfdssd"

// 	);
// 	return 0;
// }
