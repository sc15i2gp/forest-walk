#pragma once
#include <assert.h>
#include <string.h>
#include <stdio.h>

#define CHAR_QUEUE_MAX 512
struct char_queue
{
	char char_array[CHAR_QUEUE_MAX] = {};
	int push_length_array[CHAR_QUEUE_MAX] = {};
	int end = 0;
	int start = 0;
	int unused = 0;
	int push_count = 0;
	bool pushed_last = false;
	int length_pushed();
	int length_free();
	bool push(char* str, int str_length);
	bool push(char* str);
	bool pop();
	void pop_all();
	void flush(char* dest);
	int flush_chars(char*, int);
	char* head();
	char* get_pushed_string(int);
	bool is_array_full();
	char_queue& operator=(const char_queue& c_q);
};

void print_queue_buffer(char_queue*);
void print_queue_contents(char_queue*);
void print_queue(char_queue*);
