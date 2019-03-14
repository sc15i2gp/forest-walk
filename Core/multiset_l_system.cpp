#include "multiset_l_system.h"

m_l_system create_m_l_system(int str_set_max, int forest_length)
{
	m_l_system new_sys = {};
	new_sys.str_set = create_str_m_set(str_set_max);
	new_sys.t_grid = create_tree_grid(str_set_max, forest_length);
	return new_sys;
}

void destroy_m_l_system(m_l_system* m_l_sys)
{
	destroy_tree_grid(&m_l_sys->t_grid);
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
	m_l_sys->t_grid.remove_all_trees();
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

void remove_str_from_set(m_l_system* m_l_sys, int str_index)
{
	if(m_l_sys->str_set.is_allocated(str_index))
	{
		m_l_sys->str_set.free(str_index);
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


void tree_domination_check(m_l_system* m_l_sys, int x_0, int y_0)
{
	for(int y_1 = y_0-1; y_1 < y_0+2; y_1++)
	{
		for(int x_1 = x_0-1; x_1 < x_0+2; x_1++)
		{
			if(	x_1 >= 0 && x_1 < m_l_sys->t_grid.width() &&
				y_1 >= 0 && y_1 < m_l_sys->t_grid.height())
			{
				tree_node* b_0 = m_l_sys->t_grid.bucket(x_0, y_0);
				tree_node* b_1 = m_l_sys->t_grid.bucket(x_1, y_1);
				check_grid_buckets_for_intersections(m_l_sys, b_0, b_1);
			}
		}
	}
}

void tree_domination_check(m_l_system* m_l_sys, int tree)
{
	tree_node* t = m_l_sys->t_grid.find_node(tree);
	int x_0 = t->x;
	int y_0 = t->y;
	tree_domination_check(m_l_sys, x_0, y_0);
}

void tree_domination_check(m_l_system* m_l_sys)
{
	for(int y_0 = 0; y_0 < m_l_sys->t_grid.height(); y_0++)
	{
		for(int x_0 = 0; x_0 < m_l_sys->t_grid.width(); x_0++)
		{
			tree_domination_check(m_l_sys, x_0, y_0);
		}
	}
}

void add_str(m_l_system* m_l_sys, float x, float y, float r, int s)
{
	int str_index;
	char* str = m_l_sys->str_set.find_str_and_alloc(&str_index);
	assert(str);
	snprintf(str, SET_STR_MAX_SIZE, "T(%d,%f,%f,%f,0)?(1)\0", s, x, y, r);
	m_l_sys->t_grid.insert_tree(str_index, x, y);
	tree_domination_check(m_l_sys, str_index);//Only go through current string set once for added string
}

void remove_dead_trees(m_l_system* m_l_sys)
{
	//Remove "empty" strings from set
	for(int i = 0; i < m_l_sys->str_set.size(); i++)
	{
		char* str = m_l_sys->str_set.find_str(i);
		if(m_l_sys->str_set.is_allocated(i) && number_of_modules(str) <= 1)
		{
			m_l_sys->t_grid.remove_tree(i);
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

void generate_propagation_vector(m_l_system* m_l_sys, int parent)
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
	if(	new_pos.x < 0.0f || new_pos.x > m_l_sys->t_grid.grid_area_length ||
		new_pos.y < 0.0f || new_pos.y > m_l_sys->t_grid.grid_area_length)
	{//If new_pos is out of bounds	
		new_pos = p_pos; //Set it to parent's position so it will immediately be removed through domination
	}
	float r = (float)(rand() % 400)/600.0f + 0.1f;
	
	snprintf(v, 8, "%f", new_pos.x);
	snprintf(w, 8, "%f", new_pos.y);
	snprintf(t, 8, "%f", r);
	set_global_parameter((l_system*)m_l_sys, 'v', v);
	set_global_parameter((l_system*)m_l_sys, 'w', w);
	set_global_parameter((l_system*)m_l_sys, 't', t);
}

void apply_species_transformation_to_l_system(m_l_system* m_l_sys, int tree)
{
	int species = read_real_parameter_value(m_l_sys->str_set.find_str(tree), 0);
	float shade_tolerance = 0.0f;
	float longevity = 0.0f;
	switch(species)
	{
		case PINE:
			set_global_parameter((l_system*)m_l_sys, 'R', "10");
			set_global_parameter((l_system*)m_l_sys, 'G', "1.4");
			shade_tolerance = 0.3f;
			longevity = 0.95f;
			break;
		case BIRCH:
			set_global_parameter((l_system*)m_l_sys, 'R', "7");
			set_global_parameter((l_system*)m_l_sys, 'G', "1.6");
			shade_tolerance = 0.1f;
			longevity = 0.5f;
			break;
		case ROWAN:
			set_global_parameter((l_system*)m_l_sys, 'R', "2.5");
			set_global_parameter((l_system*)m_l_sys, 'G', "1.2");
			shade_tolerance = 0.5f;
			longevity = 0.6f;
			break;
	}
	set_production_probability((l_system*)m_l_sys, 0, 1.0f-shade_tolerance);
	set_production_probability((l_system*)m_l_sys, 1, shade_tolerance);
	set_production_probability((l_system*)m_l_sys, 2, longevity);
	set_production_probability((l_system*)m_l_sys, 3, 1.0f-longevity);
}

void derive_set(m_l_system* m_l_sys)
{
	//printf("Number of trees in str set: %d\n", m_l_sys->str_set.number_allocated());
	//printf("Number of trees in grid: %d\n", m_l_sys->t_grid.number_of_trees());
	//Derive strs in str_set
	for(int i = 0; i < m_l_sys->str_set.size(); i++)
	{
		//Generate position within a radius of 10.0f of the current str
		//If out of bounds, use a default vector
		if(m_l_sys->str_set.is_allocated(i))
		{
			generate_propagation_vector(m_l_sys, i);
			apply_species_transformation_to_l_system(m_l_sys, i);
			derive_str((l_system*)m_l_sys, m_l_sys->str_set.find_str(i));
		}
	}
	//Prune branched strings
	for(int i = 0; i < m_l_sys->str_set.size(); i++)
	{
		if(m_l_sys->str_set.is_allocated(i))
		{
			int index = -1;
			char* new_str = m_l_sys->str_set.find_str_and_alloc(&index);
			new_str[0] = 0;
			prune_branches(m_l_sys->str_set.find_str(i),new_str);
			if(strlen(new_str) == 0)
			{
				new_str[0] = 0;
				m_l_sys->str_set.free(index);
			}
			else
			{
				//Get new str x and y
				float x = read_real_parameter_value(new_str, 1);
				float y = read_real_parameter_value(new_str, 2);
				m_l_sys->t_grid.insert_tree(index, x, y);
			}
		}
	}
	tree_domination_check(m_l_sys);
	remove_dead_trees(m_l_sys);
	//printf("Final number of trees in str set: %d\n", m_l_sys->str_set.number_allocated());
	//printf("Final number of trees in grid: %d\n", m_l_sys->t_grid.number_of_trees());
}
