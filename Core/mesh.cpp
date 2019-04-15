#include "mesh.h"

int tree_mesh_group::size_in_bytes()
{
	return branch_mesh.size_in_bytes() + leaf_mesh.size_in_bytes() + fruit_mesh.size_in_bytes();
}

int mesh::size_in_bytes()
{
	return 2*3*sizeof(GLfloat)*vertex_data_length + sizeof(GLuint)*number_of_indices;
}

void mesh::push_vertex_data(vec3* new_vertex_data, int new_vertex_data_length, GLuint* new_indices, int number_of_new_indices)
{
	if(this->vertex_data_length + new_vertex_data_length > this->max_vertex_data_length)
	{
		printf("%s (%d) error: mesh vertex data buffer is full\n",__func__,__LINE__);
		return;
	}

	if(this->number_of_indices + number_of_new_indices > this->max_number_of_indices)
	{
		printf("%s (%d) error: mesh index buffer is full\n",__func__,__LINE__);
		return;
	}

	int index_offset = this->vertex_data_length;
	//Copy vertices and normals
	for(int i = 0; i < 2*new_vertex_data_length; i += 2)
	{
		vec3* vertex_data_location = this->vertex_data + 2*this->vertex_data_length;

		vertex_data_location[0] = new_vertex_data[i];
		vertex_data_location[1] = new_vertex_data[i+1];

		this->vertex_data_length++;
	}
	//Copy indices
	for(int i = 0; i < number_of_new_indices; i += 3)
	{
		GLuint* index_location = this->indices + this->number_of_indices;

		index_location[0] = new_indices[i] + index_offset;
		index_location[1] = new_indices[i+1] + index_offset;
		index_location[2] = new_indices[i+2] + index_offset;

		this->number_of_indices += 3;
	}
}

mesh create_mesh(int vertex_data_buffer_length, int index_buffer_length)
{
	vec3* vertex_data_buffer = (vec3*)malloc(sizeof(vec3)*vertex_data_buffer_length);
	GLuint* index_buffer = (GLuint*)malloc(sizeof(GLuint)*index_buffer_length);
	return mesh{0, vertex_data_buffer_length/2, vertex_data_buffer, 0, index_buffer_length, index_buffer};
}

void destroy_mesh(mesh m)
{
	free(m.vertex_data);
	free(m.indices);
}
