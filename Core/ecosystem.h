#pragma once
#include "multiset_l_system.h"

struct forest_ecosystem
{
	m_l_system m_l_sys;
	tree_grid t_grid;
	long int tree_seeds[8];

	bool self_thinning_should_happen;
	bool trees_should_propagate;
	bool succession_should_happen;

	void iterate_forest_by_one_plastochron();
	void add_tree(float x, float y, float r, int s);
	void set_all_trees_not_dominated();
	void tree_domination_check();
	void update_tree_grid_data();
	void remove_all_trees();
};

forest_ecosystem create_ecosystem(int,int);
void destroy_ecosystem(forest_ecosystem*); //A la Donald Trump
