#include "LSystemEditorWindow.h"

AppWindow::AppWindow(QWidget* parent): QWidget(parent)
{
	layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
}

void AppWindow::init_GLwidget()
{
	gl_widget = new GLWidget(this);
	QPushButton* derive_button = new QPushButton("Derive", this);
	QPushButton* reset_button  = new QPushButton("Reset", this);
	QBoxLayout* camera_controls_layout = new QBoxLayout(QBoxLayout::LeftToRight, NULL);
	l_system_widget = new LSystemWidget(this, gl_widget);
	layout->addWidget(gl_widget);
	layout->addWidget(derive_button);
	layout->addWidget(reset_button);
	layout->addLayout(camera_controls_layout);

	QGridLayout* camera_move_grid = new QGridLayout;
	QGridLayout* target_move_grid = new QGridLayout;

	QPushButton* camera_move_up = new QPushButton("^");
	QPushButton* camera_move_down = new QPushButton("v");
	QPushButton* camera_move_left = new QPushButton("<");
	QPushButton* camera_move_right = new QPushButton(">");
	camera_move_grid->addWidget(camera_move_up, 0, 1);
	camera_move_grid->addWidget(camera_move_down, 2, 1);
	camera_move_grid->addWidget(camera_move_left, 1, 0);
	camera_move_grid->addWidget(camera_move_right, 1, 2);

	QPushButton* camera_target_move_up = new QPushButton("^");
	QPushButton* camera_target_move_down = new QPushButton("v");
	target_move_grid->addWidget(camera_target_move_up, 0, 1);
	target_move_grid->addWidget(camera_target_move_down, 2, 1);
	target_move_grid->addWidget(new QPushButton("<"), 1, 0);
	target_move_grid->addWidget(new QPushButton(">"), 1, 2);
	
	camera_controls_layout->addLayout(camera_move_grid);
	camera_controls_layout->addLayout(target_move_grid);

	connect(derive_button, SIGNAL(clicked()), this, SLOT(update_render()));
	connect(reset_button, SIGNAL(clicked()), this, SLOT(reset()));

	connect(camera_move_up, SIGNAL(clicked()), gl_widget, SLOT(move_camera_up()));
	connect(camera_move_down, SIGNAL(clicked()), gl_widget, SLOT(move_camera_down()));
	connect(camera_move_left, SIGNAL(clicked()), gl_widget, SLOT(move_camera_left()));
	connect(camera_move_right, SIGNAL(clicked()), gl_widget, SLOT(move_camera_right()));
	connect(camera_target_move_up, SIGNAL(clicked()), gl_widget, SLOT(move_camera_target_up()));
	connect(camera_target_move_down, SIGNAL(clicked()), gl_widget, SLOT(move_camera_target_down()));
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
