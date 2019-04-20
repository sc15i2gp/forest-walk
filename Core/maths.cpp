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

vec3 operator-(vec3 v1, vec3 v2)
{
	return vec3{v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

vec3 operator/(vec3 v,float f)
{
	return vec3{v.x/f, v.y/f, v.z/f};
}

float magnitude(vec3 v)
{
	return sqrt(dot(v, v));
}

vec3 normalise(vec3 v)
{
	return v/magnitude(v);
}

vec3 normal_2d(vec3 v)
{
	return vec3{v.y, -v.x, v.z};
}

float dot(vec3 v1, vec3 v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

float dot(float* v1, float* v2)
{
	return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

vec3 cross(vec3 v1, vec3 v2)
{
	vec3 v;
	v.x = v1.y*v2.z - v1.z*v2.y;
	v.y = v1.z*v2.x - v1.x*v2.z;
	v.z = v1.x*v2.y - v1.y*v2.x;
	return v;
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


vec3 matrix_multiply_4x4(vec3 v, float* matrix)
{
	vec3 w = {0.0f, 0.0f, 0.0f};
	w.x = matrix[0]*v.x + matrix[1]*v.y + matrix[2]*v.z + matrix[3];
	w.y = matrix[4]*v.x + matrix[5]*v.y + matrix[6]*v.z + matrix[7];
	w.z = matrix[8]*v.x + matrix[9]*v.y + matrix[10]*v.z + matrix[11];
	return w;
}

void transpose_matrix_4x4(float* matrix)
{
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			float temp = matrix[4*i+j];
			matrix[4*i+j] = matrix[4*j+i];
			matrix[4*j+i] = temp;
		}
	}
}

float min(float f, float g)
{
	return (f < g) ? f : g;
}

float max(float f, float g)
{
	return (f > g) ? f : g;
}

bool do_circles_intersect(float x_0, float y_0, float r_0, float x_1, float y_1, float r_1)
{
	float dist = sqrt((x_0 - x_1)*(x_0 - x_1) + (y_0 - y_1)*(y_0 - y_1));
	float total_radius = r_0 + r_1;
	return dist < total_radius;
}

vec3 generate_vector_within_radius(vec3 center, float r_max)
{
	int c = rand()%100; //How much of r_max away from the center should v be
	float c_f = (float)c/100.0f;
	float r = r_max*c_f;
	vec3 v = {r, 0.0f, 0.0f};

	int a = rand()%360; //Angle to rotate around v
	v = rotate_about_axis(v, vec3{0.0f,0.0f,1.0f}, (float)a);
	v = v + center;
	return v;
}
