#include "tree_grid.h"

tree_grid create_tree_grid(int node_array_size, int grid_area_length)
{
	tree_grid new_grid = {};

	new_grid.node_array = (tree_node*)malloc(sizeof(tree_node)*node_array_size);
	new_grid.node_array_size = node_array_size;
	memset(new_grid.node_array, 0, sizeof(tree_node)*node_array_size);

	int num_nodes_in_length = grid_area_length/BUCKET_LENGTH;
	new_grid.grid_area_length = grid_area_length;

	new_grid.node_grid = (tree_node***)malloc(sizeof(tree_node**)*num_nodes_in_length);
	for(int i = 0; i < num_nodes_in_length; i++)
	{//For each node row in grid
		new_grid.node_grid[i] = (tree_node**)malloc(sizeof(tree_node*)*num_nodes_in_length);
		memset(new_grid.node_grid[i], 0, sizeof(tree_node*)*num_nodes_in_length);
	}

	return new_grid;
}

void destroy_tree_grid(tree_grid* t)
{
	int num_nodes_in_length = t->grid_area_length/BUCKET_LENGTH;

	for(int i = num_nodes_in_length-1; i >= 0; i--)
	{
		free(t->node_grid[i]);
	}

	free(t->node_grid);
	free(t->node_array);
}

tree_node* tree_grid::find_available_node()
{
	tree_node* node = node_array;
	while(node->in_use) node++;
	return node;
}

tree_node* tree_grid::find_node_by_str_ref(int t)
{
	for(int i = 0; i < node_array_size; i++)
	{//For each node in the tree grid
		if(node_array[i].str_ref == t && node_array[i].in_use) return node_array+i;
	}

	return NULL;
}

//Appends new tree data to a bucket's linked list
void tree_grid::insert_tree(int str_ref, float f_x, float f_y)
{
	int x = f_x/BUCKET_LENGTH;
	int y = f_y/BUCKET_LENGTH;
	
	tree_node** node_loc = &(node_grid[y][x]);
	tree_node* previous = NULL;

	while(*node_loc)
	{//While the end of the bucket's list hasn't been found
		previous = *node_loc;
		node_loc = &((*node_loc)->next);
	}

	tree_node* new_tree_node = find_available_node();

	new_tree_node->str_ref = str_ref;
	new_tree_node->x = x;
	new_tree_node->y = y;
	new_tree_node->previous = previous;
	new_tree_node->next = NULL;
	new_tree_node->in_use = true;
	new_tree_node->seed = -1;
	new_tree_node->changed = true;

	*node_loc = new_tree_node;
}

void tree_grid::remove_tree(int str_ref)
{
	tree_node* node = find_node_by_str_ref(str_ref);
	node->in_use = false;

	if(node->previous) node->previous->next = node->next; //If node to remove wasn't head of bucket list
	else node_grid[node->y][node->x] = node->next;

	if(node->next) node->next->previous = node->previous; //If node to remove wasn't at the end of the bucket list
}

void tree_grid::remove_all_trees()
{
	int num_nodes_in_length = grid_area_length/BUCKET_LENGTH;

	for(int i = 0; i < num_nodes_in_length; i++)
	{
		for(int j = 0; j < num_nodes_in_length; j++) node_grid[i][j] = NULL;
	}

	memset(node_array, 0, sizeof(tree_node)*node_array_size);
}

int tree_grid::number_of_trees()
{
	int total = 0;
	tree_node* node = node_array;

	for(int i = 0; i < node_array_size; i++, node++) if(node->in_use) total++;

	return total;
}

int tree_grid::height()
{
	return grid_area_length/BUCKET_LENGTH;
}

int tree_grid::width()
{
	return grid_area_length/BUCKET_LENGTH;
}

tree_node* tree_grid::bucket(int x, int y)
{
	return node_grid[y][x];
}

int tree_grid::number_of_used_buckets()
{
	int total = 0;
	int num_nodes_in_length = grid_area_length/BUCKET_LENGTH;

	for(int y = 0; y < num_nodes_in_length; y++)
	{
		for(int x = 0; x < num_nodes_in_length; x++) if(bucket(x,y)) total++;
	}

	return total;
}
