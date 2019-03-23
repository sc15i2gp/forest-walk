#pragma once
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//ecosystem width,height = 400 units
//node width,height = 20 units
//grid row width = 20 node pts
//grid height = 20 rows

#define BUCKET_LENGTH 20

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

struct tree_grid
{
	tree_node* node_array;
	int node_array_size;
	int grid_area_length;
	tree_node*** node_grid;
	bool derived;
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
