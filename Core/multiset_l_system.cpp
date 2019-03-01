#include "multiset_l_system.h"

m_l_system create_m_l_system(int str_set_max)
{
	m_l_system new_sys = {};
	new_sys.str_set = create_str_m_set(str_set_max);
	return new_sys;
}

void destroy_m_l_system(m_l_system* m_l_sys)
{
	destroy_str_m_set(&m_l_sys->str_set);
}

void add_production(m_l_system* m_l_sys, char* predecessor, char* successor, char* condition, float prob)
{
	add_production((l_system*)m_l_sys, predecessor, successor, condition, prob);
}

void add_global_parameter(m_l_system* m_l_sys, char token, char* initial_val)
{
	add_global_parameter((l_system*)m_l_sys, token, initial_val);
}

void clear_str_set(m_l_system* m_l_sys)
{
	m_l_sys->str_set.free_all();
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

void remove_str_from_set(m_l_system* m_l_sys, int str_index)
{
	if(m_l_sys->str_set.is_allocated(str_index))
	{
		m_l_sys->str_set.free(str_index);
	}
}

void tree_domination_check(m_l_system* m_l_sys, int tree)
{
	char* str_0 = m_l_sys->str_set.find_str(tree);
	for(int i = 0; i < m_l_sys->str_set.size(); i++)
	{
		if(i != tree && m_l_sys->str_set.is_allocated(i))
		{
			char* str_1 = m_l_sys->str_set.find_str(i);
			if(trees_intersect(str_0, str_1))
			{
				determine_dominated_tree(str_0, str_1);
			}
		}
	}
}

void check_for_tree_intersection(m_l_system* m_l_sys, int t_0, int t_1)
{
	if(t_0 != t_1)
	{
		char* s_0 = m_l_sys->str_set.find_str(t_0);
		char* s_1 = m_l_sys->str_set.find_str(t_1);
		if(number_of_modules(s_0) > 1 && number_of_modules(s_1) > 1)
		{
			if(trees_intersect(s_0, s_1)) determine_dominated_tree(s_0, s_1);
		}
	}
}

void tree_domination_check(m_l_system* m_l_sys)
{
	//Check whether trees are dominated
	for(int i = 0; i < m_l_sys->str_set.size(); i++)
	{
		for(int j = i+1; j < m_l_sys->str_set.size(); j++)
		{
			check_for_tree_intersection(m_l_sys, i,j);
		}
	}
}

void add_str(m_l_system* m_l_sys, float x, float y, float r, int c)
{
	int str_index;
	char* str = m_l_sys->str_set.find_str_and_alloc(&str_index);
	assert(str);
	snprintf(str, SET_STR_MAX_SIZE, "T(%f,%f,%f)?(%d)\0", x, y, r, c);
	//m_l_sys->t_grid.insert_tree(str_index, x, y);
	tree_domination_check(m_l_sys, str_index);//Only go through current string set once for added string
}

void remove_dead_trees(m_l_system* m_l_sys)
{
	//Remove "empty" strings from set
	for(int i = 0; i < m_l_sys->str_set.size(); i++)
	{
		char* str = m_l_sys->str_set.find_str(i);
		if(number_of_modules(str) <= 1)
		{
			//m_l_sys->t_grid.remove_tree(i);
			m_l_sys->str_set.free(i);
		}
	}
}

void derive_set(m_l_system* m_l_sys)
{
	printf("Number of trees: %d\n", m_l_sys->str_set.number_allocated());
	//Derive strs in str_set
	for(int i = 0; i < m_l_sys->str_set.size(); i++)
	{
		if(m_l_sys->str_set.is_allocated(i)) derive_str((l_system*)m_l_sys, m_l_sys->str_set.find_str(i));
	}
	tree_domination_check(m_l_sys);
	remove_dead_trees(m_l_sys);
	printf("Final number of trees: %d\n", m_l_sys->str_set.number_allocated());
}
