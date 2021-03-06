#include "ForestWidget.h"

ForestGLWidget::ForestGLWidget(QWidget* parent): QGLWidget(parent)
{
	model_generator = create_tree_model_generator();
	t_map = create_tree_model_map(4096,256);
	r_queue = create_render_queue(4096);

	tree_model_buffer.branch_mesh = create_mesh(2048*4096, 2048*4096);
	tree_model_buffer.leaf_mesh = create_mesh(2048*4096, 2048*4096);
	tree_model_buffer.fruit_mesh = create_mesh(2048*4096, 2048*4096);
	
	Ball_Init(&forest_ball_data);
	Ball_Init(&light_ball_data);
	Ball_Place(&forest_ball_data, qOne, 1.0f);
	Ball_Place(&light_ball_data, qOne, 1.0f);
}

ForestGLWidget::~ForestGLWidget()
{
	if(texture_data) free(texture_data);
	destroy_render_queue(r_queue);
	destroy_tree_model_map(t_map);
	destroy_tree_model_generator(model_generator);
}

void ForestGLWidget::set_view_dist(int d)
{
	view_dist = (float)d;
	updateGL();
}

void ForestGLWidget::set_show_domination(bool b)
{
	show_domination = b;
	updateGL();
}

void ForestGLWidget::set_show_old_age(bool b)
{
	show_senescence = b;
	updateGL();
}

void ForestGLWidget::set_show_view_range(bool b)
{
	show_view_range = b;
	updateGL();
}

void ForestGLWidget::set_show_grid(bool b)
{
	show_grid = b;
	updateGL();
}

void ForestGLWidget::set_apply_lod(bool b)
{
	apply_lod = b;
	updateGL();
}

void ForestGLWidget::set_tree_grid(tree_grid* t)
{
	t_grid = t;
}

void ForestGLWidget::buffer_circle_texture()
{
	//construct texture data
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

	//buffer texture data
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

void ForestGLWidget::render_tree_point(tree_node* t)
{	
	vec3 colour; 
	if(t->dominated == 0 && show_domination) 
	{
		colour = {1.0f, 0.0f, 0.2f};
	}
	else
	{
		switch(t->species)
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
	if(t->old_age && show_senescence) colour = colour - vec3{0.3f, 0.3f, 0.3f};
	glColor3f(colour.x, colour.y, colour.z);
	render_circle(t->_x, t->_y, t->_r);
}

void ForestGLWidget::render_circle(float x, float y, float r)
{
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glBindTexture(GL_TEXTURE_2D, texture_buffer);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-r, -r, 0.0f); //Bottom left
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(r, -r, 0.0f); //Bottom right
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(r, r, 0.0f); //Top right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-r, r, 0.0f); //Top left
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

//draws grid lines
void ForestGLWidget::render_grid()
{
	for(int i = 0; i <= t_grid->width(); i++)
	{
		glBegin(GL_LINES);
		if(i > 0)
		{
			glVertex3i(i*BUCKET_LENGTH, 0, 0);
			glVertex3i(i*BUCKET_LENGTH, t_grid->height()*BUCKET_LENGTH, 0);
		}
		else
		{
			glVertex3f(0.01f, 0.0f, 0.0f);
			glVertex3f(0.01f, (float)t_grid->height()*BUCKET_LENGTH, 0.0f);

		}
		glEnd();
	}
	for(int i = 0; i <= t_grid->height(); i++)
	{
		glBegin(GL_LINES);
		if(i > 0)
		{
			glVertex3i(0, i*BUCKET_LENGTH, 0);
			glVertex3i(t_grid->width()*BUCKET_LENGTH, i*BUCKET_LENGTH, 0);
		}
		else
		{
			glVertex3f(0.0f, 0.01f, 0.0f);
			glVertex3f((float)t_grid->width()*BUCKET_LENGTH, 0.01f, 0.0f);
		}
		glEnd();
	}
}

//Renders a rectangle showing the grid cells, the trees in which will be rendered
void ForestGLWidget::render_view_range()
{
	vec3 view_pos = {-translate_x, -translate_y, -translate_z};
	vec3 view_dir = {0.0f, 0.0f, -1.0f};
	
	//Get view position and direction
	GLfloat current_ball_value[16] = {};
	Ball_Value(&forest_ball_data, current_ball_value);
	transpose_matrix_4x4(current_ball_value);
	view_pos = matrix_multiply_4x4(view_pos, current_ball_value);
	view_dir = matrix_multiply_4x4(view_dir, current_ball_value);
	
	view_pos = view_pos + vec3{forest_width/2.0f,0.0f,forest_height/2.0f};
	view_dir.y = 0.0f;
	view_dir = normalise(view_dir);
	
	float rad = 45.0f * (PI/180.0f);
	float f = view_dist/cos(rad);
	vec3 v_1 = rotate_about_axis(f*view_dir, vec3{0.0f, 1.0f, 0.0f}, 45) + view_pos;
	vec3 v_2 = rotate_about_axis(f*view_dir, vec3{0.0f, -1.0f, 0.0f}, 45) + view_pos;
	vec3 v = view_pos;

	//Calculate bounds of view rectangle
	float min_x = min(min(v.x, v_1.x),v_2.x);
	float max_x = max(max(v.x, v_1.x), v_2.x);
	float min_y = min(min(v.z, v_1.z), v_2.z);
	float max_y = max(max(v.z, v_1.z), v_2.z);

	int _min_x = (int)(min_x/BUCKET_LENGTH) - 1;
	int _min_y = (int)(min_y/BUCKET_LENGTH) - 1;
	int _max_x = (int)(max_x/BUCKET_LENGTH) + 2;
	int _max_y = (int)(max_y/BUCKET_LENGTH) + 2;

	min_x = _min_x*BUCKET_LENGTH;
	min_y = _min_y*BUCKET_LENGTH;
	max_x = _max_x*BUCKET_LENGTH;
	max_y = _max_y*BUCKET_LENGTH;

	//draw circle representing view position
	render_circle(view_pos.x, view_pos.z, 3.0f);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	//draw view rectangle overlaying grid
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	
	glVertex3f(min_x, min_y, 0);
	glVertex3f(min_x, max_y, 0);
	
	glVertex3f(min_x, min_y, 0);
	glVertex3f(max_x, min_y, 0);

	glVertex3f(min_x, max_y, 0);
	glVertex3f(max_x, max_y, 0);

	glVertex3f(max_x, max_y, 0);
	glVertex3f(max_x, min_y, 0);

	vec3 view_end = view_pos+view_dist*view_dir;
	glVertex3f(view_pos.x, view_pos.z, 0.0f);
	glVertex3f(view_end.x, view_end.z, 0.0f);

	glVertex3f(view_pos.x, view_pos.z, 0.0f);
	glVertex3f(v_1.x, v_1.z, 0.0f);

	glVertex3f(view_pos.x, view_pos.z, 0.0f);
	glVertex3f(v_2.x, v_2.z, 0.0f);

	glEnd();
	glLineWidth(1.0f);
}

void ForestGLWidget::render_chart()
{
	glColor3f(0.0f,0.0f,0.0f);

	if(show_grid) render_grid();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	glAlphaFunc(GL_LESS, 0.2f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	//Render tree circles
	for(int i = 0; i < t_grid->height(); i++)
	{
		for(int j = 0; j < t_grid->width(); j++)
		{
			tree_node* t = t_grid->bucket(j,i);
			for(; t; t = t->next) render_tree_point(t);
		}
	}

	glColor3f(0.0f, 0.0f, 0.0f);
	
	if(show_view_range) render_view_range();
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
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

//Loads a cuboid platform on top of which is the forest
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

	//Construct mesh vertex data face by face
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

	//buffer vertex data
	glGenBuffers(1, &obj.vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, obj.vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, 2*m->vertex_data_length*sizeof(vec3), m->vertex_data, GL_STATIC_DRAW);

	//buffer index data
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

void ForestGLWidget::generate_tree_model(tree_node* t_node, int lod)
{
	clear_tree_model(&tree_model_buffer);

	int model_size = model_generator.generate_tree_model(&tree_model_buffer, lod);
	t_map.set_model(t_node, buffer_tree_mesh_group(&tree_model_buffer), model_size, lod);
}

void ForestGLWidget::clear_unused_model_buffers()
{
	for(int i = 0; i < t_map.number_of_tree_model_refs; i++)
	{//For each tree in the reference cache
		if(t_map.ref_in_use(i) && !t_map.tree_in_use(i))
		{//If the ref has a tree and that tree is no longer in use
			if(t_map.model_ref_count(i) == 1)
			{
				clear_buffers(t_map.find_model(i));
			}
			t_map.release_ref(i);
		}
	}
}

int models_gen = 0;

void ForestGLWidget::push_bucket_of_trees_to_render_queue(int x, int y)
{
	tree_node* t_node = t_grid->bucket(x, y);
	for(; t_node; t_node = t_node->next)
	{
		if(t_node->age > 0)
		{
			r_queue.push(t_node);
		}
	}
}

int models_cleared = 0;

#define LOW_DETAIL 0
#define MID_DETAIL 1
#define HIGH_DETAIL 2

#define MID_DETAIL_DIST 40.0f
#define HIGH_DETAIL_DIST 20.0f

void ForestGLWidget::generate_tree_models(vec3 view_pos)
{
	int species = -1;
	int p_species = -1;
	int age = -1;
	int p_age = -1;
	long int seed = -1;
	long int p_seed = -1;
	int p_model_cache_ref = -1;
	int previous_model = -1;

	for(int i = 0; i < r_queue.length; i++)
	{//For each node in the render queue
		tree_node* t_node = r_queue.find_item(i);

		species = t_node->species;
		age = t_node->age;
		seed = t_node->seed;

		vec3 tree_pos = {t_node->_x, 0.0f, t_node->_y};
		float dist = magnitude(view_pos - tree_pos);
		
		int lod;
		if(apply_lod)
		{
			if(dist < HIGH_DETAIL_DIST) lod = HIGH_DETAIL;
			else if(dist < MID_DETAIL_DIST) lod = MID_DETAIL;
			else lod = LOW_DETAIL;
		}
		else lod = HIGH_DETAIL;

		//Generate tree string
		if(species != p_species || age != p_age || seed != p_seed)
		{//If a new string should be derived
			//Derive string
			if(species == p_species && seed == p_seed)
			{
				int d_age = age - p_age;
				model_generator.derive_tree_str(d_age, species);
			}
			else
			{
				model_generator.load_axiom(species);
				model_generator.set_seed(seed);
				model_generator.derive_tree_str(age, species);
			}

			//Load model
			if(t_map.tree_has_model(t_node))
			{//If the tree_node has a model in cache already
				if(t_node->changed || t_map.lod(t_node) != lod)
				{//If t_node's model should change
					if(t_map.model_ref_count(t_node) == 1)
					{//If the model is only used for rendering t_node
						clear_buffers(t_map.find_model(t_node));
						models_cleared++;
					}
					t_map.release_ref(t_node);
					t_map.add_model_ref(t_node);
					generate_tree_model(t_node, lod);
					models_gen++;
					previous_model = t_map.model_ref(t_node);
					t_node->changed = false;
				}
			}
			else
			{//If the tree_node has no model in cache
				t_map.add_model_ref(t_node);

				//Gen model in cache index
				generate_tree_model(t_node, lod);
				models_gen++;
				previous_model = t_map.model_ref(t_node);
				t_node->changed = false;
			}
		}
		else 
		{//If no new string needed to be derived (t_node has same model string as previous tree)
			if(t_node->changed || t_map.lod(t_node) != lod)
			{//If t_node's model ref should be updated
				if(t_map.tree_has_model(t_node))
				{
					if(t_map.model_ref_count(t_node) == 1)
					{//If the model is only used for rendering t_node
						clear_buffers(t_map.find_model(t_node));
						models_cleared++;
					}
					t_map.release_ref(t_node);
				}
				if(t_map.lod(previous_model) != lod)
				{
					t_map.add_model_ref(t_node);
					generate_tree_model(t_node, lod);
					models_gen++;
					previous_model = t_map.model_ref(t_node);
				}
				else t_map.add_model_ref(t_node, previous_model);
				t_node->changed = false;
			}
		}

		
		p_species = species;
		p_age = age;
		p_seed = seed;
	}
}

int rendered = 0;
void ForestGLWidget::render_tree_models()
{
	for(tree_node* t_node = r_queue.pop(); r_queue.length >= 0; t_node = r_queue.pop())
	{
		glPushMatrix();
		glTranslatef(t_node->_x, 0.0f, t_node->_y);
		render(t_map.find_model(t_node), t_node->species);
		rendered++;
		glPopMatrix();
	}
	r_queue.reset(); //Not a circular buffer so head ptr needs resetting
}

int ForestGLWidget::pick_buckets_to_render(int* buckets_to_render, vec3 view_pos, vec3 view_dir, float view_dist)
{
	int number_of_buckets_to_render = 0;

	vec3 v = view_pos/(float)BUCKET_LENGTH;
	float d = view_dist/(float)BUCKET_LENGTH;
	float rad = 45.0f * (PI/180.0f);
	float f = d/cos(rad);

	vec3 v_1 = rotate_about_axis(f*view_dir, vec3{0.0f, 1.0f, 0.0f}, 45) + v;
	vec3 v_2 = rotate_about_axis(f*view_dir, vec3{0.0f, -1.0f, 0.0f}, 45) + v;

	//Calculate bounds of view range
	int min_x = (int)min(min(v.x, v_1.x),v_2.x) - 1;
	int max_x = (int)max(max(v.x, v_1.x), v_2.x) + 2;
	int min_y = (int)min(min(v.z, v_1.z), v_2.z) - 1;
	int max_y = (int)max(max(v.z, v_1.z), v_2.z) + 2;

	for(int x = min_x; x < max_x; x++)
	{
		for(int y = min_y; y < max_y; y++)
		{
			if(x >= 0 && x < t_grid->width() && y >= 0 && y < t_grid->height())
			{
				buckets_to_render[2*number_of_buckets_to_render] = x;
				buckets_to_render[2*number_of_buckets_to_render+1] = y;
				number_of_buckets_to_render++;
			}
		}
	}

	return number_of_buckets_to_render;
}

void ForestGLWidget::render_forest()
{	
	MEASURE_TIME;
	if(platform.vertex_buffer == 0) load_platform();

	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glPushMatrix();
	glLoadIdentity();

	//Get track ball data
	GLfloat current_ball_value[16];
	GLfloat light_pos[] = {0.0f, 1.0f, 1.0f, 0.0f};

	Ball_Value(&light_ball_data, current_ball_value);
	glMultMatrixf(current_ball_value);

	//Set light position
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0f);

	glLoadIdentity();
	//glTranslatef(0.0f, -0.2f, -1.0f);
	glTranslatef(translate_x, translate_y, translate_z);

	//Calculate view position and direction
	vec3 view_pos = {-translate_x, -translate_y, -translate_z};
	vec3 view_dir = {0.0f, 0.0f, -1.0f};
	
	Ball_Value(&forest_ball_data, current_ball_value);
	glMultMatrixf(current_ball_value);

	transpose_matrix_4x4(current_ball_value);
	view_pos = matrix_multiply_4x4(view_pos, current_ball_value);
	view_dir = matrix_multiply_4x4(view_dir, current_ball_value);
	
	material grass_material =
	{
		{0.004f, 0.196f, 0.125f},
		{0.227f, 0.373f, 0.043f},
		{0.5f, 0.51f, 0.5f},
		2.0f
	};

	set_material(&grass_material);

	//Set fog parameters
	glEnable(GL_FOG);
	glFogf(GL_FOG_START, 0.5f*view_dist);
	glFogf(GL_FOG_END, 2.0f*view_dist);
	glFogf(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_DENSITY, 0.05f);

	GLfloat fog_colour[] = {0.494f, 0.753f, 0.933f, 1.0f};
	glFogfv(GL_FOG_COLOR, fog_colour);

	//Render plane
	render(platform);
	
	glTranslatef(-(forest_width/2.0f),0.0f,-(forest_height/2.0f));
	view_pos = view_pos + vec3{forest_width/2.0f,0.0f,forest_height/2.0f};
	view_dir.y = 0.0f;
	view_dir = normalise(view_dir);
	
	printf("------------------------\n");
	
	models_gen = 0;
	models_cleared = 0;

	clear_unused_model_buffers();

	//Determine which tree node buckets should be rendered
	int buckets_to_render[400] = {};
	int number_of_buckets_to_render = pick_buckets_to_render(buckets_to_render, view_pos, view_dir, view_dist);
	for(int i = 0; i < number_of_buckets_to_render; i++)
	{
		int x = buckets_to_render[2*i];
		int y = buckets_to_render[2*i+1];
		push_bucket_of_trees_to_render_queue(x,y);
	}

	r_queue.sort();

	generate_tree_models(view_pos);

	rendered = 0;
	render_tree_models();

	printf("------------------------\n");

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);

	glPopMatrix();
}

void output_ppm(GLubyte* pixels, int width, int height)
{
	FILE* ppm = fopen("image.ppm", "w");
	fprintf(ppm, "P3\n");
	fprintf(ppm, "%d %d\n", width, height);
	fprintf(ppm, "255\n");

	for(int i = height-1; i >= 0; i--)
	{
		for(int j = 0; j < width; j++)
		{
			GLubyte* px = pixels+(3*i*width)+(3*j);
			fprintf(ppm, "%d %d %d\n", px[0], px[1], px[2]);
		}
	}

	fclose(ppm);
}

void ForestGLWidget::capture()
{
	int out_width = 2048;
	int out_height = 2048;

	GLuint colour_rbo;
	glGenRenderbuffers(1, &colour_rbo);
	
	glBindRenderbuffer(GL_RENDERBUFFER, colour_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, out_width, out_height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	GLuint depth_rbo;
	glGenRenderbuffers(1, &depth_rbo);

	glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, out_width, out_height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colour_rbo);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE) 
	{
		printf("Error in framebuffer construction\n");
	}
	else
	{	
		glViewport(0,0,out_width, out_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if(chart_mode) render_chart();
		else render_forest();
		glFlush();

		GLubyte* px_buffer = (GLubyte*)malloc(3*out_width*out_height*sizeof(GLubyte));
		glReadPixels(0, 0, out_width, out_height, GL_RGB, GL_UNSIGNED_BYTE, px_buffer);

		output_ppm(px_buffer, out_width, out_height);
		free(px_buffer);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);
	glDeleteRenderbuffers(1, &depth_rbo);
	glDeleteRenderbuffers(1, &colour_rbo);
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
