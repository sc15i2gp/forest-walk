#include <stdio.h>
#include <QApplication>
#include <QBoxLayout>
#include <QGLWidget>
#include <QGLFunctions>
#include <GL/glu.h>
#include "maths.h"
#include "string_functions.h"
#include "mesh.h"
#include "turtle.h"
#include "l_system.h"

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

	void render_scene(int,int);
	void render(render_object);
	render_object buffer_mesh(mesh*);
	void set_material(material*);
private:
	int view_width = 0;
	int view_height = 0;
};

render_object GLWidget::buffer_mesh(mesh* m)
{
	render_object buffered_mesh;
	glGenBuffers(1, &buffered_mesh.vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffered_mesh.vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, 2*m->vertex_data_length*sizeof(vec3), m->vertex_data, GL_STATIC_DRAW);
	glGenBuffers(1, &buffered_mesh.index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffered_mesh.index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->number_of_indices*sizeof(GLuint), m->indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	buffered_mesh.number_of_indices = m->number_of_indices;
	return buffered_mesh;
}

void GLWidget::render(render_object object)
{
	glBindBuffer(GL_ARRAY_BUFFER, object.vertex_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.index_buffer);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 6*sizeof(float), (void*)0);
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, 6*sizeof(float), (void*)(3*sizeof(float)));
	glDrawElements(GL_TRIANGLES, object.number_of_indices, GL_UNSIGNED_INT, NULL);
}

void GLWidget::set_material(material* m)
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, m->ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m->diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, m->specular);
	glMaterialf(GL_FRONT, GL_SHININESS, m->shininess);
}

void GLWidget::render_scene(int view_width, int view_height)
{
	glViewport(0.0, 0.0, view_width, view_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(0.0f, 2.5f, 3.0f, 0.0f, 1.5f, 0.0f, 0.0f, 1.0f, 0.0f);
	glColor3f(1.0f, 0.0f, 0.0f);

	l_system l;
	l.max_l_context_size = 0;
	l.max_r_context_size = 0;
	add_production(&l, "<A>", "+(30)FB");
	add_production(&l, "<B>", "-(60)FC");
	add_production(&l, "<C>", "+(60)FB");
	print_l_system(&l, "Test");

	char s[256];
	strcpy(s, "FA");

	for(int i = 0; i < 2; i++) derive_str(&l, s);
	tree_mesh_group tree;
	tree.leaf_mesh = create_mesh(2048, 2048);
	tree.branch_mesh = create_mesh(2048, 2048);
	tree.fruit_mesh = create_mesh(1024, 1024);
	run_turtle(s, &tree);
	render_object branch = buffer_mesh(&tree.branch_mesh);
	render_object leaf = buffer_mesh(&tree.leaf_mesh);
	
	destroy_mesh(tree.branch_mesh);
	destroy_mesh(tree.leaf_mesh);
	destroy_mesh(tree.fruit_mesh);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat light_pos[] = {0.0, 1.0, 4.0};
	material wood_material = 
	{
		{0.115f, 0.0845f, 0.0815f},
		{0.251f, 0.196f, 0.086f},
		{0.0, 0.0f, 0.0f},
		2.0f
	};
	material leaf_material =
	{
		{0.004f, 0.196f, 0.125f},
		{0.227f, 0.373f, 0.043f},
		{0.0f, 0.01f, 0.0f},
		2.0f
	};
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0);
	set_material(&wood_material);
	render(branch);
	set_material(&leaf_material);
	render(leaf);
	glPopMatrix();
	glFlush();
}

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
