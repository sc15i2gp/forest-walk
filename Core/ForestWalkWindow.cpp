#include "ForestWalkWindow.h"

AppWindow::AppWindow(QWidget* parent): QWidget(parent)
{
	layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
}

void AppWindow::init()
{
	gl_widget = new ForestGLWidget(this);
	m_l_widget = new MLSystemWidget(this, gl_widget);
	layout->addWidget(gl_widget);
	QBoxLayout* sim_control_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	QPushButton* derive_button = new QPushButton("Derive");
	QPushButton* reset_button = new QPushButton("Reset");
	sim_control_layout->addWidget(derive_button);
	sim_control_layout->addWidget(reset_button);
	layout->addLayout(sim_control_layout);

	QBoxLayout* chart_mode_control_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	QRadioButton* chart_mode_button = new QRadioButton("Chart Mode", this);
	QRadioButton* forest_mode_button = new QRadioButton("Forest Mode", this);
	chart_mode_button->setChecked(true);
	chart_mode_control_layout->addWidget(chart_mode_button);
	chart_mode_control_layout->addWidget(forest_mode_button);
	layout->addLayout(chart_mode_control_layout);

	QBoxLayout* chart_toggle_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	QCheckBox* show_domination = new QCheckBox("Show domination", this);
	QCheckBox* show_old_age = new QCheckBox("Show old age", this);
	QCheckBox* show_view_range = new QCheckBox("Show view range", this);
	QCheckBox* show_grid = new QCheckBox("Show grid", this);
	chart_toggle_layout->addWidget(show_domination);
	chart_toggle_layout->addWidget(show_old_age);
	chart_toggle_layout->addWidget(show_view_range);
	chart_toggle_layout->addWidget(show_grid);
	layout->addLayout(chart_toggle_layout);

	QBoxLayout* ecological_rules_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	QCheckBox* self_thinning_should_happen = new QCheckBox("Self-thinning", this);
	QCheckBox* trees_should_propagate = new QCheckBox("Propagation", this);
	QCheckBox* succession_should_happen = new QCheckBox("Succession", this);
	ecological_rules_layout->addWidget(self_thinning_should_happen);
	ecological_rules_layout->addWidget(trees_should_propagate);
	ecological_rules_layout->addWidget(succession_should_happen);
	succession_should_happen->setCheckState(Qt::Checked);
	self_thinning_should_happen->setCheckState(Qt::Checked);
	layout->addLayout(ecological_rules_layout);

	QSlider* view_dist_slider = new QSlider(Qt::Horizontal, this);
	view_dist_slider->setMinimum(10);
	view_dist_slider->setMaximum(80);
	view_dist_slider->setValue(30);
	layout->addWidget(view_dist_slider);

	connect(derive_button, SIGNAL(clicked()), this->m_l_widget, SLOT(run_derivation()));
	connect(reset_button, SIGNAL(clicked()), this->m_l_widget, SLOT(init_system()));
	connect(chart_mode_button, SIGNAL(pressed()), this->gl_widget, SLOT(set_chart_mode()));
	connect(forest_mode_button, SIGNAL(pressed()), this->gl_widget, SLOT(set_forest_mode()));
	connect(view_dist_slider, SIGNAL(valueChanged(int)), this->gl_widget, SLOT(set_view_dist(int)));
	connect(show_old_age, SIGNAL(stateChanged(int)), this, SLOT(show_old_age_checked(int)));
	connect(show_view_range, SIGNAL(stateChanged(int)), this, SLOT(show_view_range_checked(int)));
	connect(show_grid, SIGNAL(stateChanged(int)), this, SLOT(show_grid_checked(int)));
	connect(show_domination, SIGNAL(stateChanged(int)), this, SLOT(show_domination_checked(int)));
	connect(self_thinning_should_happen, SIGNAL(stateChanged(int)), this, SLOT(self_thinning_checked(int)));
	connect(trees_should_propagate, SIGNAL(stateChanged(int)), this, SLOT(propagation_checked(int)));
	connect(succession_should_happen, SIGNAL(stateChanged(int)), this, SLOT(succession_checked(int)));
}

void AppWindow::show_domination_checked(int state)
{
	gl_widget->set_show_domination(state == Qt::Checked);
}

void AppWindow::show_old_age_checked(int state)
{
	gl_widget->set_show_old_age(state == Qt::Checked);
}

void AppWindow::show_view_range_checked(int state)
{
	gl_widget->set_show_view_range(state == Qt::Checked);
}

void AppWindow::show_grid_checked(int state)
{
	gl_widget->set_show_grid(state == Qt::Checked);
}

void AppWindow::self_thinning_checked(int state)
{
	m_l_widget->set_self_thinning(state == Qt::Checked);
}

void AppWindow::propagation_checked(int state)
{
	m_l_widget->set_propagation(state == Qt::Checked);
}

void AppWindow::succession_checked(int state)
{
	m_l_widget->set_succession(state == Qt::Checked);
}
