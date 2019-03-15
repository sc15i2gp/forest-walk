#include "ForestWidget.h"

ForestGLWidget::ForestGLWidget(QWidget* parent): QGLWidget(parent)
{
	points = (point*)malloc(sizeof(point)*MAX_TREE_COUNT);
	tree_str_buffer = (char*)malloc(MAX_DERIVED_OUTPUT_SIZE);
	
	tree_model_buffer.branch_mesh = create_mesh(2048*4096, 2048*4096);
	tree_model_buffer.leaf_mesh = create_mesh(2048*4096, 2048*4096);
	tree_model_buffer.fruit_mesh = create_mesh(2048*4096, 2048*4096);
	
	Ball_Init(&forest_ball_data);
	Ball_Init(&light_ball_data);
	Ball_Place(&forest_ball_data, qOne, 1.0f);
	Ball_Place(&light_ball_data, qOne, 1.0f);
	load_l_system(&ls_pine, "../l-systems_for_work/pine_with_leaves.lsys", axiom_pine);
	load_l_system(&ls_birch, "../l-systems_for_work/birch_with_leaves.lsys", axiom_birch);
	load_l_system(&ls_rowan, "../l-systems_for_work/rowan_with_leaves_and_fruit.lsys", axiom_rowan);
	print_l_system(&ls_pine, "Pine");
	print_l_system(&ls_birch, "Birch");
	print_l_system(&ls_rowan, "Rowan");
}

ForestGLWidget::~ForestGLWidget()
{
	if(texture_data) free(texture_data);
	if(points) free(points);
	free(tree_str_buffer);
}

void ForestGLWidget::set_tree_grid(tree_grid* t)
{
	t_grid = t;
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
		gluPerspective(80.0f, (float)view_width/(float)view_height, 0.1f, 400.0f);
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

int ForestGLWidget::push_point(float x, float y, float r, int c, int s, int age, long int seed)
{
	point* p = points + number_of_points;
	p->x = x;
	p->y = y;
	p->r = r;
	p->c = c;
	p->s = s;
	p->age = age;
	p->seed = seed;
	return number_of_points++;
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
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
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
	platform = buffer_mesh(&plat);
	destroy_mesh(plat);
}

void ForestGLWidget::set_material(material* m)
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, m->ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m->diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, m->specular);
	glMaterialf(GL_FRONT, GL_SHININESS, m->shininess);
}

void ForestGLWidget::render(render_object obj)
{
	glBindBuffer(GL_ARRAY_BUFFER, obj.vertex_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.index_buffer);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 6*sizeof(float), (void*)0);
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, 6*sizeof(float), (void*)(3*sizeof(float)));
	glDrawElements(GL_TRIANGLES, obj.number_of_indices, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ForestGLWidget::set_branch_material(int species)
{
	material* wood_material = NULL;
        material pine_material =
	{
		{0.420f, 0.355f, 0.159f},
		{0.471f, 0.325f, 0.247f},
		{0.0f, 0.0f, 0.0f},
		2.0f
	};
	material birch_material =
	{
		{0.537f, 0.525f, 0.502f},
		{0.937f, 0.925f, 0.902f},
		{0.0f, 0.0f, 0.0f},
		2.0f
	};
	material rowan_material =
	{
		{0.2f, 0.2f, 0.2f},
		{0.667f, 0.698f, 0.627f},
		{0.0f, 0.0f, 0.0f},
		2.0f
	};
	switch(species)
	{
		case PINE:
			wood_material = &pine_material;
			break;
		case BIRCH:
			wood_material = &birch_material;
			break;
		case ROWAN:
			wood_material = &rowan_material;
			break;
	}
	set_material(wood_material);
}

void ForestGLWidget::set_leaf_material(int species)
{
	material leaf_material =
	{
		{0.004f, 0.196f, 0.125f},
		{0.227f, 0.373f, 0.043f},
		{0.0f, 0.01f, 0.0f},
		2.0f
	};
	set_material(&leaf_material);
}

void ForestGLWidget::set_fruit_material(int species)
{
	material fruit_material =
	{
		{0.2f, 0.027f, 0.0f},
		{1.0f, 0.141f, 0.0f},
		{1.0f, 0.914f, 0.898f},
		16.0f
	};
	set_material(&fruit_material);
}

void ForestGLWidget::render(tree_buffer_object obj, int species)
{
	TIMED(__func__);
	if(obj.branch_obj.vertex_buffer)
	{
		set_branch_material(species);
		render(obj.branch_obj);
	}
	if(obj.leaf_obj.vertex_buffer)
	{
		set_leaf_material(species);
		render(obj.leaf_obj);
	}
	if(obj.fruit_obj.vertex_buffer)
	{
		set_fruit_material(species);
		render(obj.fruit_obj);
	}
}

render_object ForestGLWidget::buffer_mesh(mesh* m)
{
	render_object obj = {};
	glGenBuffers(1, &obj.vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, obj.vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, 2*m->vertex_data_length*sizeof(vec3), m->vertex_data, GL_STATIC_DRAW);
	glGenBuffers(1, &obj.index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->number_of_indices*sizeof(GLuint), m->indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	obj.number_of_indices = m->number_of_indices;
	return obj;
}

tree_buffer_object ForestGLWidget::buffer_tree_mesh_group(tree_mesh_group* t)
{
	TIMED(__func__);
	tree_buffer_object obj = {};
	if(t->branch_mesh.vertex_data_length > 0) obj.branch_obj = buffer_mesh(&t->branch_mesh);
	if(t->leaf_mesh.vertex_data_length > 0) obj.leaf_obj = buffer_mesh(&t->leaf_mesh);
	if(t->fruit_mesh.vertex_data_length > 0) obj.fruit_obj = buffer_mesh(&t->fruit_mesh);
	return obj;
}

void ForestGLWidget::clear_buffers(render_object obj)
{
	if(obj.vertex_buffer) glDeleteBuffers(1, &obj.vertex_buffer);
	if(obj.index_buffer) glDeleteBuffers(1, &obj.index_buffer);
}

void ForestGLWidget::clear_buffers(tree_buffer_object obj)
{
	clear_buffers(obj.branch_obj);
	clear_buffers(obj.leaf_obj);
	clear_buffers(obj.fruit_obj);
}

void ForestGLWidget::clear_tree_model(tree_mesh_group* t)
{
	t->branch_mesh.vertex_data_length = 0;
	t->branch_mesh.number_of_indices = 0;
	t->leaf_mesh.vertex_data_length = 0;
	t->leaf_mesh.number_of_indices = 0;
	t->fruit_mesh.vertex_data_length = 0;
	t->fruit_mesh.number_of_indices = 0;
}

void ForestGLWidget::generate_tree_str(point* p, char* str_buffer)
{
	TIMED(__func__);
	l_system* l = NULL;
	char* axiom = NULL;
	int max_derive_count = 0;
	int derive_coefficient = 0;
	switch(p->s)
	{
		case PINE:
			l = &ls_pine;
			axiom = axiom_pine;
			max_derive_count = 20;
			derive_coefficient = 2;
			break;
		case BIRCH:
			l = &ls_birch;
			axiom = axiom_birch;
			max_derive_count = 20;
			derive_coefficient = 2;
			break;
		case ROWAN:
			l = &ls_rowan;
			axiom = axiom_rowan;
			max_derive_count = 10;
			derive_coefficient = 2;
			break;
	}
	strcpy(str_buffer, axiom);
	srand(p->seed);
	int age = derive_coefficient*p->age;
	int derive_count = (age < max_derive_count) ? age : max_derive_count;
	for(int i = 0; i < derive_count; i++) derive_str(l, str_buffer);
}

void ForestGLWidget::generate_tree_model(char* tree_str, tree_mesh_group* model)
{
	TIMED(__func__);
	run_turtle(tree_str, model);
}

//NOTE: From observing execution times, functions which take the longest during derivation are:
//	- l_system derive_str
//	- run_turtle
//	- render

//TODO: Reduce number of string derivations
//	- Have many points not too near each other share seeds

//TODO: Render only certain buckets

//TODO: Optimise rendering
//	- Reduce number of derivations which are required during rendering
//		- Only derive strings in certain buckets
//		- Save previously derived strings
//		- Share derived strings
//	- Reduce number of times tree models should be generated
//		- Cache previously generated models
//		- Only generate models in certain buckets
//	- Reduce number of draw calls
//		- Draw trees in certain buckets
//		- Fog

//TODO: Toggle domination on chart
//TODO: Toggle + show old age on chart
//TODO: Chart key

int ForestGLWidget::find_tree_buffer_object(tree_node* t)
{
	for(int i = 0; i < 4096; i++)
	{
		if(t == tree_nodes[i]) return i;
		else if(!tree_nodes[i])
		{
			tree_nodes[i] = t;
			return i;
		}
	}
}

void ForestGLWidget::clear_unused_model_buffers()
{
	for(int i = 0; i < 4096; i++)
	{
		if(tree_nodes[i] && !tree_nodes[i]->in_use)
		{
			clear_buffers(tree_models[i]);
			tree_nodes[i] = NULL;
		}
	}
}

int models_gen = 0;

void ForestGLWidget::render_bucket_of_trees(int x, int y)
{
	clear_unused_model_buffers();
	tree_node* t_node = t_grid->bucket(x,y);
	for(; t_node; t_node = t_node->next)
	{
		printf("------------------------\n");
		point* p = points+t_node->point_ref;
		int model_ref = find_tree_buffer_object(t_node);
		if(t_node->changed)
		{
			generate_tree_str(p, tree_str_buffer); //BIGGEST TIME WASTER
			clear_tree_model(&tree_model_buffer);
			generate_tree_model(tree_str_buffer, &tree_model_buffer); //NEXT BIGGEST
			clear_buffers(tree_models[model_ref]);
			tree_models[model_ref] = buffer_tree_mesh_group(&tree_model_buffer);
			t_node->changed = false;
			models_gen++;
		}
		glPushMatrix();
		glTranslatef(p->x, 0.0f, p->y);
		render(tree_models[model_ref], p->s);
		glPopMatrix();
	}
}

void ForestGLWidget::render_forest()
{
	TIMED(__func__);
	if(platform.vertex_buffer == 0) load_platform();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glPushMatrix();
	glLoadIdentity();
	GLfloat current_ball_value[16];
	GLfloat light_pos[] = {0.0f, 1.0f, 1.0f, 0.0f};
	Ball_Value(&light_ball_data, current_ball_value);
	glMultMatrixf(current_ball_value);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0f);

	glLoadIdentity();
	glTranslatef(0.0f, -0.2f, -1.0f);
	glTranslatef(translate_x, translate_y, translate_z);
	
	Ball_Value(&forest_ball_data, current_ball_value);
	glMultMatrixf(current_ball_value);
	
	material grass_material =
	{
		{0.004f, 0.196f, 0.125f},
		{0.227f, 0.373f, 0.043f},
		{0.5f, 0.51f, 0.5f},
		2.0f
	};
	set_material(&grass_material);
	render(platform);
	glTranslatef(-(forest_width/2.0f),0.0f,-(forest_height/2.0f));
	
	glEnable(GL_FOG);
	glFogf(GL_FOG_START, 0.0f);
	glFogf(GL_FOG_END, 100.0f);
	glFogf(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_DENSITY, 1.0f);
	GLfloat fog_colour[] = {0.494f, 0.753f, 0.933f, 1.0f};
	glFogfv(GL_FOG_COLOR, fog_colour);
	printf("------------------------\n");
	models_gen = 0;
	for(int i = 0; i < t_grid->height(); i++)
	{
		for(int j = 0; j < t_grid->width(); j++)
		{
			render_bucket_of_trees(j, i);
		}
	}
	printf("Generated %d tree models\n", models_gen);
	printf("------------------------\n");

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);
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

void ForestGLWidget::mousePressEvent(QMouseEvent* e)
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
			Ball_Mouse(&forest_ball_data, ball_vec);
			Ball_BeginDrag(&forest_ball_data);
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

void ForestGLWidget::mouseReleaseEvent(QMouseEvent* e)
{
	switch(last_pressed_mouse_button)
	{
		case Qt::LeftButton:
			Ball_EndDrag(&forest_ball_data);
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

void ForestGLWidget::mouseMoveEvent(QMouseEvent* e)
{
	float s_x = width();
	float s_y = height();
	HVect ball_vec;
	ball_vec.x = (2.0f*e->x() - s_x)/s_x;
	ball_vec.y = (s_y - 2.0f*e->y())/s_y;
	switch(last_pressed_mouse_button)
	{
		case Qt::LeftButton:
			Ball_Mouse(&forest_ball_data, ball_vec);
			Ball_Update(&forest_ball_data);
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

void ForestGLWidget::wheelEvent(QWheelEvent* e)
{
	translate_z += 0.5f*(e->angleDelta().y()/120);
	parentWidget()->update();
	update();
}
