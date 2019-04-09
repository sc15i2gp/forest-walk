#include "multiset_l_system.h"

m_l_system create_m_l_system(int str_set_max, int forest_length)
{
	m_l_system new_sys = {};
	new_sys.str_set = create_str_m_set(str_set_max);
	return new_sys;
}

void destroy_m_l_system(m_l_system* m_l_sys)
{
	destroy_str_m_set(&m_l_sys->str_set);
}

void m_l_system::add_production(char* predecessor, char* successor, char* condition, float prob)
{
	::add_production(&base_l_sys, predecessor, successor, condition, prob);
}

void m_l_system::set_production_probability(int production_index, float probability)
{
	::set_production_probability(&base_l_sys, production_index, probability);
}

void m_l_system::add_global_parameter(char token, char* initial_val)
{
	::add_global_parameter(&base_l_sys, token, initial_val);
}

void m_l_system::set_global_parameter(char token, char* value)
{
	::set_global_parameter(&base_l_sys, token, value);
}

void m_l_system::clear_production_set()
{
	base_l_sys.p_set_size = 0;
}

void m_l_system::clear_str_set()
{
	str_set.free_all();
}

void m_l_system::derive_str(int str_index)
{
	::derive_str(&base_l_sys, str_set.find_str(str_index));
}
