#include "MLSystemWidget.h"

MLSystemWidget::MLSystemWidget(QWidget* parent, ChartGLWidget* c): QWidget(parent)
{
	float forest_width = 400.0f;
	float forest_height = 400.0f;
	chart = c;
	chart->set_forest_bounds(forest_width, forest_height);
	m_l_sys = create_m_l_system(8192);
	add_production(&m_l_sys, "<T(x,y,r)>?(c)", "", "c == 0", 1.0f);
	add_production(&m_l_sys, "<T(x,y,r)>", "T(x,y,r)", "r >= R", 1.0f);
	add_global_parameter(&m_l_sys, 'R', "5.0");
	add_production(&m_l_sys, "<T(x,y,r)>?(c)", "T(x,y,r + 1.01*r)", NULL, 1.0f);
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
	for(int i = 0; i < 8192; i++)
	{
		float x = rand() % 400;
		float y = rand() % 400;
		float r = (float)(rand() % 400)/600.0f + 0.1f;
		add_str(&m_l_sys, x, y, r, 1);
	}
	push_str_set_to_chart_and_render();
}

void MLSystemWidget::push_str_set_to_chart_and_render()
{
	for(int i = 0; i < m_l_sys.max_str_set_size; i++)
	{
		char* str = get_str_from_set(&m_l_sys, i);
		if(number_of_modules(str) > 1)
		{
			float x = read_real_parameter_value(str, 0);
			float y = read_real_parameter_value(str, 1);
			float r = read_real_parameter_value(str, 2);
			int c = (int)read_real_parameter_value(find_next_module(str));
			chart->push_point(x,y,r,c);
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
