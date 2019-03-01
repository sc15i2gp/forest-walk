#pragma once
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//ecosystem width,height = 400 units
//node width,height = 20 units
//grid row width = 20 node pts
//grid height = 20 rows

struct tree_node
{
	bool in_use;
	int str_ref;
	int x;
	int y;
	tree_node* previous;
	tree_node* next;
};

struct tree_grid
{
	tree_node* node_array;
	int node_array_size;
	tree_node*** node_grid;
	tree_node* find_available_node();
	void insert_tree(int,float,float);
	void remove_tree(int);
	void remove_all_trees();
	int number_of_trees();
	int height();
	int width();
	tree_node* bucket(int,int);
	int number_of_used_buckets();
};

tree_grid create_tree_grid(int);
void destroy_tree_grid(tree_grid*);
