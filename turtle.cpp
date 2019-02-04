#include "turtle.h"

void polygon::push_vertex(vec3 position)
{
	positions[vertex_count] = position;
	vertex_count++;
}

void turtle::move_forward(float distance)
{
	this->position = this->position + distance * this->orientation.heading;
}

void turtle::turn_left(float angle)
{
	this->orientation.heading = rotate_about_axis(this->orientation.heading, this->orientation.up, angle);
	this->orientation.left = rotate_about_axis(this->orientation.left, this->orientation.up, angle);
}

void turtle::turn_right(float angle)
{
	turn_left(360.0f - angle);
}

void turtle::roll_backward(float angle)
{
	this->orientation.heading = rotate_about_axis(this->orientation.heading, this->orientation.left, angle);
	this->orientation.up = rotate_about_axis(this->orientation.up, this->orientation.left, angle);
}

void turtle::roll_forward(float angle)
{
	roll_backward(360.0f - angle);
}

void turtle::roll_left(float angle)
{
	roll_right(360.0f - angle);
}

void turtle::roll_right(float angle)
{
	this->orientation.up = rotate_about_axis(this->orientation.up, this->orientation.heading, angle);
	this->orientation.left = rotate_about_axis(this->orientation.left, this->orientation.heading, angle);
}

void push_fruit_sphere_to_tree_mesh(mesh* fruit_mesh, turtle* t, float radius, int v)
{
	//TODO
}

void push_leaf_polygon_to_tree_mesh(mesh* leaf_mesh, turtle* t, polygon* p)
{
	vec3 e_0 = p->positions[1] - p->positions[0];
	vec3 e_1 = p->positions[2] - p->positions[1];
	vec3 normal = normalise(cross(e_1, e_0));
	if(p->vertex_count == 3)
	{
		GLuint indices[] = {0, 1, 2};
		vec3 vertex_data[6];
		vertex_data[0] = p->positions[0];
		vertex_data[1] = normal;
		vertex_data[2] = p->positions[1];
		vertex_data[3] = normal;
		vertex_data[4] = p->positions[2];
		vertex_data[5] = normal;
		leaf_mesh->push_vertex_data(vertex_data, 3, indices, 3);
	}
	else if(p->vertex_count == 4)
	{
		GLuint indices[] = {0, 1, 3, 1, 2, 3};
		vec3 vertex_data[8];
		for(int i = 0; i < 4; i++)
		{
			vertex_data[2*i] = p->positions[i];
			vertex_data[2*i+1] = normal;
		}
		leaf_mesh->push_vertex_data(vertex_data, 4, indices, 6);
	}
	else
	{
		//TODO
	}
}

void push_branch_cylinder_to_tree_mesh(mesh* branch_mesh, turtle* t, float r, float h, int v)
{
        int number_of_vertex_positions = 2*(v+1); //2 for each circle, +1 for the central vertex
        int number_of_triangles = 4*v;
        int number_of_indices = 3*number_of_triangles;
        int number_of_faces = 2+v;
        int number_of_vertex_rows = 6*v + 2;
        int length_of_vertex_row = 2; //position + normal vectors
        vec3* positions = (vec3*)malloc(sizeof(vec3)*number_of_vertex_positions);
        float v_angle = 360.0f/(float)v;
        //First circle vertex positions counter clockwise from (r, 0)
        for(int i = 0; i < v; i++)
        {
                positions[i] = t->position + rotate_about_axis(r*(-t->orientation.left), t->orientation.heading, i*v_angle);
        }
        positions[v] = t->position;

        //Copy positions of vertices and increase height for second circle
        for(int i = 0; i < v+1; i++)
        {
                positions[v+1+i] = positions[i] + h*t->orientation.heading;
        }

        vec3* normals = (vec3*)malloc(sizeof(vec3)*number_of_faces);

        for(int i = 0; i < v; i++)
        {
                vec3 edge = positions[(i+1)%v] - positions[i];
                vec3 normal = cross(edge, t->orientation.heading);
                normal = normalise(normal);
                normals[i] = normal;
        }
        normals[v] = -t->orientation.heading;
      	normals[v+1] = t->orientation.heading;

        int vertex_data_size = sizeof(vec3)*length_of_vertex_row*number_of_vertex_rows;
        vec3* vertex_data = (vec3*)malloc(vertex_data_size);
        vec3* vertex_row = vertex_data;

        //Bottom circle
        for(int i = 0; i < v+1; i++)
        {
                vertex_row[0] = positions[i];
                vertex_row[1] = normals[v];
                vertex_row += length_of_vertex_row;
        }
        //Mid section
        for(int i = 0; i < v; i++)
        {
                vertex_row[0] = positions[i];
                vertex_row[1] = normals[i];
                vertex_row += length_of_vertex_row;
                vertex_row[0] = positions[(i+1)%v];
                vertex_row[1] = normals[i];
                vertex_row += length_of_vertex_row;
                vertex_row[0] = positions[v+1+(i+1)%v];
                vertex_row[1] = normals[i];
                vertex_row += length_of_vertex_row;
                vertex_row[0] = positions[v+i+1];
                vertex_row[1] = normals[i];
                vertex_row += length_of_vertex_row;
        }
        //Top circle
        for(int i = 0; i < v+1; i++)
        {
                vertex_row[0] = positions[v+i+1];
                vertex_row[1] = normals[v+1];
                vertex_row += length_of_vertex_row;
        }

        //Indices
        int indices_size = sizeof(GLuint)*number_of_indices;
        GLuint* indices = (GLuint*)malloc(indices_size);
        GLuint* triangle_indices = indices;
        //Bottom circle
        for(int i = 0; i < v; i++)
        {
                triangle_indices[0] = i;
                triangle_indices[1] = v;
                triangle_indices[2] = (i+1)%v;
                triangle_indices += 3;
        }
        //Mid section
        GLuint rect_indices[] = {0, 1, 2, 2, 3, 0};
        for(int i = 0; i < v; i++)
        {
                for(int j = 0; j < 6; j++)
                {
                        triangle_indices[j] = rect_indices[j] + v + 1 + 4*i;
                }
                triangle_indices += 6;
        }
        //Top circle
        for(int i = 0; i < v; i++)
        {
                int offset = 5*v + 1;
                triangle_indices[0] = offset + i;
                triangle_indices[1] = offset + v;
                triangle_indices[2] = offset + (i+1)%v;
                triangle_indices += 3;
        }

        branch_mesh->push_vertex_data(vertex_data, number_of_vertex_rows, indices, number_of_indices);
        free(positions);
        free(normals);
        free(indices);
        free(vertex_data);
}

void run_turtle(char* input, tree_mesh_group* tree, float default_distance, float default_radius, float default_angle)
{
	turtle t = {};
	t.orientation.heading = vec3{0.0f, 1.0f, 0.0f};
	t.orientation.left = vec3{-1.0, 0.0, 0.0};
	t.orientation.up = vec3{0.0, 0.0, 1.0};

	polygon p = {};

	STACK(turtle) t_stack;
	STACK(polygon) p_stack;
	
	int input_length = number_of_modules(input);
	char* module = input;

	int polygon_depth = 0;
	bool polygon_mode = false;
	for(int i = 0; i < input_length; i++)
	{
		int module_param_count = number_of_parameters(module);
		switch(*module)
		{
			case '[':
				t_stack.push_state(t);
				break;
			case ']':
				t = t_stack.pop_state();
				break;
			case '{':
				p_stack.push_state(p);
				p.vertex_count = 0;
				polygon_mode = true;
				polygon_depth++;
				break;
			case '}':
				push_leaf_polygon_to_tree_mesh(&tree->leaf_mesh, &t, &p);
				p = p_stack.pop_state();
				polygon_depth--;
				if(polygon_depth == 0) polygon_mode = false;
				break;
			case '.':
				p.push_vertex(t.position);
				break;
			case '@':
			{
				float radius = default_radius;
				if(module_param_count > 0) radius = read_real_parameter_value(module);
				push_fruit_sphere_to_tree_mesh(&tree->fruit_mesh, &t, radius, 8);
				break;
			}
			case 'F':
			{
				float radius = default_radius;
				float length = default_distance;
				if(module_param_count > 0)
				{
					length = read_real_parameter_value(module);
					if(module_param_count > 1) radius = read_real_parameter_value(module, 1);
				}
				if(!polygon_mode)
				{
					push_branch_cylinder_to_tree_mesh(&tree->branch_mesh, &t, radius, length, 16);
				}
				t.move_forward(length);
				break;
			}
			case '+':
			{
				float angle = default_angle;
				if(module_param_count > 0) angle = read_real_parameter_value(module);
				t.turn_left(angle);
				break;
			}
			case '-':
			{
				float angle = default_angle;
				if(module_param_count > 0) angle = read_real_parameter_value(module);
				t.turn_right(angle);
				break;
			}
			case '^':
			{
				float angle = default_angle;
				if(module_param_count > 0) angle = read_real_parameter_value(module);
				t.roll_backward(angle);
				break;
			}
			case '&':
			{
				float angle = default_angle;
				if(module_param_count > 0) angle = read_real_parameter_value(module);
				t.roll_forward(angle);
				break;
			}
		}
		module = find_next_module(module);
	}
}
