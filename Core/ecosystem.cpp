#include "ecosystem.h"

forest_ecosystem create_ecosystem(int str_set_max_size, int forest_length)
{
	forest_ecosystem ecosystem = {};

	for(int i = 0; i < 8; i++) ecosystem.tree_seeds[i] = (long int)rand();
	
	//Initialise ecosystem fields
	ecosystem.m_l_sys = create_m_l_system(str_set_max_size, forest_length);
	ecosystem.t_grid = create_tree_grid(str_set_max_size, forest_length);
	ecosystem.number_of_derivations = 0;
	ecosystem.forest_length = forest_length;
	ecosystem.trees_should_propagate = false;
	ecosystem.succession_should_happen = true;
	ecosystem.self_thinning_should_happen = true;

	//Set up multiset l-system rules
	ecosystem.m_l_sys.add_global_parameter('R', "5.0");
	ecosystem.m_l_sys.add_global_parameter('v', "390.0");
	ecosystem.m_l_sys.add_global_parameter('w', "390.0");
	ecosystem.m_l_sys.add_global_parameter('t', "10.0");
	ecosystem.m_l_sys.add_global_parameter('G', "20.0");
	ecosystem.m_l_sys.add_global_parameter('e', "2.71828");
	ecosystem.m_l_sys.add_production("<T(s,x,y,r,a)>?(c)", "", "c == 0", 1.0f);
	ecosystem.m_l_sys.add_production("<T(s,x,y,r,a)>?(c)", "T(s,x,y,r,a)", "c==0", 0.0f); //Shade tolerance
	ecosystem.m_l_sys.add_production("<T(s,x,y,r,a)>", "T(s,x,y,r,a)", "r >= R", 1.0f);
	ecosystem.m_l_sys.add_production("<T(s,x,y,r,a)>", "", "r >= R", 0.0f); //Longevity
	ecosystem.m_l_sys.add_production("<T(s,x,y,r,a)>?(c)", "T(s,x,y,0.5+R*(1 - e^(-G*(a+1))),a+1)[%T(s,v,w,t,0)?(1)]", NULL, 1.0f);

	return ecosystem;
}

void destroy_ecosystem(forest_ecosystem* ecosystem)
{
	destroy_tree_grid(&ecosystem->t_grid);
	destroy_m_l_system(&ecosystem->m_l_sys);
}

//Get positions and radii of t and u, then check for intersection
bool trees_intersect(char* t, char* u)
{
	float t_x = read_real_parameter_value(t, 1);
	float t_y = read_real_parameter_value(t, 2);
	float t_r = read_real_parameter_value(t, 3);
	float u_x = read_real_parameter_value(u, 1);
	float u_y = read_real_parameter_value(u, 2);
	float u_r = read_real_parameter_value(u, 3);

	return do_circles_intersect(t_x, t_y, t_r, u_x, u_y, u_r);
}

void determine_dominated_tree(char* t, char* u)
{
	float t_r = read_real_parameter_value(t, 3);
	float u_r = read_real_parameter_value(u, 3);

	char* dominated_tree;
	if(t_r < u_r) dominated_tree = t;
	else dominated_tree = u;

	//Dominated tree's domination parameter set to 0 (?(c) where c = 0)
	write_into_parameter(find_next_module(dominated_tree), 0, 0.0f);
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

void check_grid_buckets_for_intersections(m_l_system* m_l_sys, tree_node* bucket_0, tree_node* bucket_1)
{
	for(tree_node* n_0 = bucket_0; n_0; n_0 = n_0->next)
	{//For each node in bucket 0
		for(tree_node* n_1 = bucket_1; n_1; n_1 = n_1->next)
		{//For each node in bucket 1
			check_for_tree_intersection(m_l_sys, n_0->str_ref, n_1->str_ref);
		}
	}
}

//To check for tree domination, tree circles need to be checked for intersection
//To speed this up, only surrounding tree_grid buckets are checked for intersections
void tree_domination_check(m_l_system* m_l_sys, tree_grid* t_grid, int x_0, int y_0)
{
	for(int y_1 = y_0-1; y_1 < y_0+2; y_1++)
	{
		for(int x_1 = x_0-1; x_1 < x_0+2; x_1++)
		{
			if(	x_1 >= 0 && x_1 < t_grid->width() &&
				y_1 >= 0 && y_1 < t_grid->height())
			{
				tree_node* b_0 = t_grid->bucket(x_0, y_0);
				tree_node* b_1 = t_grid->bucket(x_1, y_1);

				check_grid_buckets_for_intersections(m_l_sys, b_0, b_1);
			}
		}
	}
}

void tree_domination_check(m_l_system* m_l_sys, tree_grid* t_grid, int tree)
{
	tree_node* t = t_grid->find_node_by_str_ref(tree);

	int x_0 = t->x;
	int y_0 = t->y;

	tree_domination_check(m_l_sys, t_grid, x_0, y_0);
}

//For a tree to be dead, it's string must have been truncated
//Tree string goes from 'T(s,x,y,r,a)?(c)' to '?(c)'
void remove_dead_trees(m_l_system* m_l_sys, tree_grid* t_grid)
{
	//Remove "empty" strings from set
	for(int i = 0; i < m_l_sys->str_set.size(); i++)
	{//For each string in the m_l_system
		char* str = m_l_sys->str_set.find_str(i);

		if(m_l_sys->str_set.is_allocated(i) && number_of_modules(str) <= 1)
		{//If tree is dead
			t_grid->remove_tree(i);
			m_l_sys->str_set.free(i);
		}
	}
}

float generate_initial_tree_radius(int forest_length)
{
	return 0.5f;
}

//Generates a propagation vector and an initial radius for a new tree
void generate_propagation_data(m_l_system* m_l_sys, tree_grid* t_grid, int parent, int forest_length)
{
	//Determine propagation radius for species
	float propagation_radius = 0.0f;
	char* p_str = m_l_sys->str_set.find_str(parent);
	
	vec3 p_pos;
	p_pos.x = read_real_parameter_value(p_str, 1);
	p_pos.y = read_real_parameter_value(p_str, 2);
	
	int species = read_real_parameter_value(p_str, 0);
	
	switch(species)
	{
		case PINE:
			propagation_radius = PINE_PROPAGATION_MAX_RADIUS;
			break;
		case BIRCH:
			propagation_radius = BIRCH_PROPAGATION_MAX_RADIUS;
			break;
		case ROWAN:
			propagation_radius = ROWAN_PROPAGATION_MAX_RADIUS;
			break;
	}

	//Generate vector within propagation radius
	vec3 new_pos = generate_vector_within_radius(p_pos, propagation_radius);

	if(	new_pos.x < 0.0f || new_pos.x > t_grid->grid_area_length ||
		new_pos.y < 0.0f || new_pos.y > t_grid->grid_area_length)
	{//If new_pos is out of bounds	
		new_pos = p_pos; //Set it to parent's position so it will immediately be removed through domination
	}
	
	//Generate initial radius for a potential new tree
	float r = generate_initial_tree_radius(forest_length);
	
	char v[8] = {};
	char w[8] = {};
	char t[8] = {};
	
	snprintf(v, 8, "%f", new_pos.x);
	snprintf(w, 8, "%f", new_pos.y);
	snprintf(t, 8, "%f", r);
	
	m_l_sys->set_global_parameter('v', v);
	m_l_sys->set_global_parameter('w', w);
	m_l_sys->set_global_parameter('t', t);
}

//Change l_system's growth rate, max radius, shade tolerance and longevity according to the species of the tree parameter
void apply_species_transformation_to_l_system(m_l_system* m_l_sys, int tree, bool succession_should_happen)
{
	int species = read_real_parameter_value(m_l_sys->str_set.find_str(tree), 0);
	float shade_tolerance = 0.0f;
	float longevity = 0.0f;
	switch(species)
	{
		case PINE:
			m_l_sys->set_global_parameter('R', PINE_MAX_RADIUS);
			m_l_sys->set_global_parameter('G', PINE_GROWTH_FACTOR);
			shade_tolerance = PINE_SHADE_TOLERANCE;
			longevity = PINE_LONGEVITY;
			break;
		case BIRCH:
			m_l_sys->set_global_parameter('R', BIRCH_MAX_RADIUS);
			m_l_sys->set_global_parameter('G', BIRCH_GROWTH_FACTOR);
			shade_tolerance = BIRCH_SHADE_TOLERANCE;
			longevity = BIRCH_LONGEVITY;
			break;
		case ROWAN:
			m_l_sys->set_global_parameter('R', ROWAN_MAX_RADIUS);
			m_l_sys->set_global_parameter('G', ROWAN_GROWTH_FACTOR);
			shade_tolerance = ROWAN_SHADE_TOLERANCE;
			longevity = ROWAN_LONGEVITY;
			break;
	}
	if(succession_should_happen)
	{//If succession should happen, change production probabilities such that shade tolerance nad longevity are tekn into account
		m_l_sys->set_production_probability(0, 1.0f-shade_tolerance);
		m_l_sys->set_production_probability(1, shade_tolerance);
		m_l_sys->set_production_probability(2, longevity);
		m_l_sys->set_production_probability(3, 1.0f-longevity);
	}
	else
	{
		m_l_sys->set_production_probability(0, 1.0f);
		m_l_sys->set_production_probability(1, 0.0f);
		m_l_sys->set_production_probability(2, 1.0f);
		m_l_sys->set_production_probability(3, 0.0f);
	}
}

void derive_tree_strs(m_l_system* m_l_sys, tree_grid* t_grid, bool succession_should_happen, int forest_length)
{
	for(int i = 0; i < m_l_sys->str_set.size(); i++)
	{
		//Generate position within a certain radius of the current str
		//If out of bounds, use a default vector
		if(m_l_sys->str_set.is_allocated(i))
		{
			generate_propagation_data(m_l_sys, t_grid, i, forest_length);
			apply_species_transformation_to_l_system(m_l_sys, i, succession_should_happen);
			char* s = m_l_sys->str_set.find_str(i);

			float r_0 = read_real_parameter_value(s, 3);
			m_l_sys->derive_str(i);
			float r_1 = read_real_parameter_value(s, 3);

			//This is necessary to ensure only trees which have changed need their tree models generated
			if(r_0 != r_1) t_grid->find_node_by_str_ref(i)->changed = true;
		}
	}
}

//Removes propagated trees from their original strings and places them in their own
void prune_tree_strs(m_l_system* m_l_sys, tree_grid* t_grid, bool trees_should_propagate)
{
	for(int i = 0; i < m_l_sys->str_set.size(); i++)
	{
		if(m_l_sys->str_set.is_allocated(i))
		{
			if(trees_should_propagate)
			{
				int index = -1;
				char* new_str = m_l_sys->str_set.find_str_and_alloc(&index);
				new_str[0] = 0;

				prune_branches(m_l_sys->str_set.find_str(i),new_str);

				if(strlen(new_str) == 0)
				{//If there was no propagated tree string
					m_l_sys->str_set.free(index);
				}
				else
				{
					//Get new str x and y
					float x = read_real_parameter_value(new_str, 1);
					float y = read_real_parameter_value(new_str, 2);

					t_grid->insert_tree(index, x, y);
				}
			}
			else
			{
				prune_branches(m_l_sys->str_set.find_str(i));
			}
		}
	}
}

/***************************************************/

/*************** Forest Ecosystem ******************/

/***************************************************/

void forest_ecosystem::set_all_trees_not_dominated()
{
	for(int i = 0; i < m_l_sys.str_set.size(); i++)
	{
		if(m_l_sys.str_set.is_allocated(i))
		{
			write_into_parameter(find_next_module(m_l_sys.str_set.find_str(i)), 0, 1.0f);
		}
	}
}

void forest_ecosystem::tree_domination_check()
{
	for(int y_0 = 0; y_0 < t_grid.height(); y_0++)
	{
		for(int x_0 = 0; x_0 < t_grid.width(); x_0++)
		{
			::tree_domination_check(&m_l_sys, &t_grid, x_0, y_0);
		}
	}
}

//Each tree string in the multiset l-system's string set has a corresponding tree node in the tree grid
//This function copies data from each tree string to their matching tree node
void forest_ecosystem::update_tree_grid_data()
{
	str_m_set* s = &(m_l_sys.str_set);
	for(int i = 0; i < t_grid.height(); i++)
	{
		for(int j = 0; j < t_grid.width(); j++)
		{
			tree_node* tree = t_grid.bucket(j,i);
			for(; tree; tree = tree->next)
			{
				char* str = s->find_str(tree->str_ref);
				if(number_of_modules(str) > 1)
				{
					int sp = (int)read_real_parameter_value(str, 0);
					const char* max_r = (sp == PINE) ? PINE_MAX_RADIUS : (sp == BIRCH) ? BIRCH_MAX_RADIUS : ROWAN_MAX_RADIUS;

					float x = read_real_parameter_value(str, 1);
					float y = read_real_parameter_value(str, 2);
					float r = read_real_parameter_value(str, 3);
					int age = (int)read_real_parameter_value(str, 4);
					int c = (int)read_real_parameter_value(find_next_module(str));
					if(tree->seed < 0) tree->seed = tree_seeds[rand() % 8];
					tree->species = sp;
					tree->age = age;
					tree->old_age = r >= atof(max_r);
					tree->_x = x;
					tree->_y = y;
					tree->_r = r;
					tree->dominated = c;
				}
			}
		}
	}
}

void forest_ecosystem::remove_all_trees()
{
	m_l_sys.clear_str_set();
	t_grid.remove_all_trees();
}

void forest_ecosystem::add_tree(float x, float y, float r, int s)
{
	int str_index;
	char* str = m_l_sys.str_set.find_str_and_alloc(&str_index);

	snprintf(str, SET_STR_MAX_SIZE, "T(%d,%f,%f,%f,0)?(1)\0", s, x, y, r);

	t_grid.insert_tree(str_index, x, y);
	::tree_domination_check(&m_l_sys, &t_grid, str_index);//Only go through current string set once for added string to check for domination
}

float forest_ecosystem::generate_initial_tree_radius()
{
	return ::generate_initial_tree_radius(forest_length);
}

void forest_ecosystem::iterate_forest_by_one_plastochron()
{
	MEASURE_TIME;
	derive_tree_strs(&m_l_sys, &t_grid, succession_should_happen, forest_length);
	prune_tree_strs(&m_l_sys, &t_grid, trees_should_propagate);


	if(self_thinning_should_happen) tree_domination_check();
	remove_dead_trees(&m_l_sys, &t_grid);
	
	update_tree_grid_data();
	
	number_of_derivations++;

	MEASURE_KEY(number_of_derivations);
	MEASURE_INT(t_grid.number_of_trees_of_species(PINE));
	MEASURE_INT(t_grid.number_of_trees_of_species(BIRCH));
	MEASURE_INT(t_grid.number_of_trees_of_species(ROWAN));
	MEASURE_INT(t_grid.number_of_trees());

	MEASURE_FLOAT(t_grid.h_index(forest_length));
	MEASURE_FLOAT(t_grid.h_index_for_species(forest_length, PINE));
	MEASURE_FLOAT(t_grid.h_index_for_species(forest_length, BIRCH));
	MEASURE_FLOAT(t_grid.h_index_for_species(forest_length, ROWAN));
	MEASURE_FLOAT(t_grid.area_covered_by(PINE));
	MEASURE_FLOAT(t_grid.area_covered_by(BIRCH));
	MEASURE_FLOAT(t_grid.area_covered_by(ROWAN));
}
