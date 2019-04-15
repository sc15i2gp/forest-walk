#pragma once
#include "l_system.h"
#include "species_info.h"
#include "mesh.h"
#include "turtle.h"

//This structure has a buffer for a string, can switch species l_systems and control generation of tree models
//In order to speed up model generation, the string in the buffer is derived a certain number of times and the result is stored
//This speedup works best when models with the same seeds and species are derived in sequence by age
struct tree_model_generator
{
	int age;

	//Pine, birch + rowan
	char axioms[3][64];	
	l_system l_systems[3];
	
	char* tree_str_buffer;
	
	int generate_tree_model(tree_mesh_group*,int);
	void derive_tree_str(int,int);

	void load_axiom(int);
	void set_seed(long int);
};

tree_model_generator create_tree_model_generator();
void destroy_tree_model_generator(tree_model_generator);
