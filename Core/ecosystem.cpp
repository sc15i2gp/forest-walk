#include "ecosystem.h"

forest_ecosystem create_ecosystem(int str_set_max_size, int forest_length)
{
	forest_ecosystem ecosystem = {};

	for(int i = 0; i < 8; i++) ecosystem.tree_seeds[i] = (long int)rand();
	
	ecosystem.m_l_sys = create_m_l_system(str_set_max_size, forest_length);
	ecosystem.t_grid = create_tree_grid(str_set_max_size, forest_length);
	ecosystem.trees_should_propagate = false;
	ecosystem.succession_should_happen = true;
	ecosystem.self_thinning_should_happen = true;

	ecosystem.m_l_sys.add_global_parameter('R', "5.0");
	ecosystem.m_l_sys.add_global_parameter('v', "390.0");
	ecosystem.m_l_sys.add_global_parameter('w', "390.0");
	ecosystem.m_l_sys.add_global_parameter('t', "10.0");
	ecosystem.m_l_sys.add_global_parameter('G', "20.0");
	ecosystem.m_l_sys.add_production("<T(s,x,y,r,a)>?(c)", "", "c == 0", 1.0f);
	ecosystem.m_l_sys.add_production("<T(s,x,y,r,a)>?(c)", "T(s,x,y,r,a)", "c==0", 0.0f); //Shade tolerance
	ecosystem.m_l_sys.add_production("<T(s,x,y,r,a)>", "T(s,x,y,r,a)", "r >= R", 1.0f);
	ecosystem.m_l_sys.add_production("<T(s,x,y,r,a)>", "", "r >= R", 0.0f); //Longevity
	ecosystem.m_l_sys.add_production("<T(s,x,y,r,a)>?(c)", "T(s,x,y,G*r,a+1)[%T(s,v,w,t,0)?(1)]", NULL, 1.0f);

	return ecosystem;
}

void destroy_ecosystem(forest_ecosystem* ecosystem)
{
	destroy_tree_grid(&ecosystem->t_grid);
	destroy_m_l_system(&ecosystem->m_l_sys);
}

bool trees_intersect(char* t, char* u)
{
	float t_x = read_real_parameter_value(t, 1);
	float t_y = read_real_parameter_value(t, 2);
	float t_r = read_real_parameter_value(t, 3);
	float u_x = read_real_parameter_value(u, 1);
	float u_y = read_real_parameter_value(u, 2);
	float u_r = read_real_parameter_value(u, 3);

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
	float t_r = read_real_parameter_value(t, 3);
	float u_r = read_real_parameter_value(u, 3);
	if(t_r < u_r) set_dominated_tree(t);
	else set_dominated_tree(u);
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

void check_grid_nodes_for_intersection(m_l_system* m_l_sys, tree_node* n_0, tree_node* n_1)
{	
	if(n_0->str_ref != n_1->str_ref)
	{
		check_for_tree_intersection(m_l_sys, n_0->str_ref, n_1->str_ref);
	}
}

void check_grid_buckets_for_intersections(m_l_system* m_l_sys, tree_node* b_0, tree_node* b_1)
{
	for(tree_node* bucket_0 = b_0; bucket_0; bucket_0 = bucket_0->next)
	{
		for(tree_node* bucket_1 = b_1; bucket_1; bucket_1 = bucket_1->next)
		{
			check_grid_nodes_for_intersection(m_l_sys, bucket_0, bucket_1);
		}
	}
}


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


void remove_dead_trees(m_l_system* m_l_sys, tree_grid* t_grid)
{
	//Remove "empty" strings from set
	for(int i = 0; i < m_l_sys->str_set.size(); i++)
	{
		char* str = m_l_sys->str_set.find_str(i);
		if(m_l_sys->str_set.is_allocated(i) && number_of_modules(str) <= 1)
		{
			t_grid->remove_tree(i);
			m_l_sys->str_set.free(i);
		}
	}
}

vec3 generate_vector_within_radius(vec3 center, float r_max)
{
	int c = rand()%100; //How much of r_max away from the center should v be
	float c_f = (float)c/100.0f;
	float r = r_max*c_f;
	vec3 v = {r, 0.0f, 0.0f};

	int a = rand()%360; //Angle to rotate around v
	v = rotate_about_axis(v, vec3{0.0f,0.0f,1.0f}, (float)a);
	v = v + center;
	return v;
}

void generate_propagation_vector(m_l_system* m_l_sys, tree_grid* t_grid, int parent)
{
	char* p_str = m_l_sys->str_set.find_str(parent);
	int species = read_real_parameter_value(p_str, 0);
	vec3 p_pos;
	p_pos.x = read_real_parameter_value(p_str, 1);
	p_pos.y = read_real_parameter_value(p_str, 2);
	float propagation_radius = 0.0f;
	switch(species)
	{
		case PINE:
			propagation_radius = 10.0f;
			break;
		case BIRCH:
			propagation_radius = 15.0f;
			break;
		case ROWAN:
			propagation_radius = 50.0f;
			break;
	}
	vec3 new_pos = generate_vector_within_radius(p_pos, propagation_radius);
	char v[8] = {};
	char w[8] = {};
	char t[8] = {};
	if(	new_pos.x < 0.0f || new_pos.x > t_grid->grid_area_length ||
		new_pos.y < 0.0f || new_pos.y > t_grid->grid_area_length)
	{//If new_pos is out of bounds	
		new_pos = p_pos; //Set it to parent's position so it will immediately be removed through domination
	}
	float r = (float)(rand() % 400)/600.0f + 0.1f;
	
	snprintf(v, 8, "%f", new_pos.x);
	snprintf(w, 8, "%f", new_pos.y);
	snprintf(t, 8, "%f", r);
	m_l_sys->set_global_parameter('v', v);
	m_l_sys->set_global_parameter('w', w);
	m_l_sys->set_global_parameter('t', t);
}

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
	{
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
					int s = (int)read_real_parameter_value(str, 0);
					float x = read_real_parameter_value(str, 1);
					float y = read_real_parameter_value(str, 2);
					float r = read_real_parameter_value(str, 3);
					int age = (int)read_real_parameter_value(str, 4);
					int c = (int)read_real_parameter_value(find_next_module(str));
					if(tree->seed < 0) tree->seed = tree_seeds[rand() % 8];
					tree->species = s;
					tree->age = age;
					const char* max_r = (s == PINE) ? PINE_MAX_RADIUS : (s == BIRCH) ? BIRCH_MAX_RADIUS : ROWAN_MAX_RADIUS;
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
	::tree_domination_check(&m_l_sys, &t_grid, str_index);//Only go through current string set once for added string
}

void forest_ecosystem::iterate_forest_by_one_plastochron()
{
	//Derive strs in str_set
	for(int i = 0; i < m_l_sys.str_set.size(); i++)
	{
		//Generate position within a radius of 10.0f of the current str
		//If out of bounds, use a default vector
		if(m_l_sys.str_set.is_allocated(i))
		{
			generate_propagation_vector(&m_l_sys, &t_grid, i);
			apply_species_transformation_to_l_system(&m_l_sys, i, succession_should_happen);
			char* s = m_l_sys.str_set.find_str(i);
			float r_0 = read_real_parameter_value(s, 3);
			m_l_sys.derive_str(i);
			float r_1 = read_real_parameter_value(s, 3);
			if(r_0 != r_1) t_grid.find_node_by_str_ref(i)->changed = true;
		}
	}
	//Prune branched strings
	for(int i = 0; i < m_l_sys.str_set.size(); i++)
	{
		if(m_l_sys.str_set.is_allocated(i))
		{
			if(trees_should_propagate)
			{
				int index = -1;
				char* new_str = m_l_sys.str_set.find_str_and_alloc(&index);
				new_str[0] = 0;
				prune_branches(m_l_sys.str_set.find_str(i),new_str);
				if(strlen(new_str) == 0)
				{
					new_str[0] = 0;
					m_l_sys.str_set.free(index);
				}
				else
				{
					//Get new str x and y
					float x = read_real_parameter_value(new_str, 1);
					float y = read_real_parameter_value(new_str, 2);
					t_grid.insert_tree(index, x, y);
				}
			}
			else
			{
				prune_branches(m_l_sys.str_set.find_str(i));
			}
		}
	}
	if(self_thinning_should_happen) tree_domination_check();
	remove_dead_trees(&m_l_sys, &t_grid);
	t_grid.derived = true;
	
}
