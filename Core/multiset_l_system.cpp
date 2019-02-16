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

bool trees_intersect(char* t, char* u)
{
	float t_x = read_real_parameter_value(t, 0);
	float t_y = read_real_parameter_value(t, 1);
	float t_r = read_real_parameter_value(t, 2);
	float u_x = read_real_parameter_value(u, 0);
	float u_y = read_real_parameter_value(u, 1);
	float u_r = read_real_parameter_value(u, 2);

	float dist = sqrt((t_x - u_x)*(t_x - u_x) + (t_y - u_y)*(t_y - u_y));
	float total_radius = t_r + u_r;
	return dist < total_radius;
}

void set_dominated_tree(char* t)
{
	char* q_module = find_next_module(t);
	write_into_parameter(q_module, 0, 0.0f);
}

void determine_dominated_tree(char* t, char* u)
{
	float t_r = read_real_parameter_value(t, 2);
	float u_r = read_real_parameter_value(u, 2);
	if(t_r < u_r) set_dominated_tree(t);
	else set_dominated_tree(u);
}

void derive_set(m_l_system* m_l_sys)
{
	//Grow/remove trees
	for(int i = 0; i < m_l_sys->str_set_size; i++)
	{
		char* str = get_str_from_set(m_l_sys, i);
		derive_str((l_system*)m_l_sys, str);
	}
	//Check whether trees are dominated
	for(int i = 0; i < m_l_sys->str_set_size; i++)
	{
		char* str_0 = get_str_from_set(m_l_sys, i);
		for(int j = i+1; j < m_l_sys->str_set_size; j++)
		{
			char* str_1 = get_str_from_set(m_l_sys, j);
			if(number_of_modules(str_0) > 1 && number_of_modules(str_1) > 1)
			{
				if(trees_intersect(str_0, str_1))
				{
					determine_dominated_tree(str_0, str_1);
				}
			}
		}
	}
}
