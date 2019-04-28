#pragma once
#include <QObject>
#include <QWidget>
#include <QGLFunctions>
#include <GL/glu.h>
#include <QMouseEvent>
#include "../Core/Ball.h"
#include "../Core/turtle.h"
#include "../Core/mesh.h"
#include "../Core/measure.h"

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
	void mousePressEvent(QMouseEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);
	void wheelEvent(QWheelEvent* e);

public slots:
	void capture();

signals:
	void initialised();

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	void render_scene();
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
	BallData tree_ball_data;
	BallData light_ball_data;
	int last_pressed_mouse_button = -1;
	float last_x = 0.0f;
	float last_y = 0.0f;
	float translate_x = 0.0f;
	float translate_y = 0.0f;
	float translate_z = 0.0f;
};

