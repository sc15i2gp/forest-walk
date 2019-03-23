#pragma once
#include <stdio.h>
#include <assert.h>
#include <GL/gl.h>
#include "tree_grid.h"

struct render_object
{
	GLuint vertex_buffer;
	GLuint index_buffer;
	int number_of_indices;
};

struct tree_buffer_object
{
	render_object branch_obj;
	render_object leaf_obj;
	render_object fruit_obj;
};

struct model_cache
{
	int ref_count;
	int lod;
	tree_buffer_object model;
};

struct tree_model_ref
{
	int model_ref;
	tree_node* tree;
};

//Render queue
struct render_queue
{
	int length;
	int max_length;
	tree_node** to_render_buffer;
	tree_node** head;
	void sort();
	void push(tree_node*);
	tree_node* pop();
	tree_node* find_item(int);
	void reset();
	void print();
};

render_queue create_render_queue(int);
void destroy_render_queue(render_queue);

//tree_model_map
struct tree_model_map
{
	int number_of_tree_model_refs;
	int number_of_tree_model_caches;
	tree_model_ref* tree_model_refs;// = NULL;
	model_cache* tree_models; //[4096] = {};
	
	void add_model_ref(tree_node*,int=-1);
	int model_ref(tree_node*);

	int lod(tree_node*);
	int lod(int);
	
	int model_ref_count(tree_node*);
	int model_ref_count(int);
	
	void release_ref(tree_model_ref*);
	void release_ref(tree_node*);
	void release_ref(int);

	int find_available_model_cache();
	tree_model_ref* find_tree_model_ref(tree_node*);
	
	tree_buffer_object find_model(tree_node*);
	tree_buffer_object find_model(int);
	void set_model(tree_node*, tree_buffer_object, int);
	bool tree_has_model(tree_node*);

	bool ref_in_use(int); //tree_model_refs[i]->tree != NULL
	bool tree_in_use(int); //tree_model_refs[i]->tree->in_use != NULL

	void print();
};

tree_model_map create_tree_model_map(int,int);
void destroy_tree_model_map(tree_model_map);
