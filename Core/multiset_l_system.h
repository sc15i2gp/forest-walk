#pragma once
#include "l_system.h"
#include "str_set.h"
#include "tree_grid.h"
#include "species_info.h"
#include "timed.h"

struct m_l_system
{
	l_system base_sys;
	str_m_set str_set;
	tree_grid t_grid;
	bool self_thinning_should_happen;
	bool trees_should_propagate;
	bool succession_should_happen;
};

m_l_system create_m_l_system(int str_set_max, int forest_length); 
void print_str_set(m_l_system*);
void clear_production_set(m_l_system*);
void clear_str_set(m_l_system*);
char* get_str_from_set(m_l_system*, int);
void destroy_m_l_system(m_l_system*);
void add_str(m_l_system*, float x, float y, float r, int s);
void add_production(m_l_system*, char* predecessor, char* successor, char* condition, float prob = 1.0f);
void add_global_parameter(m_l_system*, char token, char* initial_val);
void tree_domination_check(m_l_system*);
void set_all_trees_not_dominated(m_l_system*);
void derive_set(m_l_system*);
