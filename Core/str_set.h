#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SET_STR_MAX_SIZE 128

//String multiset (str_m_set) contains a char buffer interpreted as max_size number of string buffers with SET_STR_MAX_SIZE chars each
//str_m_set contains an array of max_size bools. The n'th bool indicates whether the n'th string buffer is in use

struct str_m_set
{
	int max_size;

	char* strs;
	bool* allocated;

	char* find_str(int);
	char* find_str_and_alloc(int* = NULL);

	int number_of_strs_allocated();
	bool is_allocated(int);
	void alloc(int);

	void free(int);
	void free_all();

	int size();

	void print();
};

str_m_set create_str_m_set(int);
void destroy_str_m_set(str_m_set*);
