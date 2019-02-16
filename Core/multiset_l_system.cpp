#include "multiset_l_system.h"

tree_node* get_next_tree_node(tree_grid* trees)
{
	int i = 0;
	for(; trees->in_use[i]; i++);
	trees->in_use[i] = true;
	return trees->node_array+i;
}

int tree_grid_width(tree_grid* trees)
{
	return (int)(trees->forest_width/trees->bucket_width);
}

int tree_grid_height(tree_grid* trees)
{
	return (int)(trees->forest_height/trees->bucket_height);
}

void insert_into_grid(tree_grid* trees, float x, float y, int str_ref)
{
	int bucket_x = (int)(x/trees->bucket_width);
	int bucket_y = (int)(y/trees->bucket_height);

	int grid_width = tree_grid_width(trees);
	int bucket = bucket_y*grid_width + bucket_x;

	tree_node** node_location = trees->grid + bucket;
	while(*node_location)
	{
		node_location = &((*node_location)->next);
	}
	tree_node* new_node = get_next_tree_node(trees);
	new_node->str_ref = str_ref;
	new_node->next = NULL;
	*node_location = new_node;
}

int find_surrounding_grid_buckets(tree_grid* trees, int center_bucket, int surrounding_buckets[8])
{
	int grid_width = tree_grid_width(trees);
	int grid_height = tree_grid_height(trees);
	int number_of_buckets = 0;
	int center_bucket_y = center_bucket/grid_width;
	int center_bucket_x = center_bucket - center_bucket_y*grid_width;

	if(center_bucket_y > 0)
	{
		int x = center_bucket_x - 1;
		int y = center_bucket_y - 1;
		int b = 3;
		if(x < 0) 
		{
			b += x;
			x = 0;
		}
		else if(x + b >= grid_width)
		{
			b -= (x+b - grid_width + 1);
		}
		for(int i = 0; i < b; i++)
		{
			int bucket = y*grid_width + x;
			surrounding_buckets[number_of_buckets] = bucket;
			number_of_buckets++;
		}
	}

	if(center_bucket_y < grid_height - 1)
	{
		int x = center_bucket_x - 1;
		int y = center_bucket_y + 1;
		int b = 3;
		if(x < 0)
		{
			b += x;
			x = 0;
		}
		else if(x + b >= grid_width)
		{
			b -= (x+b - grid_width + 1);
		}
		for(int i = 0; i < b; i++)
		{
			int bucket = y*grid_width + x;
			surrounding_buckets[number_of_buckets] = bucket;
			number_of_buckets++;
		}
	}

	if(center_bucket_x > 0)
	{
		int x = center_bucket_x - 1;
		int y = center_bucket_y;
		int bucket = y*grid_width + x;
		surrounding_buckets[number_of_buckets] = bucket;
		number_of_buckets++;
	}

	if(center_bucket_x < grid_width - 1)
	{
		int x = center_bucket_x + 1;
		int y = center_bucket_y;
		int bucket = y*grid_width + x;
		surrounding_buckets[number_of_buckets] = bucket;
		number_of_buckets++;
	}
	return number_of_buckets;
}

tree_node* get_bucket_list_head(tree_grid* trees, int bucket)
{
	return trees->grid[bucket];
}

int number_of_grid_buckets(tree_grid* trees)
{
	//Number = width of forest/width of bucket * height of forest/height of bucket
	return (trees->forest_width/trees->bucket_width) * (trees->forest_height/trees->bucket_height);
}

m_l_system create_m_l_system(int str_set_max, float forest_width, float forest_height, float bucket_width, float bucket_height)
{
	m_l_system new_sys = {};
	new_sys.str_set = (char*)malloc(SET_STR_MAX_SIZE*str_set_max);
	new_sys.str_set_size = 0;
	new_sys.trees.bucket_width = bucket_width;
	new_sys.trees.bucket_height = bucket_height;
	new_sys.trees.forest_width = forest_width;
	new_sys.trees.forest_height = forest_height;
	new_sys.trees.node_array = (tree_node*)malloc(sizeof(tree_node)*str_set_max);
	new_sys.trees.in_use = (bool*)malloc(sizeof(bool)*str_set_max);
	new_sys.trees.grid = (tree_node**)malloc(sizeof(tree_node*)*tree_grid_width(&new_sys.trees)*tree_grid_height(&new_sys.trees));
	memset(new_sys.trees.in_use, 0, sizeof(bool)*str_set_max);
	memset(new_sys.trees.node_array, 0, sizeof(tree_node)*str_set_max);
	return new_sys;
}

void destroy_m_l_system(m_l_system* m_l_sys)
{
	free(m_l_sys->str_set);
	free(m_l_sys->trees.node_array);
	free(m_l_sys->trees.in_use);
	free(m_l_sys->trees.grid);
}

void print_str_set(m_l_system* m_l_sys)
{
	for(int i = 0; i < m_l_sys->str_set_size; i++)
	{
		printf("%d: %s\n", i, get_str_from_set(m_l_sys, i));
	}
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
	insert_into_grid(&m_l_sys->trees, x, y, m_l_sys->str_set_size);
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
	for(int i = 0; i < number_of_grid_buckets(&m_l_sys->trees); i++)
	{
		int collision_buckets[8] = {};
		int number_of_buckets_to_check = find_surrounding_grid_buckets(&m_l_sys->trees, i, collision_buckets);

		for(int j = 0; j < number_of_buckets_to_check; j++)
		{
			tree_node* t = get_bucket_list_head(&m_l_sys->trees, i);
			while(t)
			{
				char* t_str = get_str_from_set(m_l_sys, t->str_ref);
				tree_node* u = get_bucket_list_head(&m_l_sys->trees, collision_buckets[j]);
				while(u)
				{
					char* u_str = get_str_from_set(m_l_sys, u->str_ref);
					if(number_of_modules(t_str) > 1 && number_of_modules(u_str) > 1)
					{
						if(trees_intersect(t_str, u_str))
						{
							determine_dominated_tree(t_str, u_str);
						}
					}
					u = u->next;
				}
				t = t->next;
			}
		}

	}
	/*
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
	*/
}
