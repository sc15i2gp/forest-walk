#include "ForestWalkWindow.h"

AppWindow::AppWindow(QWidget* parent): QWidget(parent)
{
	layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
}

void AppWindow::init()
{
	QBoxLayout* sim_control_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	sim_control_layout->addWidget(new QPushButton("Derive"));
	sim_control_layout->addWidget(new QPushButton("Reset"));
	layout->addLayout(sim_control_layout);
}
