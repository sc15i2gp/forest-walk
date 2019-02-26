#include "RenderWidget.h"

GLWidget::GLWidget(QWidget* parent): QGLWidget(parent)
{
	//NOTE: If there is a bug in the rendering, make sure enough memory is allocated
	tree.branch_mesh = create_mesh(2048*4096, 2048*4096);
	tree.leaf_mesh = create_mesh(2048*4096, 2048*4096);
	tree.fruit_mesh = create_mesh(2048*4096, 2048*4096);
	camera_position = vec3{0.0f, 2.5f, 6.0f};
	camera_target = vec3{0.0f, 2.5f, 0.0f};
	//Ball init
	Ball_Init(&tree_ball_data);
	Ball_Init(&light_ball_data);
	//Ball place
	Ball_Place(&tree_ball_data, qOne, 1.0f);
	Ball_Place(&light_ball_data, qOne, 1.0f);
}

GLWidget::~GLWidget()
{
	destroy_mesh(tree.branch_mesh);
	destroy_mesh(tree.leaf_mesh);
	destroy_mesh(tree.fruit_mesh);
}

void GLWidget::initializeGL()
{
	initializeGLFunctions();
	glClearColor(0.494f, 0.753f, 0.933f, 0.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);
	emit initialised();
}

void GLWidget::resizeGL(int w, int h)
{
	view_width = w;
	view_height = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80.0f, (float)view_width/(float)view_height, 0.1f, 100.0f);
}

void GLWidget::paintGL()
{
	render_scene(view_width, view_height);
}

void GLWidget::clear_buffers(render_object r)
{
	glDeleteBuffers(1, &r.vertex_buffer);
	glDeleteBuffers(1, &r.index_buffer);
	r.number_of_indices = 0;
}

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

void GLWidget::load_tree_model(char* tree_string)
{
	if(branches.vertex_buffer) clear_buffers(branches);
	if(leaves.vertex_buffer) clear_buffers(leaves);
	if(fruit.vertex_buffer) clear_buffers(fruit);

	tree.branch_mesh.vertex_data_length = 0;
	tree.branch_mesh.number_of_indices = 0;
	tree.leaf_mesh.vertex_data_length = 0;
	tree.leaf_mesh.number_of_indices = 0;
	tree.fruit_mesh.vertex_data_length = 0;
	tree.fruit_mesh.number_of_indices = 0;

	run_turtle(tree_string, &tree);

	if(tree.branch_mesh.vertex_data_length > 0) branches = buffer_mesh(&tree.branch_mesh);
	if(tree.leaf_mesh.vertex_data_length > 0) leaves = buffer_mesh(&tree.leaf_mesh);
	if(tree.fruit_mesh.vertex_data_length > 0) fruit = buffer_mesh(&tree.fruit_mesh);
}

void GLWidget::render_scene(int view_width, int view_height)
{
	glViewport(0.0, 0.0, view_width, view_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat light_pos[] = {0.0, 0.0, 1.0, 0.0};
	material wood_material = 
	{
		{0.420f, 0.355f, 0.159f},
		{0.471f, 0.325f, 0.247f},
		{0.0f, 0.0f, 0.0f},
		2.0f
	};
	material leaf_material =
	{
		{0.004f, 0.196f, 0.125f},
		{0.227f, 0.373f, 0.043f},
		{0.0f, 0.01f, 0.0f},
		2.0f
	};
	material fruit_material =
	{
		{0.2f, 0.027f, 0.0f},
		{1.0f, 0.141f, 0.0f},
		{1.0f, 0.914f, 0.898f},
		16.0f
	};

	GLfloat current_ball_value[16];
	Ball_Value(&light_ball_data, current_ball_value);
	glLoadIdentity();
	glMultMatrixf(current_ball_value);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0);
	
	glLoadIdentity();
	glTranslatef(0.0f, -0.2f, -1.0f);
	glTranslatef(translate_x,translate_y,translate_z);
	//ball value
	Ball_Value(&tree_ball_data, current_ball_value);
	//glMultMatrixf
	glMultMatrixf(current_ball_value);
	if(branches.vertex_buffer)
	{
		set_material(&wood_material);
		render(branches);
	}
	if(leaves.vertex_buffer)
	{
		set_material(&leaf_material);
		render(leaves);
	}
	if(fruit.vertex_buffer)
	{
		set_material(&fruit_material);
		render(fruit);
	}
	glPopMatrix();
	glFlush();
}

void GLWidget::mousePressEvent(QMouseEvent* e)
{
	last_pressed_mouse_button = e->button();
	float s_x = width();
	float s_y = height();
	HVect ball_vec;
	ball_vec.x = (2.0f*e->x() - s_x)/s_x;
	ball_vec.y = (s_y - 2.0f*e->y())/s_y;
	//tree_ball_vec.y = (2.0f*e->y() - s)/s;
	switch(last_pressed_mouse_button)
	{
		case Qt::LeftButton:
			Ball_Mouse(&tree_ball_data, ball_vec);
			Ball_BeginDrag(&tree_ball_data);
			parentWidget()->update();
			update();
			break;
		case Qt::RightButton:
			last_x = ball_vec.x;
			last_y = ball_vec.y;
			parentWidget()->update();
			update();
			break;
		case Qt::MiddleButton:
			Ball_Mouse(&light_ball_data, ball_vec);
			Ball_BeginDrag(&light_ball_data);
			parentWidget()->update();
			update();
			break;
	}
}

void GLWidget::mouseReleaseEvent(QMouseEvent* e)
{
	switch(last_pressed_mouse_button)
	{
		case Qt::LeftButton:
			Ball_EndDrag(&tree_ball_data);
			parentWidget()->update();
			update();
			break;
		case Qt::RightButton:
			parentWidget()->update();
			update();
			break;
		case Qt::MiddleButton:
			Ball_EndDrag(&light_ball_data);
			parentWidget()->update();
			update();
	}
}

void GLWidget::mouseMoveEvent(QMouseEvent* e)
{
	float s_x = width();
	float s_y = height();
	HVect ball_vec;
	ball_vec.x = (2.0f*e->x() - s_x)/s_x;
	ball_vec.y = (s_y - 2.0f*e->y())/s_y;
	switch(last_pressed_mouse_button)
	{
		case Qt::LeftButton:
			Ball_Mouse(&tree_ball_data, ball_vec);
			Ball_Update(&tree_ball_data);
			parentWidget()->update();
			update();
			break;
		case Qt::RightButton:
			translate_x += ball_vec.x - last_x;
			translate_y += ball_vec.y - last_y;
			last_x = ball_vec.x;
			last_y = ball_vec.y;
			parentWidget()->update();
			update();
			break;
		case Qt::MiddleButton:
			Ball_Mouse(&light_ball_data, ball_vec);
			Ball_Update(&light_ball_data);
			parentWidget()->update();
			update();
			break;
	}
}

void GLWidget::wheelEvent(QWheelEvent* e)
{
	translate_z += 0.5f*(e->angleDelta().y()/120);
	parentWidget()->update();
	update();
}
