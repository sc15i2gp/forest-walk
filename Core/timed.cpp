#include "timed.h"

timed_block::timed_block(const char* name, const char* info): block_name(name), block_info(info)
{
	start = clock();
}

timed_block::~timed_block()
{
	int time_taken = (clock() - start) * 1000 / CLOCKS_PER_SEC;
	if(!block_info) printf("%s took %d ms\n", block_name, time_taken);
	else printf("%s : %s took %d ms\n", block_name, block_info, time_taken);
}
