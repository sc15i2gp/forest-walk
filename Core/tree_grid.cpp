#include "tree_grid.h"

tree_grid create_tree_grid(int node_array_size)
{
	tree_grid new_grid = {};
	new_grid.node_array = (tree_node*)malloc(sizeof(tree_node)*node_array_size);
	new_grid.node_array_size = node_array_size;
	memset(new_grid.node_array, 0, sizeof(tree_node)*node_array_size);

	new_grid.node_grid = (tree_node***)malloc(sizeof(tree_node**)*20);
	for(int i = 0; i < 20; i++)
	{//For each node row in grid
		new_grid.node_grid[i] = (tree_node**)malloc(sizeof(tree_node*)*20);
		memset(new_grid.node_grid[i], 0, sizeof(tree_node*)*20);
	}
	return new_grid;
}

void destroy_tree_grid(tree_grid* t)
{
	for(int i = 19; i >= 0; i--)
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

void tree_grid::insert_tree(int str_ref, float f_x, float f_y)
{
	int x = f_x/20;
	int y = f_y/20;
	
	tree_node** node_loc = &(node_grid[y][x]);
	tree_node* previous = NULL;
	while(*node_loc)
	{
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
	*node_loc = new_tree_node;
}

void tree_grid::remove_tree(int str_ref)
{
	tree_node* node = node_array;
	for(int i = 0; i < node_array_size; i++, node++)
	{
		if(node->str_ref == str_ref && node->in_use)
		{
			node->in_use = false;
			if(node->previous) node->previous->next = node->next;
			else node_grid[node->y][node->x] = node->next;
			if(node->next) node->next->previous = node->previous;
			break;
		}
	}
}

void tree_grid::remove_all_trees()
{
	for(int i = 0; i < 20; i++)
	{
		for(int j = 0; j < 20; j++)
		{
			node_grid[i][j] = NULL;
		}
	}
	memset(node_array, 0, sizeof(tree_node)*node_array_size);
}

int tree_grid::number_of_trees()
{
	int total = 0;
	tree_node* node = node_array;
	for(int i = 0; i < node_array_size; i++, node++)
	{
		if(node->in_use) total++;
	}
	return total;
}

int tree_grid::height()
{
	return 20;
}

int tree_grid::width()
{
	return 20;
}

tree_node* tree_grid::bucket(int x, int y)
{
	return node_grid[y][x];
}

int tree_grid::number_of_used_buckets()
{
	int total = 0;
	for(int y = 0; y < 20; y++)
	{
		for(int x = 0; x < 20; x++)
		{
			if(bucket(x,y)) total++;
		}
	}
	return total;
}
