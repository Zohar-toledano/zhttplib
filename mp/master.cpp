#include "Source.hpp"
int main()
{


	SharedQueueWrapper<t> sq(shared_mem_size, shared_mem_name, true);

	t res = 80;
	sq.push(res);
	sq.push(53);
	sq.push(0x85);
	sq.push(0x70);

	Sleep(10000);
	std::cout<<sq.length();
	// dumpToFile("C:\\Users\\Zohar\\Downloads\\master.bin", (const char *)sq.queue, sq.queue->getTotalSize());
	return 0;
}