#include "ChartWidget.h"

ChartGLWidget::ChartGLWidget(QWidget* parent): QGLWidget(parent)
{
	points = (point*)malloc(sizeof(point)*MAX_TREE_COUNT);
}

ChartGLWidget::~ChartGLWidget()
{
	if(texture_data) free(texture_data);
	if(points) free(points);
}

void ChartGLWidget::buffer_circle_texture()
{
	texture_data = (GLubyte*)malloc(sizeof(GLubyte)*CT_WIDTH*CT_WIDTH*4);
	for(int i = 0; i < CT_WIDTH; i++)
	{//For each row of pixels
		for(int j = 0; j < CT_WIDTH; j++)
		{//For each pixel
			int dist = sqrt((i-(CT_WIDTH/2))*(i-(CT_WIDTH/2)) + (j-(CT_WIDTH/2))*(j-(CT_WIDTH/2)));
			for(int k = 0; k < 4; k++)
			{
				if(dist <= CT_WIDTH/2)
				texture_data[i*CT_WIDTH*4+j*4+k] = 255;
				else texture_data[i*CT_WIDTH*4+j*4+k] = 0;
			}
		}
	}
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture_buffer);
	glBindTexture(GL_TEXTURE_2D, texture_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CT_WIDTH, CT_WIDTH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ChartGLWidget::initializeGL()
{
	initializeGLFunctions();
	buffer_circle_texture();
	glClearColor(0.9, 0.9, 0.9, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);
	emit initialised();
}

void ChartGLWidget::set_forest_bounds(float width, float height)
{
	forest_width = width;
	forest_height = height;
}

void ChartGLWidget::resizeGL(int w, int h)
{
	view_width = w;
	view_height = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, forest_width, 0.0f, forest_height, -1.0f, 1.0f);
}

void ChartGLWidget::clear_points()
{
	number_of_points = 0;
}

void ChartGLWidget::push_point(float x, float y, float r, int c, int s)
{
	point* p = points + number_of_points;
	p->x = x;
	p->y = y;
	p->r = r;
	p->c = c;
	p->s = s;
	number_of_points++;
}

void ChartGLWidget::render_circle(point* p)
{	
	vec3 colour; 
	if(p->c == 0) 
	{
		colour = {1.0f, 0.0f, 0.2f};
	}
	else
	{
		switch(p->s)
		{
			case 0:
				colour = {0.004f,0.475f,0.435f};
				break;
			case 1:
				colour = {0.508f,0.8f,0.13f};
				break;
			case 2:
				colour = {1.0f,0.412f,0.706f};
				break;
		}
	}
	glPushMatrix();
	glTranslatef(p->x, p->y, 0.0f);
	glBindTexture(GL_TEXTURE_2D, texture_buffer);
	glColor3f(colour.x, colour.y, colour.z);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-p->r, -p->r, 0.0f); //Bottom left
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(p->r, -p->r, 0.0f); //Bottom right
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(p->r, p->r, 0.0f); //Top right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-p->r, p->r, 0.0f); //Top left
	glEnd();
	glPopMatrix();
}

void ChartGLWidget::paintGL()
{
	glViewport(0.0f,0.0f,view_width, view_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glAlphaFunc(GL_LESS, 0.2f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	for(int i = 0; i < number_of_points; i++)
	{
		render_circle(points+i);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glFlush();
}
