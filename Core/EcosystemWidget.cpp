#include "EcosystemWidget.h"

EcosystemWidget::EcosystemWidget(QWidget* parent, ForestGLWidget* gl): QWidget(parent)
{
	plastochron_count = 1;
	int forest_length = 200;
	ecosystem = create_ecosystem(8192, forest_length);
	ecosystem.forest_length = forest_length;
	seed = (long int)time(NULL);

	gl_widget = gl;
	gl_widget->set_forest_bounds((float)forest_length, (float)forest_length);
	gl_widget->set_tree_grid(&ecosystem.t_grid);

	connect(gl_widget, SIGNAL(initialised()), this, SLOT(init_ecosystem()));
}

EcosystemWidget::~EcosystemWidget()
{
	destroy_ecosystem(&ecosystem);
}

void EcosystemWidget::populate_initial_ecosystem()
{
	srand(seed);

	for(int i = 0; i < 128; i++)
	{
		int s = (i < 20) ? ROWAN : ((i < 74) ? PINE : BIRCH);

		float x = rand() % ecosystem.forest_length;
		float y = rand() % ecosystem.forest_length;
		float r = ecosystem.generate_initial_tree_radius();

		ecosystem.add_tree(x, y, r, s);
	}
	/*
	ecosystem.add_tree(100, 90, 0.5f, PINE);
	ecosystem.add_tree(90, 80, 0.5f, PINE);
	ecosystem.add_tree(110, 80, 0.5f, PINE);

	ecosystem.add_tree(80, 70, 0.5f, PINE);
	ecosystem.add_tree(120, 70, 0.5f, PINE);

	ecosystem.add_tree(70, 60, 0.5f, PINE);
	ecosystem.add_tree(130, 60, 0.5f, PINE);
	*/
}

void EcosystemWidget::init_ecosystem()
{
	ecosystem.number_of_derivations = 0;
	ecosystem.remove_all_trees();

	populate_initial_ecosystem();

	ecosystem.update_tree_grid_data();

	parentWidget()->update();
	gl_widget->update();
}

void EcosystemWidget::set_self_thinning(bool b)
{
	ecosystem.self_thinning_should_happen = b;

	if(ecosystem.self_thinning_should_happen) ecosystem.tree_domination_check();
	else ecosystem.set_all_trees_not_dominated();

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

void EcosystemWidget::set_plastochron_count(int n)
{
	plastochron_count = n;
}

void EcosystemWidget::iterate_plastochron()
{
	for(int i = 0; i < plastochron_count; i++)
	{
		BEGIN_MEASURE;
		ecosystem.iterate_forest_by_one_plastochron();
		END_MEASURE;
	}
	parentWidget()->update();
	gl_widget->update();
}
