#pragma once
#include "l_system.h"

#define SET_STR_MAX_SIZE 64
struct tree_node
{
	int str_ref;
	tree_node* next;
};

struct tree_grid
{
	tree_node* node_array;
	bool* in_use;

	float bucket_width;
	float bucket_height;
	float forest_width;
	float forest_height;
	tree_node** grid;
};

struct m_l_system
{
	l_system base_sys;
	char* str_set;
	int str_set_size;
	tree_grid trees;
};

m_l_system create_m_l_system(int str_set_max, float forest_width, float forest_height, float bucket_width = 20.0f, float bucket_height = 20.0f);
void print_str_set(m_l_system*);
void clear_str_set(m_l_system*);
char* get_str_from_set(m_l_system*, int);
void destroy_m_l_system(m_l_system*);
void add_str(m_l_system*, float x, float y, float r, int c = 0);
void add_production(m_l_system*, char* predecessor, char* successor, char* condition, float prob = 1.0f);
void add_global_parameter(m_l_system*, char token, char* initial_val);
void derive_set(m_l_system*);
