#include "ForestWidget.h"

ForestGLWidget::ForestGLWidget(QWidget* parent): QGLWidget(parent)
{
	points = (point*)malloc(sizeof(point)*MAX_TREE_COUNT);
}

ForestGLWidget::~ForestGLWidget()
{
	if(texture_data) free(texture_data);
	if(points) free(points);
}

void ForestGLWidget::buffer_circle_texture()
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

void ForestGLWidget::initializeGL()
{
	initializeGLFunctions();
	buffer_circle_texture();
	set_projection_matrix();
	glClearColor(0.9, 0.9, 0.9, 1.0);
	glEnable(GL_DEPTH_TEST);
	emit initialised();
}

void ForestGLWidget::set_forest_bounds(float width, float height)
{
	forest_width = width;
	forest_height = height;
}

void ForestGLWidget::set_projection_matrix()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(chart_mode)
	{
		glOrtho(0.0f, forest_width, 0.0f, forest_height, -1.0f, 1.0f);
	}
	else 
	{
		gluPerspective(80.0f, (float)view_width/(float)view_height, 0.1f, 100.0f);
	}
}

void ForestGLWidget::resizeGL(int w, int h)
{
	view_width = w;
	view_height = h;
	set_projection_matrix();
}

void ForestGLWidget::clear_points()
{
	number_of_points = 0;
}

void ForestGLWidget::push_point(float x, float y, float r, int c, int s)
{
	point* p = points + number_of_points;
	p->x = x;
	p->y = y;
	p->r = r;
	p->c = c;
	p->s = s;
	number_of_points++;
}

void ForestGLWidget::render_circle(point* p)
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

void ForestGLWidget::set_chart_mode()
{
	chart_mode = true;
	glClearColor(0.9, 0.9, 0.9, 1.0);
	set_projection_matrix();
	parentWidget()->update();
	update();
}

void ForestGLWidget::set_forest_mode()
{
	chart_mode = false;
	glClearColor(0.494f, 0.753f, 0.933f, 0.0f);
	set_projection_matrix();
	parentWidget()->update();
	update();
}

void ForestGLWidget::render_chart()
{
	//Chart mode only
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
	//Chart mode only
}

void copy_float_vector(float* src, float* dest, int size = 3)
{
	for(int i = 0; i < size; i++) dest[i] = src[i];
}

void copy_face_data(float* vertex_positions_src, float* face_normal_src, int* face_vertex_indices, float* vertex_data_dest)
{
	for(int i = 0; i < 4; i++)
	{
		//Position
		float* src_location = vertex_positions_src + face_vertex_indices[i]*3;
		float* dest_location = vertex_data_dest + i*6;
		copy_float_vector(src_location, dest_location);

		//Normal
		float* src_normal_location = face_normal_src;
		float* dest_normal_location = vertex_data_dest + i*6 + 3;
		copy_float_vector(src_normal_location, dest_normal_location);

	}
}

void ForestGLWidget::load_platform()
{
	float w = forest_width;
	float h = 0.1f;
	float d = forest_height;
	w = w/2.0f;
	h = h/2.0f;
	d = d/2.0f;
	
	//Set up generic cuboid vertex data sources
	float positions[] = 
	{
		-w, -h, -d, //Left down back
		w, -h, -d, //Right down back
		w, h, -d, //Right up back
		-w, h, -d, //Left up back
		-w, -h, d, //Left down front
		w, -h, d, //Right down front
		w, h, d, //Right up front
		-w, h, d //Left up front
	};
	
	int face_position_indices[] = 
	{
		0, 4, 7, 3, //Left face
		5, 1, 2, 6, //Right face
		4, 5, 6, 7, //Front face
		1, 0, 3, 2, //Back face
		2, 3, 7, 6, //Up face
		1, 0, 4, 5 //Down face
	};

	float face_normals[] =
	{
		-1.0, 0.0, 0.0, //Left face
		1.0, 0.0, 0.0, //Right face
		0.0, 0.0, 1.0, //Front face
		0.0, 0.0, -1.0, //Back face
		0.0, 1.0, 0.0, //Up face
		0.0, -1.0, 0.0 //Down face
	};
	
	vec3 _vertex_data[48];
	float* vertex_data = (float*)_vertex_data;

	for(int face = 0; face < 6; face++)
	{
		float* face_normal = face_normals + 3*face;
		int* face_indices = face_position_indices + 4*face;
		float* dest_vertex_data = vertex_data + face*4*6;
		copy_face_data(positions, face_normal, face_indices, dest_vertex_data);
	}

	GLuint rect_indices[] = {0, 1, 2, 2, 3, 0};
	GLuint final_indices[36];
	
	//Create gl index array for cuboid
	for(int i = 0; i < 6; i++) for(int j = 0; j < 6; j++) final_indices[i*6 + j] = rect_indices[j] + 4*i;
	//Final vertex data destination
	mesh plat = create_mesh(144, 36);
	plat.push_vertex_data(_vertex_data, 24, final_indices, 36);
	glGenBuffers(1, &platform);
	glBindBuffer(GL_ARRAY_BUFFER, platform);
	glBufferData(GL_ARRAY_BUFFER, 2*plat.vertex_data_length*sizeof(vec3), plat.vertex_data, GL_STATIC_DRAW);
	glGenBuffers(1, &platform_index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, platform_index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, plat.number_of_indices*sizeof(GLuint), plat.indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	destroy_mesh(plat);
}

void ForestGLWidget::render_forest()
{
	if(platform == 0) load_platform();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(0.0f, 1.0f, 0.0f);
	glBindBuffer(GL_ARRAY_BUFFER, platform);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, platform_index_buffer);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 6*sizeof(float), (void*)0);
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, 6*sizeof(float), (void*)(3*sizeof(float)));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glPopMatrix();
}

void ForestGLWidget::paintGL()
{
	glViewport(0.0f,0.0f,view_width, view_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(chart_mode) render_chart();
	else render_forest();	
	glFlush();
}
