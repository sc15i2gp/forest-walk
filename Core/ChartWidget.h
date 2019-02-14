#pragma once
#include <QWidget>
#include <QGLWidget>
#include <QGLFunctions>
#include <GL/glu.h>
#include "maths.h"

#define CT_WIDTH 512
class ChartGLWidget: public QGLWidget, protected QGLFunctions
{
public:
	ChartGLWidget(QWidget* parent);
	~ChartGLWidget();

	void render_circle(float, float, float, vec3 colour);
protected:
	void initializeGL();
	void buffer_circle_texture();
	void resizeGL(int w, int h);
	void paintGL();
	int view_width = 0;
	int view_height = 0;
	GLubyte* texture_data = 0;
	GLuint texture_buffer = 0;
};
