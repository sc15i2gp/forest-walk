#pragma once
#include "maths.h"
#include "mesh.h"
#include "string_functions.h"
struct polygon
{
	vec3 positions[8] = {};
	int vertex_count = 0;
	void push_vertex(vec3);
};

#include "polygon_stack.h"

struct turtle
{
	struct
	{
		vec3 heading;
		vec3 left;
		vec3 up;
	} orientation;
	vec3 position;
	float width;
	void move_forward(float); //F
	void turn_left(float); //+
	void turn_right(float); //-
	void roll_backward(float); //^
	void roll_forward(float); //&
	void roll_left(float); // /
	void roll_right(float); // 
};

#include "turtle_stack.h"

void run_turtle(char* input, tree_mesh_group* tree, float default_distance = 1.0f, float default_branch_length = 0.5f, float default_angle = 90.0f);
