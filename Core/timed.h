#pragma once
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

struct timed_block
{
	timed_block(const char*,const char* = NULL);
	~timed_block();
	const char* block_name;
	const char* block_info;
	clock_t start;
};

#define MT_TIME 0
#define MT_INT 1
#define MT_FLOAT 2

struct measure_event
{
	int type;
	int queue_index;
};

struct measure
{
	bool recording;

	float time_queue[256];
	int time_queue_length;

	int int_queue[256];
	int int_queue_length;

	float float_queue[256];
	int float_queue_length;

	measure_event event_queue[256];
	int event_queue_length;

	FILE* output_file;

	void push_event(int,int);

	void begin_measure();
	void measure_time(float);
	void measure_float(float);
	void measure_int(int);
	void end_measure();
};

extern measure __global_measure;

//At start of program, open output file
//When begin measure, set queues to 0
//When end measure, iterate through event queue, append data as row to output csv file

#define TIMED(func) timed_block _t(func)

#define BEGIN_MEASURE __global_measure.begin_measure()
#define MEASURE_TIME TIMED(__func__)
#define MEASURE_INT(i) __global_measure.measure_int(i)
#define MEASURE_FLOAT(f) __global_measure.measure_float(f)
#define END_MEASURE __global_measure.end_measure()
