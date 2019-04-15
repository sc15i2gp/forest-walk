#pragma once
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//grid row width = 20 node pts
//grid height = 20 rows

#define BUCKET_LENGTH 20

//A tree node contains info about a single tree in the string set
//tree_node also forms a linked list
struct tree_node
{
	bool in_use;
	bool changed;
	int str_ref;
	int point_ref;
	int x; //Grid_x
	int y; //Grid_y
	float _x; //Actual_x
	float _y; //Actual_y
	float _r; //Radius
	int species;
	int age;
	bool old_age;
	int dominated;
	long int seed;
	tree_node* previous;
	tree_node* next;
};

//Spatially partitions tree_nodes into a fixed size grid
//Each bucket of the grid is a linked list of tree_nodes
struct tree_grid
{
	tree_node* node_array;
	int node_array_size;

	int grid_area_length;
	tree_node*** node_grid;

	tree_node* find_available_node();
	tree_node* find_node_by_str_ref(int);

	void insert_tree(int,float,float);
	void remove_tree(int);
	void remove_all_trees();

	int number_of_trees();

	int height();
	int width();

	tree_node* bucket(int,int);
	int number_of_used_buckets();
};

tree_grid create_tree_grid(int,int);
void destroy_tree_grid(tree_grid*);
