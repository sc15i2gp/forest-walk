#pragma once
#include "l_system.h"
#include "str_set.h"
#include "tree_grid.h"
#include "species_info.h"
#include "timed.h"

struct m_l_system
{
	l_system base_l_sys;
	str_m_set str_set;

	void print_str_set();

	void add_production(char* predecessor, char* successor, char* condition, float prob = 1.0f);
	void set_production_probability(int,float);
	void add_global_parameter(char token, char* initial_val);
	void set_global_parameter(char token, char* value);

	void clear_production_set();
	void clear_str_set();

	void derive_str(int);
};

m_l_system create_m_l_system(int str_set_max, int forest_length); 
void destroy_m_l_system(m_l_system*);

