#pragma once
#include <QGLFunctions>
#include "maths.h"
struct mesh
{
	int vertex_data_length;
	int max_vertex_data_length;
	vec3* vertex_data;

	int number_of_indices;
	int max_number_of_indices;
	GLuint* indices;
	void push_vertex_data(vec3*,int, GLuint*,int);
};

struct tree_mesh_group
{
	mesh branch_mesh;
	mesh leaf_mesh;
	mesh fruit_mesh;
};

mesh create_mesh(int vertex_data_buffer_length, int index_buffer_length);
void destroy_mesh(mesh m);