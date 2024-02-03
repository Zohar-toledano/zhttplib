#pragma once

#include <string>
#include <sstream>

#include "osResources.hpp"
struct WorkerBase
{
public:
	static const char *pipeName;
	static std::string getPipeName(int id)
	{
		std::stringstream ss;
		ss << pipeName << id;
		return ss.str();
	}
	Pipe pipe;
	int id;
	WorkerBase(int id) : id(id){};
};

const char *WorkerBase::pipeName = "\\\\.\\pipe\\worker_pipe_";

