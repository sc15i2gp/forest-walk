#pragma once
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

//NOTE: This module is used to output measurements to evaluate the project programs

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
	bool written;

	int key;

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
	void measure_key(int);
	void measure_time(float);
	void measure_float(float);
	void measure_int(int);
	void end_measure();
};

extern measure __global_measure;

//Between BEGIN_MEASURE and END_MEASURE, values are taken and stored in queues
//The queues are flushed as a single row to a csv file for processing

#ifdef SHOULD_EVALUATE
#define TIMED(func) timed_block _t(func)
#define BEGIN_MEASURE __global_measure.begin_measure()
#define MEASURE_KEY(i) __global_measure.measure_key(i)
#define MEASURE_TIME TIMED(__func__)
#define MEASURE_INT(i) __global_measure.measure_int(i)
#define MEASURE_FLOAT(f) __global_measure.measure_float(f)
#define END_MEASURE __global_measure.end_measure()


#ifdef EDITOR_EVALUATE
#define MEASURE_TIME_IF_EDITOR MEASURE_TIME

#else
#define MEASURE_TIME_IF_EDITOR

#endif


#else
#define TIMED(func)
#define BEGIN_MEASURE
#define MEASURE_TIME
#define MEASURE_KEY
#define MEASURE_INT(i)
#define MEASURE_FLOAT(f)
#define END_MEASURE
#define MEASURE_TIME_IF_EDITOR

#endif
