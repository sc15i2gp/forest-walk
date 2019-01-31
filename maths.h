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

vec3 normalise(vec3 v);

float magnitude(vec3);
float dot(vec3 v1, vec3 v2);
float dot(float* v1, float* v2);
vec3 cross(vec3 v1, vec3 v2);
//Anticlockwise rotation
vec3 rotate_about_axis(vec3 v, vec3 axis, float angle);
void print_vector(vec3 v);