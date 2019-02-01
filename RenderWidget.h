#pragma once
#include <QWidget>
#include <QGLFunctions>
#include <GL/glu.h>
#include "turtle.h"
#include "mesh.h"

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

class GLWidget: public QGLWidget, protected QGLFunctions
{
public:
	GLWidget(QWidget* parent);
	~GLWidget();
	
	void load_tree_model(char*);

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	void render_scene(int,int);
	void render(render_object);
	render_object buffer_mesh(mesh*);
	void clear_buffers(render_object);
	void set_material(material*);
private:
	tree_mesh_group tree = {};
	render_object branches {};
	render_object leaves = {};
	render_object fruit = {};
	int view_width = 0;
	int view_height = 0;
};

