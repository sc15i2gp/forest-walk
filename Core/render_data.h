#pragma once
#include <stdio.h>
#include <assert.h>
#include <GL/gl.h>
#include "tree_grid.h"

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

//OpenGL buffer references to a model in GPU memory
struct render_object
{
	GLuint vertex_buffer;
	GLuint index_buffer;
	int number_of_indices;
};

//Contains complete tree model (branches, leaves and fruit models)
struct tree_buffer_object
{
	render_object branch_obj;
	render_object leaf_obj;
	render_object fruit_obj;
};

//Contains data about recently generated tree models
struct model_cache
{
	int ref_count;
	int lod;
	int model_size;
	tree_buffer_object model;
};

//Points to a model_cache in the tree_model_map structure
struct tree_model_ref
{
	int model_ref;
	tree_node* tree;
};

//tree_model_map: key-value map
//maps a tree model ref (key) to a model cache (value)
struct tree_model_map
{
	int number_of_tree_model_refs;
	int number_of_tree_model_caches;
	int buffered_model_data_size;

	tree_model_ref* tree_model_refs;
	model_cache* tree_models;
	
	void add_model_ref(tree_node*,int=-1);
	int model_ref(tree_node*);

	//Level of detail for a particular tree_node
	int lod(tree_node*);
	int lod(int);
	
	int model_ref_count(tree_node*);
	int model_ref_count(int);
	
	void release_ref(tree_model_ref*);
	void release_ref(tree_node*);
	void release_ref(int);

	int find_available_model_cache();
	tree_model_ref* find_tree_model_ref(tree_node*);
	
	//Gets model data from a tree_node's model cache
	tree_buffer_object find_model(tree_node*);
	tree_buffer_object find_model(int);

	void set_model(tree_node*, tree_buffer_object, int, int);
	bool tree_has_model(tree_node*);

	bool ref_in_use(int); //tree_model_refs[i]->tree != NULL
	bool tree_in_use(int); //tree_model_refs[i]->tree->in_use != NULL

	void print();
};

tree_model_map create_tree_model_map(int,int);
void destroy_tree_model_map(tree_model_map);
