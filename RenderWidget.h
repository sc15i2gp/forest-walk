#pragma once
#include <QObject>
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
	Q_OBJECT
public:
	GLWidget(QWidget* parent);
	~GLWidget();
	
	void load_tree_model(char*);

public slots:	
	void move_camera_up();
	void move_camera_down();
	void move_camera_left();
	void move_camera_right();
	
	void move_camera_target_up();
	void move_camera_target_down();
	void move_camera_forwards();
	void move_camera_backwards();

signals:
	void initialised();

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
	vec3 camera_position;
	vec3 camera_target;
};

