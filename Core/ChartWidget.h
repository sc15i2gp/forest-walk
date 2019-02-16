#pragma once
#include <QWidget>
#include <QGLWidget>
#include <QGLFunctions>
#include <QObject>
#include <GL/glu.h>
#include "maths.h"

#define CT_WIDTH 512
#define MAX_TREE_COUNT 8192

struct point
{
	float x;
	float y;
	float r;
	int c;
};

class ChartGLWidget: public QGLWidget, protected QGLFunctions
{
	Q_OBJECT
public:
	ChartGLWidget(QWidget* parent);
	~ChartGLWidget();

	void render_circle(point*);
	void push_point(float,float,float,int);
	void clear_points();
	void set_forest_bounds(float width, float height);
signals:
	void initialised();

protected:
	void initializeGL();
	void buffer_circle_texture();
	void resizeGL(int w, int h);
	void paintGL();
	float forest_width = 0.0f;
	float forest_height = 0.0f;
	int view_width = 0;
	int view_height = 0;
	GLubyte* texture_data = 0;
	GLuint texture_buffer = 0;
	point* points;
	int number_of_points = 0;
};
