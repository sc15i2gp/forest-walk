#include <stdio.h>
#include <QApplication>
#include <QBoxLayout>
#include <QGLFunctions>
#include <GL/glu.h>

void render_scene(int view_width, int view_height)
{
	glViewport(0.0, 0.0, view_width, view_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glColor3f(1.0f, 0.0f, 0.0f);

	glBegin(GL_POLYGON);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glEnd();

	glFlush();
}

class GLWidget: public QGLWidget, protected QGLFunctions
{
public:
	GLWidget(QWidget* parent): QGLWidget(parent){}

protected:
	void initializeGL()
	{
		initializeGLFunctions();
		glClearColor(0.1, 0.1, 0.1, 0.0);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glEnable(GL_DEPTH_TEST);
	}

	void resizeGL(int w, int h)
	{
		view_width = w;
		view_height = h;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(80.0f, (float)view_width/(float)view_height, 0.1f, 100.0f);
	}

	void paintGL()
	{
		render_scene(view_width, view_height);
	}
private:
	int view_width = 0;
	int view_height = 0;
};

class AppWindow: public QWidget
{
public:
	AppWindow(QWidget* parent): QWidget(parent)
	{
		layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	}

	void init_GLwidget()
	{
		gl_widget = new GLWidget(this);
		layout->addWidget(gl_widget);
	}

	~AppWindow()
	{
		delete layout;
	}
private:
	GLWidget* gl_widget;
	QBoxLayout* layout;
};

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	AppWindow* window = new AppWindow(NULL);
	window->resize(512, 512);
	window->show();
	window->init_GLwidget();
	app.exec();
	delete window;
	return 0;
}
