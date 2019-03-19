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

	QSlider* view_dist_slider = new QSlider(Qt::Horizontal, this);
	view_dist_slider->setMinimum(20);
	view_dist_slider->setMaximum(80);
	view_dist_slider->setValue(30);
	layout->addWidget(view_dist_slider);

	connect(derive_button, SIGNAL(clicked()), this->m_l_widget, SLOT(run_derivation()));
	connect(reset_button, SIGNAL(clicked()), this->m_l_widget, SLOT(init_system()));
	connect(chart_mode_button, SIGNAL(pressed()), this->gl_widget, SLOT(set_chart_mode()));
	connect(forest_mode_button, SIGNAL(pressed()), this->gl_widget, SLOT(set_forest_mode()));
	connect(view_dist_slider, SIGNAL(valueChanged(int)), this->gl_widget, SLOT(set_view_dist(int)));
}
