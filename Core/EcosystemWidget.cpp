#include "EcosystemWidget.h"

EcosystemWidget::EcosystemWidget(QWidget* parent, ForestGLWidget* gl): QWidget(parent)
{
	forest_length = 100;
	gl_widget = gl;
	gl_widget->set_forest_bounds((float)forest_length, (float)forest_length);
	gl_widget->set_tree_grid(&ecosystem.t_grid);
	ecosystem = create_ecosystem(8192, forest_length);
	seed = (long int)time(NULL);
	connect(gl_widget, SIGNAL(initialised()), this, SLOT(init_system()));
}

EcosystemWidget::~EcosystemWidget()
{
	destroy_ecosystem(&ecosystem);
}

void EcosystemWidget::init_system()
{
	ecosystem.m_l_sys.clear_str_set();
	ecosystem.remove_all_trees();
	srand(seed);
	for(int i = 0; i < 128; i++)
	{
		int s = (i < 20) ? ROWAN : ((i < 74) ? PINE : BIRCH);
		float x = rand() % forest_length;
		float y = rand() % forest_length;
		float r = (float)(rand() % forest_length)/(forest_length+200.0f) + 0.1f;
		ecosystem.add_tree(x, y, r, s);
	}
	ecosystem.update_tree_grid_data();
}

void EcosystemWidget::set_self_thinning(bool b)
{
	ecosystem.self_thinning_should_happen = b;
	if(ecosystem.self_thinning_should_happen)
	{
		ecosystem.tree_domination_check();
	}
	else
	{
		ecosystem.set_all_trees_not_dominated();
	}
	ecosystem.update_tree_grid_data();
}

void EcosystemWidget::set_succession(bool b)
{
	ecosystem.succession_should_happen = b;
	parentWidget()->update();
	gl_widget->update();
}

void EcosystemWidget::set_propagation(bool b)
{
	ecosystem.trees_should_propagate = b;
	parentWidget()->update();
	gl_widget->update();
}

void EcosystemWidget::run_derivation()
{
	ecosystem.iterate_forest_by_one_plastochron();
	ecosystem.update_tree_grid_data();
	parentWidget()->update();
	gl_widget->update();
}
