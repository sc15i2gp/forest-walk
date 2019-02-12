#include "maths.h"

float pow2(float n)
{
	return n*n;
}

vec3 operator+(vec3 v1, vec3 v2)
{
	return vec3{v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

vec3 operator*(float f, vec3 v)
{
	return vec3{f*v.x, f*v.y, f*v.z};
}

vec3 operator-(vec3 v)
{
	return vec3{-v.x, -v.y, -v.z};
}
#include <stdio.h>
vec3 cross(vec3 v1, vec3 v2)
{
	vec3 v;
	v.x = v1.y*v2.z - v1.z*v2.y;
	v.y = v1.z*v2.x - v1.x*v2.z;
	v.z = v1.x*v2.y - v1.y*v2.x;
	return v;
}

float dot(vec3 v1, vec3 v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

float dot(float* v1, float* v2)
{
	return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

float magnitude(vec3 v)
{
	return sqrt(dot(v, v));
}

vec3 normalise(vec3 v)
{
	return v/magnitude(v);
}

vec3 operator/(vec3 v,float f)
{
	return vec3{v.x/f, v.y/f, v.z/f};
}

//Anticlockwise rotation
vec3 rotate_about_axis(vec3 v, vec3 axis, float angle_deg)
{
	float angle = angle_deg * (PI/180);
	float c = cos(angle);
	float s = sin(angle);

	float x = 	v.x*(c + pow2(axis.x)*(1-c)) + 
			v.y*(axis.x*axis.y*(1-c) - axis.z*s) +
			v.z*(axis.x*axis.z*(1-c) + axis.y*s);
	float y = 	v.x*(axis.y*axis.x*(1-c) + axis.z*s) +
			v.y*(c + pow2(axis.y)*(1-c)) +
			v.z*(axis.y*axis.z*(1-c) - axis.x*s);
	float z =	v.x*(axis.z*axis.x*(1-c) - axis.y*s) +
			v.y*(axis.z*axis.y*(1-c) + axis.x*s) +
			v.z*(c + pow2(axis.z)*(1-c));
	return vec3{x, y, z};
}

vec3 operator-(vec3 v1, vec3 v2)
{
	return vec3{v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

void print_vector(vec3 v)
{
	printf("%f %f %f ", v.x, v.y, v.z);
}
