#pragma once
#include <math.h>
#define PI 3.14159265359

inline
float pow2(float n);

struct vec3
{
	float x;
	float y;
	float z;
};

vec3 operator+(vec3 v1, vec3 v2);
vec3 operator*(float f, vec3 v);
vec3 operator-(vec3 v);
vec3 operator-(vec3, vec3);
vec3 operator/(vec3,float);

float magnitude(vec3);

vec3 normalise(vec3 v);
vec3 normal_2d(vec3 v);

float dot(vec3 v1, vec3 v2);
float dot(float* v1, float* v2);

vec3 cross(vec3 v1, vec3 v2);

//Anticlockwise rotation
vec3 rotate_about_axis(vec3 v, vec3 axis, float angle);

vec3 matrix_multiply_4x4(vec3 v, float* matrix);
void transpose_matrix_4x4(float* matrix);

float min(float,float);
float max(float,float);

float circle_area(float);
bool do_circles_intersect(float x_0, float y_0, float r_0, float x_1, float y_1, float r_1);
vec3 generate_vector_within_radius(vec3 center, float r_max);
