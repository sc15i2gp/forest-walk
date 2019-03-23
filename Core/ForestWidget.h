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
#include "timed.h"
#include "tree_grid.h"
#include "render_data.h"
#include "tree_model_gen.h"

#define CT_WIDTH 512
#define MAX_TREE_COUNT 8192

struct material
{
	GLfloat ambient[3];
	GLfloat diffuse[3];
	GLfloat specular[3];
	GLfloat shininess;
};

class ForestGLWidget: public QGLWidget, protected QGLFunctions
{
	Q_OBJECT
public:
	ForestGLWidget(QWidget* parent);
	~ForestGLWidget();

	void render_chart();
	void render_forest();
	void render_circle(float,float,float);
	void render_tree_point(tree_node*);
	void set_forest_bounds(float width, float height);
	void mousePressEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void wheelEvent(QWheelEvent*);
	void set_tree_grid(tree_grid*);

public slots:
	void set_chart_mode();
	void set_forest_mode();
	void set_view_dist(int);
signals:
	void initialised();

protected:
	//QT functions
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	
	void set_projection_matrix();
	
	//Material functions
	void set_material(material*);
	void set_leaf_material(int);
	void set_branch_material(int);
	void set_fruit_material(int);
	
	//Buffer functions
	void buffer_circle_texture();
	tree_buffer_object buffer_tree_mesh_group(tree_mesh_group*);
	void load_platform();
	void clear_buffers(render_object);
	void clear_buffers(tree_buffer_object);
	void clear_unused_model_buffers();

	//Render functions	
	void render(render_object obj);
	render_object buffer_mesh(mesh* m);
	void render(tree_buffer_object,int);
	void render_tree_models();
	int pick_buckets_to_render(int*,vec3,vec3,float);
	void render_grid();

	//Tree model functions
	void clear_tree_model(tree_mesh_group*);
	void generate_tree_model(tree_node*,int);
	void generate_tree_models(vec3);
	
	void push_bucket_of_trees_to_render_queue(int,int);

	//Presentation data
	float forest_width = 0.0f;
	float forest_height = 0.0f;
	int view_width = 0;
	int view_height = 0;
	bool chart_mode = true;
	
	//Texture data
	GLubyte* texture_data = 0;
	GLuint texture_buffer = 0;
	
	//Forest data
	render_object platform = {};
	tree_mesh_group tree_model_buffer = {};
	float view_dist = 30.0f;

	//Arcball data
	int last_pressed_mouse_button = -1;
	float last_x = 0.0f;
	float last_y = 0.0f;
	float translate_x = 0.0f;
	float translate_y = -2.0f;
	float translate_z = 0.0f;
	BallData forest_ball_data = {};
	BallData light_ball_data = {};

	tree_model_generator model_generator = {};

	tree_grid* t_grid = NULL;

	tree_model_map t_map = {}; //Maps tree_nodes in grid to buffered tree_models
	render_queue r_queue = {};
};
