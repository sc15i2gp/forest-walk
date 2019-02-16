#include "multiset_l_system.h"

m_l_system create_m_l_system(int str_set_max)
{
	m_l_system new_sys = {};
	new_sys.str_set = (char*)malloc(SET_STR_MAX_SIZE*str_set_max);
	new_sys.str_set_size = 0;
	return new_sys;
}

void print_str_set(m_l_system* m_l_sys)
{
	for(int i = 0; i < m_l_sys->str_set_size; i++)
	{
		printf("%d: %s\n", i, get_str_from_set(m_l_sys, i));
	}
}

void destroy_m_l_system(m_l_system* m_l_sys)
{
	free(m_l_sys->str_set);
}

void add_production(m_l_system* m_l_sys, char* predecessor, char* successor, char* condition, float prob)
{
	add_production((l_system*)m_l_sys, predecessor, successor, condition, prob);
}

void add_global_parameter(m_l_system* m_l_sys, char token, char* initial_val)
{
	add_global_parameter((l_system*)m_l_sys, token, initial_val);
}

char* get_str_from_set(m_l_system* m_l_sys, int index)
{
	return m_l_sys->str_set + SET_STR_MAX_SIZE*index;
}

void clear_str_set(m_l_system* m_l_sys)
{
	m_l_sys->str_set_size = 0;
}

void add_str(m_l_system* m_l_sys, float x, float y, float r, int c)
{
	snprintf(get_str_from_set(m_l_sys, m_l_sys->str_set_size), SET_STR_MAX_SIZE, "T(%f,%f,%f)?(%d)\0", x, y, r, c);
	m_l_sys->str_set_size++;
}

void derive_set(m_l_system* m_l_sys)
{
	//Grow/remove trees
	for(int i = 0; i < m_l_sys->str_set_size; i++)
	{
		char* str = m_l_sys->str_set+i*SET_STR_MAX_SIZE;
		derive_str((l_system*)m_l_sys, str);
	}
	//Check whether trees are dominated
	/*
	for(int i = 0; i < m_l_sys->str_set_size; i++)
	{

	}*/
}
