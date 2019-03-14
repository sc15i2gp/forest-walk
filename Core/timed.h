#pragma once
#include <time.h>
#include <stdio.h>

struct timed_block
{
	timed_block(const char*,const char* = NULL);
	~timed_block();
	const char* block_name;
	const char* block_info;
	clock_t start;
};

#define TIMED(func) timed_block _t(func)
