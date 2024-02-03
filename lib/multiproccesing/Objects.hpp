#pragma once

#include <osResources/Socket.h>
#define shared_mem_size 800
#define sharedQueueType ZServer::SocketTCP

struct MasterSlaveMessage
{
	sharedQueueType socketID;
	MasterSlaveMessage() = default;
	MasterSlaveMessage(sharedQueueType socketID) : socketID(socketID){};
};




struct SomeStruct
{
	bool running = true;
};

