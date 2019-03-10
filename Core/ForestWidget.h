#pragma once
#include <QWidget>
#include <QGLWidget>
#include <QGLFunctions>
#include <QObject>
#include <QMouseEvent>
#include <GL/glu.h>
#include "maths.h"
#include "Ball.h"
#include "mesh.h"
#include "l_system.h"
#include "turtle.h"
#include "species_info.h"

#define CT_WIDTH 512
#define MAX_TREE_COUNT 8192

struct render_object
{
	GLuint vertex_buffer;
	GLuint index_buffer;
	int number_of_indices;
};

struct tree_buffer_object
{
	render_object branch_obj;
	render_object leaf_obj;
	render_object fruit_obj;
};

struct material
{
	GLfloat ambient[3];
	GLfloat diffuse[3];
	GLfloat specular[3];
	GLfloat shininess;
};

struct point
{
	float x;
	float y;
	float r; //Radius
	int c; //Dominated
	int s; //Species
	int age; //Age in plastochrons at plant level
	long int seed;
};

class ForestGLWidget: public QGLWidget, protected QGLFunctions
{
	Q_OBJECT
public:
	ForestGLWidget(QWidget* parent);
	~ForestGLWidget();

	void render_chart();
	void render_forest();
	void render_circle(point*);
	void push_point(float,float,float,int,int,int, long int);
	void clear_points();
	void set_forest_bounds(float width, float height);
	void mousePressEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void wheelEvent(QWheelEvent*);

public slots:
	void set_chart_mode();
	void set_forest_mode();
signals:
	void initialised();

protected:
	void set_projection_matrix();
	void set_material(material*);
	void initializeGL();
	void buffer_circle_texture();
	void load_platform();
	void resizeGL(int w, int h);
	void paintGL();
	void render(render_object obj);
	render_object buffer_mesh(mesh* m);
	void set_leaf_material(int);
	void set_branch_material(int);
	void set_fruit_material(int);
	void render(tree_buffer_object,int);
	tree_buffer_object buffer_tree_mesh_group(tree_mesh_group*);
	void clear_tree_model(tree_mesh_group*);
	void clear_buffers(render_object);
	void clear_buffers(tree_buffer_object);
	void generate_tree_model(char*,tree_mesh_group*);
	void generate_tree_str(point* p, char*);

	float forest_width = 0.0f;
	float forest_height = 0.0f;
	int view_width = 0;
	int view_height = 0;
	bool chart_mode = true;
	
	//Chart data
	GLubyte* texture_data = 0;
	GLuint texture_buffer = 0;
	point* points;
	int number_of_points = 0;
	
	//Forest data
	render_object platform = {};
	tree_buffer_object tree_obj = {};
	BallData forest_ball_data;
	BallData light_ball_data;
	tree_mesh_group tree_model_buffer = {};

	int last_pressed_mouse_button = -1;
	float last_x = 0.0f;
	float last_y = 0.0f;
	float translate_x = 0.0f;
	float translate_y = 0.0f;
	float translate_z = 0.0f;

	char axiom_pine[64] = {};
	char axiom_birch[64] = {};
	char axiom_rowan[64] = {};
	l_system ls_pine;
	l_system ls_birch;
	l_system ls_rowan;

	char* tree_str_buffer = NULL;
};
