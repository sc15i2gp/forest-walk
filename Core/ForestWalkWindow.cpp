#include "ForestWalkWindow.h"

AppWindow::AppWindow(QWidget* parent): QWidget(parent)
{
	layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
}

void AppWindow::init()
{
	gl_widget = new ForestGLWidget(this);
	ecosystem_widget = new EcosystemWidget(this, gl_widget);
	layout->addWidget(gl_widget);
	
	//Simulation controls
	QBoxLayout* sim_control_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	QBoxLayout* sim_step_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	QPushButton* derive_button = new QPushButton("Derive",this);
	QPushButton* reset_button = new QPushButton("Reset",this);
	QPushButton* capture_button = new QPushButton("Capture",this);
	QSpinBox* spin_box = new QSpinBox(this);
	spin_box->setMinimum(1);
	sim_step_layout->addWidget(spin_box);
	sim_step_layout->addWidget(derive_button);
	sim_control_layout->addLayout(sim_step_layout);
	sim_control_layout->addWidget(reset_button);
	sim_control_layout->addWidget(capture_button);
	layout->addLayout(sim_control_layout);

	//Chart mode controls
	QBoxLayout* chart_mode_control_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	QLabel* view_label = new QLabel("View Modes:",this);
	view_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum);
	view_label->setMaximumWidth(150);
	QRadioButton* chart_mode_button = new QRadioButton("Chart Mode", this);
	chart_mode_button->setMaximumWidth(150);
	QRadioButton* forest_mode_button = new QRadioButton("Forest Mode", this);
	chart_mode_button->setChecked(true);
	chart_mode_control_layout->addWidget(view_label);
	chart_mode_control_layout->addWidget(chart_mode_button);
	chart_mode_control_layout->addWidget(forest_mode_button);
	layout->addLayout(chart_mode_control_layout);

	QBoxLayout* chart_toggle_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	QLabel* chart_controls_label = new QLabel("Chart controls:",this);
	chart_controls_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum);
	chart_controls_label->setMaximumWidth(150);
	QCheckBox* show_domination = new QCheckBox("Show domination", this);
	show_domination->setMaximumWidth(150);
	QCheckBox* show_old_age = new QCheckBox("Show old age", this);
	show_old_age->setMaximumWidth(150);
	QCheckBox* show_view_range = new QCheckBox("Show view range", this);
	show_old_age->setMaximumWidth(150);
	QCheckBox* show_grid = new QCheckBox("Show grid", this);
	chart_toggle_layout->addWidget(chart_controls_label);
	chart_toggle_layout->addWidget(show_domination);
	chart_toggle_layout->addWidget(show_old_age);
	chart_toggle_layout->addWidget(show_view_range);
	chart_toggle_layout->addWidget(show_grid);

	//Ecological rule toggles
	QBoxLayout* ecological_rules_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	QLabel* rules_label = new QLabel("Ecological Rules:",this);
	rules_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum);
	rules_label->setMaximumWidth(150);
	QCheckBox* self_thinning_should_happen = new QCheckBox("Self-thinning", this);
	self_thinning_should_happen->setMaximumWidth(150);
	QCheckBox* trees_should_propagate = new QCheckBox("Propagation", this);
	trees_should_propagate->setMaximumWidth(150);
	QCheckBox* succession_should_happen = new QCheckBox("Succession", this);
	ecological_rules_layout->addWidget(rules_label);
	ecological_rules_layout->addWidget(self_thinning_should_happen);
	ecological_rules_layout->addWidget(trees_should_propagate);
	ecological_rules_layout->addWidget(succession_should_happen);
	succession_should_happen->setCheckState(Qt::Checked);
	self_thinning_should_happen->setCheckState(Qt::Checked);
	layout->addLayout(ecological_rules_layout);
	layout->addLayout(chart_toggle_layout);

	//View range slider
	QBoxLayout* render_settings_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	QLabel* forest_mode_label = new QLabel("3D controls:",this);
	forest_mode_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum);
	forest_mode_label->setMaximumWidth(150);
	QSlider* view_dist_slider = new QSlider(Qt::Horizontal, this);
	view_dist_slider->setMaximumWidth(514);
	QCheckBox* apply_lod = new QCheckBox("Apply L.O.D.", this);
	view_dist_slider->setMinimum(10);
	view_dist_slider->setMaximum(80);
	view_dist_slider->setValue(30);
	apply_lod->setCheckState(Qt::Checked);
	render_settings_layout->addWidget(forest_mode_label);
	render_settings_layout->addWidget(view_dist_slider);
	render_settings_layout->addWidget(apply_lod);
	layout->addLayout(render_settings_layout);

	connect(derive_button, SIGNAL(clicked()), this->ecosystem_widget, SLOT(iterate_plastochron()));
	connect(spin_box, SIGNAL(valueChanged(int)), this, SLOT(set_plastochron_count(int)));
	connect(reset_button, SIGNAL(clicked()), this->ecosystem_widget, SLOT(init_ecosystem()));
	connect(capture_button, SIGNAL(clicked()), this->gl_widget, SLOT(capture()));
	connect(chart_mode_button, SIGNAL(pressed()), this->gl_widget, SLOT(set_chart_mode()));
	connect(forest_mode_button, SIGNAL(pressed()), this->gl_widget, SLOT(set_forest_mode()));
	connect(view_dist_slider, SIGNAL(valueChanged(int)), this->gl_widget, SLOT(set_view_dist(int)));
	connect(apply_lod, SIGNAL(stateChanged(int)), this, SLOT(apply_lod_checked(int)));
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
	ecosystem_widget->set_self_thinning(state == Qt::Checked);
}

void AppWindow::propagation_checked(int state)
{
	ecosystem_widget->set_propagation(state == Qt::Checked);
}

void AppWindow::succession_checked(int state)
{
	ecosystem_widget->set_succession(state == Qt::Checked);
}

void AppWindow::set_plastochron_count(int n)
{
	ecosystem_widget->set_plastochron_count(n);
}

void AppWindow::apply_lod_checked(int state)
{
	gl_widget->set_apply_lod(state == Qt::Checked);
}
