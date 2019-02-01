#include "LSystemEditorWindow.h"

AppWindow::AppWindow(QWidget* parent): QWidget(parent)
{
	layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
}

void AppWindow::init_GLwidget()
{
	gl_widget = new GLWidget(this);
	derive_button = new QPushButton("Derive", this);
	reset_button  = new QPushButton("Reset", this);
	l_system_widget = new LSystemWidget(this, gl_widget);
	layout->addWidget(gl_widget);
	layout->addWidget(derive_button);
	layout->addWidget(reset_button);
	connect(derive_button, SIGNAL(clicked()), this, SLOT(update_render()));
	connect(reset_button, SIGNAL(clicked()), this, SLOT(reset()));
}

AppWindow::~AppWindow()
{
	delete layout;
}

void AppWindow::update_render()
{
	l_system_widget->l_system_derivation();
	gl_widget->update();
}

void AppWindow::reset()
{
	l_system_widget->reset();
	gl_widget->update();
}
