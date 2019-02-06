#include "RenderWidget.h"

GLWidget::GLWidget(QWidget* parent): QGLWidget(parent)
{
	//NOTE: If there is a bug in the rendering, make sure enough memory is allocated
	tree.branch_mesh = create_mesh(1024*4096, 1024*4096);
	tree.leaf_mesh = create_mesh(16*4096, 16*4096);
	tree.fruit_mesh = create_mesh(16*4096, 16*4096);
	camera_position = vec3{0.0f, 2.5f, 6.0f};
	camera_target = vec3{0.0f, 2.5f, 0.0f};
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
	glClearColor(0.1, 0.1, 0.1, 0.0);
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

	if(tree.branch_mesh.vertex_data_length > 0)
	{
		branches = buffer_mesh(&tree.branch_mesh);
	}
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
	gluLookAt(	camera_position.x, camera_position.y, camera_position.z, 
			camera_target.x, camera_target.y, camera_target.z,
			0.0f, 1.0f, 0.0f);
	glColor3f(1.0f, 0.0f, 0.0f);

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
		//TODO
	}
	glPopMatrix();
	glFlush();
}

void GLWidget::move_camera_up()
{
	camera_position.y += 1.5;
	update();
}

void GLWidget::move_camera_down()
{
	camera_position.y -= 1.5;
	update();
}

void GLWidget::move_camera_left()
{
	camera_position = rotate_about_axis(camera_position, vec3{0.0f, -1.0f, 0.0f}, 22.5f);
	update();
}

void GLWidget::move_camera_right()
{
	camera_position = rotate_about_axis(camera_position, vec3{0.0f, 1.0f, 0.0f}, 22.5f);
	update();
}

void GLWidget::move_camera_target_up()
{
	camera_target.y += 1.5f;
	update();
}

void GLWidget::move_camera_target_down()
{
	camera_target.y -= 1.5f;
	update();
}

void GLWidget::move_camera_forwards()
{
	vec3 forwards = normalise(camera_target - camera_position);
	camera_position = camera_position + 2.0f*forwards;
	update();
}

void GLWidget::move_camera_backwards()
{
	vec3 backwards = normalise(camera_position - camera_target);
	camera_position = camera_position + 2.0f*backwards;
	update();
}
