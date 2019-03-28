#include "MLSystemWidget.h"

MLSystemWidget::MLSystemWidget(QWidget* parent, ForestGLWidget* c): QWidget(parent)
{
	forest_length = 100;
	chart = c;
	chart->set_forest_bounds((float)forest_length, (float)forest_length);
	m_l_sys = create_m_l_system(8192, forest_length);
	chart->set_tree_grid(&m_l_sys.t_grid);
	for(int i = 0; i < 8; i++) tree_seeds[i] = (long int)rand();
	seed = (long int)time(NULL);
	load_productions_and_parameters();
	connect(chart, SIGNAL(initialised()), this, SLOT(init_system()));
}

MLSystemWidget::~MLSystemWidget()
{
	destroy_m_l_system(&m_l_sys);
}

void MLSystemWidget::load_productions_and_parameters()
{
	add_global_parameter(&m_l_sys, 'R', "5.0");
	add_global_parameter(&m_l_sys, 'v', "390.0");
	add_global_parameter(&m_l_sys, 'w', "390.0");
	add_global_parameter(&m_l_sys, 't', "10.0");
	add_global_parameter(&m_l_sys, 'G', "20.0");
	add_production(&m_l_sys, "<T(s,x,y,r,a)>?(c)", "", "c == 0", 1.0f);
	add_production(&m_l_sys, "<T(s,x,y,r,a)>?(c)", "T(s,x,y,r,a)", "c==0", 0.0f); //Shade tolerance
	add_production(&m_l_sys, "<T(s,x,y,r,a)>", "T(s,x,y,r,a)", "r >= R", 1.0f);
	add_production(&m_l_sys, "<T(s,x,y,r,a)>", "", "r >= R", 0.0f); //Longevity
	add_production(&m_l_sys, "<T(s,x,y,r,a)>?(c)", "T(s,x,y,G*r,a+1)[%T(s,v,w,t,0)?(1)]", NULL, 1.0f);
}

void MLSystemWidget::init_system()
{
	clear_str_set(&m_l_sys);
	srand(seed);
	for(int i = 0; i < 128; i++)
	{
		int s = (i < 20) ? ROWAN : ((i < 74) ? PINE : BIRCH);
		float x = rand() % forest_length;
		float y = rand() % forest_length;
		float r = (float)(rand() % forest_length)/(forest_length+200.0f) + 0.1f;
		add_str(&m_l_sys, x, y, r, s);
	}
	push_str_set_to_chart_and_render();
}

void MLSystemWidget::set_self_thinning(bool b)
{
	m_l_sys.self_thinning_should_happen = b;
	if(m_l_sys.self_thinning_should_happen)
	{
		tree_domination_check(&m_l_sys);
	}
	else
	{
		set_all_trees_not_dominated(&m_l_sys);
	}
	push_str_set_to_chart_and_render();
}

void MLSystemWidget::set_succession(bool b)
{
	m_l_sys.succession_should_happen = b;
	parentWidget()->update();
	chart->update();
}

void MLSystemWidget::set_propagation(bool b)
{
	m_l_sys.trees_should_propagate = b;
	parentWidget()->update();
	chart->update();
}

void MLSystemWidget::push_str_set_to_chart_and_render()
{
	str_m_set* s = &(m_l_sys.str_set);
	for(int i = 0; i < m_l_sys.t_grid.height(); i++)
	{
		for(int j = 0; j < m_l_sys.t_grid.width(); j++)
		{
			tree_node* tree = m_l_sys.t_grid.bucket(j,i);
			for(; tree; tree = tree->next)
			{
				char* str = s->find_str(tree->str_ref);
				if(number_of_modules(str) > 1)
				{
					int s = (int)read_real_parameter_value(str, 0);
					float x = read_real_parameter_value(str, 1);
					float y = read_real_parameter_value(str, 2);
					float r = read_real_parameter_value(str, 3);
					int age = (int)read_real_parameter_value(str, 4);
					int c = (int)read_real_parameter_value(find_next_module(str));
					if(tree->seed < 0) tree->seed = tree_seeds[rand() % 8];
					tree->species = s;
					tree->age = age;
					const char* max_r = (s == PINE) ? PINE_MAX_RADIUS : (s == BIRCH) ? BIRCH_MAX_RADIUS : ROWAN_MAX_RADIUS;
					tree->old_age = r >= atof(max_r);
					tree->_x = x;
					tree->_y = y;
					tree->_r = r;
					tree->dominated = c;
				}
			}
		}
	}
	parentWidget()->update();
	chart->update();
}

void MLSystemWidget::run_derivation()
{
	derive_set(&m_l_sys);
	m_l_sys.t_grid.derived = true;
	push_str_set_to_chart_and_render();
}
