#include <iostream>
#include "server/Server.h"
#include "multiproccesing/WorkerRoutine.hpp"

int main()
{
	std::string memory = "mem";
	WorkerRoutine w (0,memory);
	w.workerMain();
}

