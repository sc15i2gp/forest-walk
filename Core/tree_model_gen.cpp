#include "tree_model_gen.h"

tree_model_generator create_tree_model_generator()
{
	tree_model_generator t = {};
	t.tree_str_buffer = (char*)malloc(MAX_DERIVED_OUTPUT_SIZE);
	memset(t.tree_str_buffer, 0, MAX_DERIVED_OUTPUT_SIZE);
	
	load_l_system(&t.l_systems[0], "../l-systems_for_work/pine_with_leaves.lsys", t.axioms[0]);
	load_l_system(&t.l_systems[1], "../l-systems_for_work/birch_with_leaves.lsys", t.axioms[1]);
	load_l_system(&t.l_systems[2], "../l-systems_for_work/rowan_with_leaves_and_fruit.lsys", t.axioms[2]);
	print_l_system(&t.l_systems[0], "Pine");
	print_l_system(&t.l_systems[1], "Birch");
	print_l_system(&t.l_systems[2], "Rowan");
	return t;
}

void destroy_tree_model_generator(tree_model_generator t)
{
	free(t.tree_str_buffer);
}

void tree_model_generator::generate_tree_model(tree_mesh_group* t_mesh, int lod)
{
	run_turtle(tree_str_buffer, t_mesh, lod);
}

void tree_model_generator::derive_tree_str(int derive_count, int species)
{
	l_system* l = NULL;
	int max_derive_count = 0;
	int derive_coefficient = 0;
	switch(species)
	{
		case PINE:
			l = &l_systems[0];
			max_derive_count = 20;
			derive_coefficient = 2;
			break;
		case BIRCH:
			l = &l_systems[1];
			max_derive_count = 20;
			derive_coefficient = 2;
			break;
		case ROWAN:
			l = &l_systems[2];
			max_derive_count = 10;
			derive_coefficient = 2;
			break;
	}
	int final_age = age + derive_coefficient*derive_count;
	if(final_age > max_derive_count) derive_count = max_derive_count - age;
	else derive_count = final_age - age;
	for(int i = 0; i < derive_count; i++) derive_str(l, tree_str_buffer);
	age += derive_count;
}

void tree_model_generator::load_axiom(int species)
{
	age = 0;
	char* axiom;
	switch(species)
	{
		case PINE:
			axiom = axioms[0];
			break;
		case BIRCH:
			axiom = axioms[1];
			break;
		case ROWAN:
			axiom = axioms[2];
			break;
	}
	strcpy(tree_str_buffer, axiom);
}

void tree_model_generator::set_seed(long int seed)
{
	srand(seed);
}
