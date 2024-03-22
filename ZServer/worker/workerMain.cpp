#include "argparse.hpp" // before windows.h
// #include "server/Server.h"
#include "WorkerRoutine.hpp"


using namespace ZServer;

struct ExecuteArgs : public argparse::Args
{
    int &id = kwarg("i,id", "An ID for slave").set_default(0);
    std::string &memory = kwarg("m,memory", "Shared memory name");
    std::string &dllPath = kwarg("d,dll", "The dll path.");
};

int main(int argc, char *argv[])
{
    auto args = argparse::parse<ExecuteArgs>(argc, argv);

	WorkerRoutine worker(args.id, args.memory, args.dllPath);
	worker.workerMain();
    return 0;
}
