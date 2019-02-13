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

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	int view_width = 0;
	int view_height = 0;
	GLubyte* texture_data;
};
