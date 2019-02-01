#include "char_queue.h"

void print_queue(char_queue* q)
{
	print_queue_buffer(q);
	print_queue_contents(q);
}

void print_queue_buffer(char_queue* q)
{
	printf("Buffer contents: %.*s\n", CHAR_QUEUE_MAX, q->char_array);
}

//TODO: Change this for circular buffer
void print_queue_contents(char_queue* q)
{
	if(q->start > q->end || (q->start == q->end && q->pushed_last))
	{
		printf("Queue contents: %.*s%.*s\n", CHAR_QUEUE_MAX - q->unused - q->start, q->char_array+q->start, q->end, q->char_array);
	}
	else
	{
		printf("Queue contents: %.*s\n", q->end - q->start, q->char_array + q->start);
	}
}

char_queue& char_queue::operator=(const char_queue& c_q)
{
	if(this != &c_q)
	{
		memcpy(this, &c_q, sizeof(char_queue));
	}
	return *this;
}

int char_queue::length_pushed()
{
	if(start < end) return end - start;
	else if(start > end || (start == end && pushed_last))
	{
		int length = CHAR_QUEUE_MAX - unused - start;
		length += end;
	}
	else
	{
		return 0;
	}
}

char* char_queue::head()
{
	return char_array + start;
}

int char_queue::length_free()
{
	if(start < end) return (CHAR_QUEUE_MAX - end) + start;
	else if(start > end) return start - end;
	else
	{
		if(pushed_last) return 0;
		else return CHAR_QUEUE_MAX;
	}
}

bool char_queue::push(char* str, int str_length)
{
	if(end + str_length > CHAR_QUEUE_MAX)
	{
		int initial_end = end;
		int initial_unused = unused;
		printf("INITIAL END %d\n", initial_end);
		end = 0;
		unused = CHAR_QUEUE_MAX - initial_end;
		printf("unused %d\n", unused);
		if(length_free() < str_length)
		{
			end = initial_end;
			unused = initial_unused;
			return false;
		}
	}
	if(length_free() < str_length) return false;
	memcpy(char_array+end, str, str_length);
	push_length_array[end] = str_length;
	int i = end;
	end = (end + str_length) % CHAR_QUEUE_MAX;
	pushed_last = true;
	push_count++;
	return true;
}

bool char_queue::pop()
{
	int length = push_length_array[start];
	int new_start = start;
	int new_unused = unused;
	if(start + length >= CHAR_QUEUE_MAX - unused)
	{
		new_start = 0;
		new_start += (start+length) - (CHAR_QUEUE_MAX - unused);
		new_unused = 0;
	}
	else
	{
		new_start += length;
	}
	if(start <= end && new_start > end)
	{
		return false;
	}
	else
	{
		start = new_start;
		unused = new_unused;
		pushed_last = false;
		push_count--;
		return true;
	}
}

bool char_queue::push(char* str)
{
	this->push(str, strlen(str));
}

void char_queue::pop_all()
{
	//while(start != end) pop();
	start = 0;
	end = 0;
	unused = 0;
	push_count = 0;
	pushed_last = false;
}

char* char_queue::get_pushed_string(int s)
{
	char* str = head();
	int index = start;
	for(int i = 0; i < s; i++)
	{
		str += push_length_array[index];
		index += push_length_array[index];
		if(index >= end) return NULL;
	}
	return str;
}

//TODO: Change this for circular buffer
void char_queue::flush(char* dest)
{
	if(start > end || (start == end && length_free() == 0))
	{
		int first_cpy_length = CHAR_QUEUE_MAX - unused - start;
		memcpy(dest, char_array+start, first_cpy_length);
		dest += first_cpy_length;
		int second_cpy_length = end;
		memcpy(dest, char_array, second_cpy_length);
	}
	else if(start < end)
	{
		memcpy(dest, char_array+start, end - start);
	}
	start = 0;
	end = 0;
	unused = 0;
	push_count = 0;
	pushed_last = false;
}

//TODO: Ensure cpy_length <= length_pushed()
int char_queue::flush_chars(char* dest, int cpy_length)
{
	int cpy_end = start+cpy_length;
	int to_return = 0;
	if(cpy_end >= CHAR_QUEUE_MAX - unused)
	{
		printf("FLUSHING ACROSS BORDER\n");
		int push_count_difference = 0;
		int final_cpy_length = 0;
		int char_index = start;
		while(char_index < CHAR_QUEUE_MAX - unused)
		{
			push_count_difference++;
			final_cpy_length += push_length_array[char_index];
			char_index += push_length_array[char_index];
		}
		cpy_end -= final_cpy_length;
		memcpy(dest, char_array + start, final_cpy_length);
		dest += final_cpy_length;
		to_return += final_cpy_length;
		start = 0;
		unused = 0;

		char_index = start;
		final_cpy_length = 0;
		while(char_index + push_length_array[char_index] < cpy_end)
		{
			push_count_difference++;
			final_cpy_length += push_length_array[char_index];
			char_index += push_length_array[char_index];
		}
		memcpy(dest, char_array+start, final_cpy_length);
		to_return += final_cpy_length;
		start += final_cpy_length;
		push_count -= push_count_difference;
	}
	else
	{
		printf("NOT FLUSHING ACROSS BORDER\n");
		int final_cpy_length = 0;
		int push_count_difference = 0;
		int char_index = start;
		printf("COPY END %d\n", cpy_end);
		printf("CHAR INDEX = ");
		//TODO: Fix this
		while(char_index + push_length_array[char_index] < cpy_end)
		{
			printf("%d | ", char_index);
			push_count_difference++;
			final_cpy_length += push_length_array[char_index];
			char_index += push_length_array[char_index];
		}
		int cpy_deficit = cpy_length - final_cpy_length;
		int last_push_difference = push_length_array[char_index] - cpy_deficit;
		char_index += cpy_deficit;
		final_cpy_length += cpy_deficit;
		printf("\n");
		memcpy(dest, char_array + start, final_cpy_length);
		to_return = final_cpy_length;
		start += final_cpy_length;
		push_count -= push_count_difference;
		push_length_array[char_index] = last_push_difference;
	}
	return to_return;
}
