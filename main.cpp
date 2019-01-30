#include <stdio.h>
#include <QApplication>
#include <QBoxLayout>
#include <QGLWidget>
#include <QGLFunctions>
#include <GL/glu.h>
#include "maths.h"

struct mesh
{
	int vertex_data_length = 0;
	vec3* vertex_data;

	int number_of_indices = 0;
	GLuint* indices;
	void push_vertex_data(vec3*,int, GLuint*,int);
};

mesh create_mesh(int vertex_data_buffer_length, int index_buffer_length)
{
	vec3* vertex_data_buffer = (vec3*)malloc(sizeof(vec3)*vertex_data_buffer_length);
	GLuint* index_buffer = (GLuint*)malloc(sizeof(GLuint)*index_buffer_length);
	return mesh{0, vertex_data_buffer, 0, index_buffer};
}

void destroy_mesh(mesh m)
{
	free(m.vertex_data);
	free(m.indices);
}

struct render_object
{
	GLuint vertex_buffer;
	GLuint index_buffer;
	int number_of_indices;
};
	
struct material
{
	GLfloat ambient[3];
	GLfloat diffuse[3];
	GLfloat specular[3];
	GLfloat shininess;
};

//Note: indices provided as triangles in ccw order
void mesh::push_vertex_data(vec3* new_vertex_data, int new_vertex_data_length, GLuint* new_indices, int number_of_new_indices)
{
	int index_offset = this->vertex_data_length;
	for(int i = 0; i < 2*new_vertex_data_length; i += 2)
	{
		vec3* vertex_data_location = this->vertex_data + 2*this->vertex_data_length;
		vertex_data_location[0] = new_vertex_data[i];
		vertex_data_location[1] = new_vertex_data[i+1];
		this->vertex_data_length++;
	}
	for(int i = 0; i < number_of_new_indices; i += 3)
	{
		GLuint* index_location = this->indices + this->number_of_indices;
		index_location[0] = new_indices[i] + index_offset;
		index_location[1] = new_indices[i+1] + index_offset;
		index_location[2] = new_indices[i+2] + index_offset;
		this->number_of_indices += 3;
	}
}

class GLWidget: public QGLWidget, protected QGLFunctions
{
public:
	GLWidget(QWidget* parent): QGLWidget(parent){}

protected:
	void initializeGL()
	{
		initializeGLFunctions();
		glClearColor(0.1, 0.1, 0.1, 0.0);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glEnable(GL_DEPTH_TEST);
	}

	void resizeGL(int w, int h)
	{
		view_width = w;
		view_height = h;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(80.0f, (float)view_width/(float)view_height, 0.1f, 100.0f);
	}

	void paintGL()
	{
		render_scene(view_width, view_height);
	}

	void render_scene(int,int);
	void render(render_object);
	render_object buffer_mesh(mesh*);
	void set_material(material*);
private:
	int view_width = 0;
	int view_height = 0;
};

render_object GLWidget::buffer_mesh(mesh* m)
{
	render_object buffered_mesh;
	glGenBuffers(1, &buffered_mesh.vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffered_mesh.vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, 2*m->vertex_data_length*sizeof(vec3), m->vertex_data, GL_STATIC_DRAW);
	glGenBuffers(1, &buffered_mesh.index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffered_mesh.index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->number_of_indices*sizeof(GLuint), m->indices, GL_STATIC_DRAW);
	buffered_mesh.number_of_indices = m->number_of_indices;
	return buffered_mesh;
}
void push_cylinder_to_mesh(mesh* m, float r, float h, int v)
{
	vec3 t_position = {0.0, 0.0, 0.0};
	vec3 t_orientation_left = {-1.0, 0.0, 0.0};
	vec3 t_orientation_heading = {0.0, 1.0, 0.0};
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
                positions[i] = t_position + rotate_about_axis(r*(-t_orientation_left), t_orientation_heading, i*v_angle);
        }
        positions[v] = t_position;

        //Copy positions of vertices and increase height for second circle
        for(int i = 0; i < v+1; i++)
        {
                positions[v+1+i] = positions[i] + h*t_orientation_heading;
        }

        vec3* normals = (vec3*)malloc(sizeof(vec3)*number_of_faces);

        for(int i = 0; i < v; i++)
        {
                vec3 edge = positions[(i+1)%v] - positions[i];
                vec3 normal = cross(edge, t_orientation_heading);
                normal = normalise(normal);
                normals[i] = normal;
        }
        normals[v] = -t_orientation_heading;
      	normals[v+1] = t_orientation_heading;

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

        m->push_vertex_data(vertex_data, number_of_vertex_rows, indices, number_of_indices);
        free(positions);
        free(normals);
        free(indices);
        free(vertex_data);
}

void GLWidget::render(render_object object)
{	
	glBindBuffer(GL_ARRAY_BUFFER, object.vertex_buffer);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 6*sizeof(float), (void*)0);
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, 6*sizeof(float), (void*)(3*sizeof(float)));
	glDrawElements(GL_TRIANGLES, object.number_of_indices, GL_UNSIGNED_INT, NULL);
}

void GLWidget::set_material(material* m)
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, m->ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m->diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, m->specular);
	glMaterialf(GL_FRONT, GL_SHININESS, m->shininess);
}

void GLWidget::render_scene(int view_width, int view_height)
{
	glViewport(0.0, 0.0, view_width, view_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(0.0f, 2.0f, 1.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glColor3f(1.0f, 0.0f, 0.0f);

	mesh cylinder_mesh = create_mesh(1024, 1024);
	push_cylinder_to_mesh(&cylinder_mesh, 0.5, 1.0, 16);

	render_object cylinder = buffer_mesh(&cylinder_mesh);
	
	destroy_mesh(cylinder_mesh);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat light_pos[] = {0.0, 1.0, 4.0};
	material wood_material = 
	{
		{0.115f, 0.0845f, 0.0815f},
		{0.251f, 0.196f, 0.086f},
		{0.0, 0.0f, 0.0f},
		2.0f
	};
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0);
	set_material(&wood_material);
	render(cylinder);
	
	glPopMatrix();
	glFlush();
}

class AppWindow: public QWidget
{
public:
	AppWindow(QWidget* parent): QWidget(parent)
	{
		layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	}

	void init_GLwidget()
	{
		gl_widget = new GLWidget(this);
		layout->addWidget(gl_widget);
	}

	~AppWindow()
	{
		delete layout;
	}
private:
	GLWidget* gl_widget;
	QBoxLayout* layout;
};

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	AppWindow* window = new AppWindow(NULL);
	window->resize(512, 512);
	window->show();
	window->init_GLwidget();
	app.exec();
	delete window;
	return 0;
}
