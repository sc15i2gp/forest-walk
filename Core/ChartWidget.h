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
};

class ChartGLWidget: public QGLWidget, protected QGLFunctions
{
	Q_OBJECT
public:
	ChartGLWidget(QWidget* parent);
	~ChartGLWidget();

	void render_circle(point*,vec3 colour);
	void push_position_and_radius(float,float,float);
	void clear_positions_and_radii();
signals:
	void initialised();

protected:
	void initializeGL();
	void buffer_circle_texture();
	void resizeGL(int w, int h);
	void paintGL();
	int view_width = 0;
	int view_height = 0;
	GLubyte* texture_data = 0;
	GLuint texture_buffer = 0;
	point* points;
	int number_of_points = 0;
};
