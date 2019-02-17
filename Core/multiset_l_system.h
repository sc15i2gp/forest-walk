#pragma once
#include "l_system.h"

#define SET_STR_MAX_SIZE 64
struct m_l_system
{
	l_system base_sys;
	char* str_set;
	int str_set_size;
};

m_l_system create_m_l_system(int str_set_max); 
void print_str_set(m_l_system*);
void clear_str_set(m_l_system*);
char* get_str_from_set(m_l_system*, int);
void destroy_m_l_system(m_l_system*);
void add_str(m_l_system*, float x, float y, float r, int c = 0);
void add_production(m_l_system*, char* predecessor, char* successor, char* condition, float prob = 1.0f);
void add_global_parameter(m_l_system*, char token, char* initial_val);
void derive_set(m_l_system*);