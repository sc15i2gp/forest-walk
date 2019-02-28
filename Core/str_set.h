#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SET_STR_MAX_SIZE 64

struct str_m_set
{
	int max_size;
	char* strs;
	bool* allocated;

	char* find_str(int);
	char* find_str_and_alloc(int* = NULL);
	int number_allocated();
	int size();
	bool is_allocated(int);
	void alloc(int);
	void free(int);
	void free_all();
	void print();
};

str_m_set create_str_m_set(int);
void destroy_str_m_set(str_m_set*);
