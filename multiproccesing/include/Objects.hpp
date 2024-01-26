#pragma once

#define sharedQueueType int
#define shared_mem_size 800

struct MasterSlaveMessage
{
	int socketID;
	MasterSlaveMessage() = default;
	MasterSlaveMessage(int socketID) : socketID(socketID){};
};




struct SomeStruct
{
	bool running = true;
};

