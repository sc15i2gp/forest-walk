#pragma once
#include "l_system.h"
#include "species_info.h"
#include "mesh.h"
#include "turtle.h"

struct tree_model_generator
{
	int age;

	//Pine, birch + rowan
	char axioms[3][64];
	
	l_system l_systems[3];
	
	char* tree_str_buffer;
	
	void generate_tree_model(tree_mesh_group*);
	void derive_tree_str(int,int);
	void load_axiom(int);
	void set_seed(long int);
};

tree_model_generator create_tree_model_generator();
void destroy_tree_model_generator(tree_model_generator);
