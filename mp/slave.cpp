#include "Source.hpp"
int main()
{

	t res;

	SharedQueueWrapper<t> sq(shared_mem_size, shared_mem_name, false);
	
	sq.pop(res);
	std::cout << res << std::endl;

	sq.pop(res);
	std::cout << res << std::endl;

	// dumpToFile("C:\\Users\\Zohar\\Downloads\\slave.bin", (const char *)sq.queue, sq.queue->getTotalSize());

	return 0;
}