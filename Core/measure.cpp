#include "measure.h"

measure __global_measure = {};

timed_block::timed_block(const char* name, const char* info): block_name(name), block_info(info)
{
	start = clock();
}

timed_block::~timed_block()
{
	int time_taken = (clock() - start) * 1000 / CLOCKS_PER_SEC;
	if(!block_info) printf("%s took %d ms\n", block_name, time_taken);
	else printf("%s : %s took %d ms\n", block_name, block_info, time_taken);
	if(__global_measure.recording)
	{
		__global_measure.measure_time(time_taken);
	}
}

void measure::push_event(int type, int index)
{
	event_queue[event_queue_length++] = measure_event{type, index};
}

void measure::begin_measure()
{
	recording = true;
	event_queue_length = 0;
	if(written)
	output_file = fopen("eval_log","a");
	else
	output_file = fopen("eval_log","w");
}

void measure::measure_key(int k)
{
	key = k;
}

void measure::measure_time(int t)
{
	time_queue[time_queue_length] = t;
	push_event(MT_TIME, time_queue_length);
	time_queue_length++;
}

void measure::measure_float(float f)
{
	float_queue[float_queue_length] = f;
	push_event(MT_FLOAT, float_queue_length);
	float_queue_length++;
}

void measure::measure_int(int i)
{
	int_queue[int_queue_length] = i;
	push_event(MT_INT, int_queue_length);
	int_queue_length++;
}

void measure::end_measure()
{
	recording = false;

	if(!output_file)
	{
		printf("Error: No eval log file specified\n");
		return;
	}
	//flush queues
	fprintf(output_file, "%d,", key);
	for(int i = 0; i < event_queue_length; i++)
	{
		measure_event e = event_queue[i];
		switch(e.type)
		{
			case MT_TIME:
				fprintf(output_file, "%d,", time_queue[e.queue_index]);
				break;
			case MT_INT:
				fprintf(output_file, "%d,", int_queue[e.queue_index]);
				break;
			case MT_FLOAT:
				fprintf(output_file, "%f,", float_queue[e.queue_index]);
				break;
		}
	}
	fprintf(output_file, "\n");

	fclose(output_file);
	written = true;
	output_file = NULL;
	time_queue_length = 0;
	int_queue_length = 0;
	float_queue_length = 0;
}
