#include "ForestWalkWindow.h"

AppWindow::AppWindow(QWidget* parent): QWidget(parent)
{
	layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
}

void AppWindow::init()
{
	gl_widget = new ChartGLWidget(this);
	m_l_widget = new MLSystemWidget(this, gl_widget);
	layout->addWidget(gl_widget);
	QBoxLayout* sim_control_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	QPushButton* derive_button = new QPushButton("Derive");
	QPushButton* reset_button = new QPushButton("Reset");
	sim_control_layout->addWidget(derive_button);
	sim_control_layout->addWidget(reset_button);
	layout->addLayout(sim_control_layout);

	connect(derive_button, SIGNAL(clicked()), this->m_l_widget, SLOT(run_derivation()));
	connect(reset_button, SIGNAL(clicked()), this->m_l_widget, SLOT(init_system()));
}
