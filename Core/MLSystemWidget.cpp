#include "MLSystemWidget.h"

MLSystemWidget::MLSystemWidget(QWidget* parent, ChartGLWidget* c): QWidget(parent)
{
	forest_length = 200;
	chart = c;
	chart->set_forest_bounds((float)forest_length, (float)forest_length);
	m_l_sys = create_m_l_system(8192, forest_length);
	add_production(&m_l_sys, "<T(s,x,y,r)>?(c)", "", "c == 0", 1.0f);
	add_production(&m_l_sys, "<T(s,x,y,r)>", "T(s,x,y,r)", "r >= R", 1.0f);
	add_global_parameter(&m_l_sys, 'R', "5.0");
	add_global_parameter(&m_l_sys, 'v', "390.0");
	add_global_parameter(&m_l_sys, 'w', "390.0");
	add_global_parameter(&m_l_sys, 't', "10.0");
	add_global_parameter(&m_l_sys, 'G', "20.0");
	add_production(&m_l_sys, "<T(s,x,y,r)>?(c)", "T(s,x,y,G*r)[%T(s,v,w,t)?(1)]", NULL, 1.0f);
	//add_production(&m_l_sys, "<T(s,x,y,r)>?(c)", "T(s,x,y,1.947*r)", NULL, 1.0f);
	seed = (long int)time(NULL);
	connect(chart, SIGNAL(initialised()), this, SLOT(init_system()));
}

MLSystemWidget::~MLSystemWidget()
{
	destroy_m_l_system(&m_l_sys);
}

void MLSystemWidget::init_system()
{
	clear_str_set(&m_l_sys);
	chart->clear_points();
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

void MLSystemWidget::push_str_set_to_chart_and_render()
{
	str_m_set* s = &(m_l_sys.str_set);
	for(int i = 0; i < s->size(); i++)
	{
		if(s->is_allocated(i))
		{
			char* str = s->find_str(i);
			if(number_of_modules(str) > 1)
			{
				int s = (int)read_real_parameter_value(str, 0);
				float x = read_real_parameter_value(str, 1);
				float y = read_real_parameter_value(str, 2);
				float r = read_real_parameter_value(str, 3);
				int c = (int)read_real_parameter_value(find_next_module(str));
				chart->push_point(x,y,r,c,s);
			}
		}
	}
	parentWidget()->update();
	chart->update();
}

void MLSystemWidget::run_derivation()
{
	chart->clear_points();
	derive_set(&m_l_sys);
	//print_l_system(&m_l_sys.base_sys, "multiset");
	//print_str_set(&m_l_sys);
	push_str_set_to_chart_and_render();
}
