#pragma once
#include <QWidget>
#include <QGLWidget>
#include <QGLFunctions>
#include <QObject>
#include <GL/glu.h>
#include "maths.h"
#include "mesh.h"

#define CT_WIDTH 512
#define MAX_TREE_COUNT 8192

struct point
{
	float x;
	float y;
	float r; //Radius
	int c; //Dominated
	int s; //Species
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
	void push_point(float,float,float,int,int);
	void clear_points();
	void set_forest_bounds(float width, float height);

public slots:
	void set_chart_mode();
	void set_forest_mode();
signals:
	void initialised();

protected:
	void set_projection_matrix();
	void initializeGL();
	void buffer_circle_texture();
	void load_platform();
	void resizeGL(int w, int h);
	void paintGL();
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
	GLuint platform = 0;
	GLuint platform_index_buffer = 0;
};
