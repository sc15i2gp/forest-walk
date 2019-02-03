#include "LSystemEditorWindow.h"

AppWindow::AppWindow(QWidget* parent): QWidget(parent)
{
	layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	signal_map = new QSignalMapper(this);
}

void AppWindow::add_production_text_fields()
{
	static int layout_id = 1;
	if(number_of_production_layouts < 16)
	{
		QBoxLayout* l_system_production_layout = new QBoxLayout(QBoxLayout::LeftToRight, NULL);
		//TODO: Provide parent widget which forces scroll bars in the scroll area, not using this*
		QLineEdit* pre_l_context = new QLineEdit(this);
		QLineEdit* predecessor = new QLineEdit(this);
		QLineEdit* pre_r_context = new QLineEdit(this);
		QLineEdit* successor = new QLineEdit(this);
		QLineEdit* condition = new QLineEdit(this);
		QLineEdit* probability = new QLineEdit(this);
		QPushButton* remove = new QPushButton("-", this);
		predecessor->setFixedWidth(120);
		condition->setFixedWidth(80);
		probability->setFixedWidth(60);
		l_system_production_layout->addWidget(pre_l_context);
		l_system_production_layout->addWidget(predecessor);
		l_system_production_layout->addWidget(pre_r_context);
		l_system_production_layout->addWidget(successor);
		l_system_production_layout->addWidget(condition);
		l_system_production_layout->addWidget(probability);
		l_system_production_layout->addWidget(remove);
		l_system_productions_list_layout->addLayout(l_system_production_layout);

		l_system_production_layouts[number_of_production_layouts] = l_system_production_layout;
		production_layout_ids[number_of_production_layouts] = layout_id;

		connect(remove, SIGNAL(clicked()), signal_map, SLOT(map()));
		signal_map->setMapping(remove, layout_id);
		connect(signal_map, SIGNAL(mapped(int)), this, SLOT(remove_production_text_fields(int)));
		number_of_production_layouts++;
		layout_id++;
	}
}

void AppWindow::shift_production_text_fields(int removed_index)
{
	for(int i = removed_index; i < number_of_production_layouts - 1; i++)
	{
		production_layout_ids[i] = production_layout_ids[i+1];
		l_system_production_layouts[i] = l_system_production_layouts[i+1];
	}
}

void AppWindow::remove_production_text_fields(int production_id)
{
	for(int i = 0; i < number_of_production_layouts; i++)
	{
		if(production_layout_ids[i] == production_id)
		{
			QBoxLayout* p_l = l_system_production_layouts[i];
			for(int j = 0; j < p_l->count(); j++)
			{
				p_l->itemAt(j)->widget()->setVisible(false);
			}
			l_system_productions_list_layout->removeItem(p_l);
			delete p_l;
			shift_production_text_fields(i);
			number_of_production_layouts--;
			break;
		}
	}
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
	QPushButton* camera_move_forwards = new QPushButton("^");
	QPushButton* camera_move_backwards = new QPushButton("v");
	target_move_grid->addWidget(camera_target_move_up, 0, 0);
	target_move_grid->addWidget(camera_target_move_down, 2, 0);
	target_move_grid->addWidget(camera_move_forwards, 0, 2);
	target_move_grid->addWidget(camera_move_backwards, 2, 2);

	camera_controls_layout->addLayout(camera_move_grid);
	camera_controls_layout->addLayout(target_move_grid);
	
	QPushButton* add_production_button = new QPushButton("+");
	layout->addWidget(add_production_button);

	QScrollArea* l_system_scroll_area = new QScrollArea(this);
	QGroupBox* l_system_production_box = new QGroupBox(l_system_scroll_area);
	l_system_productions_list_layout = new QVBoxLayout;
	l_system_production_box->setLayout(l_system_productions_list_layout);
	for(int i = 0; i < 8; i++)
	{
		add_production_text_fields();
	}
	//for(int i = 0; i < 4; i++) add_production_text_fields();
	l_system_productions_list_layout->setSizeConstraint(QLayout::SetFixedSize);
	l_system_production_box->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
	l_system_scroll_area->setWidget(l_system_production_box);
	l_system_scroll_area->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	layout->addWidget(l_system_scroll_area);

	connect(derive_button, SIGNAL(clicked()), this, SLOT(update_render()));
	connect(reset_button, SIGNAL(clicked()), this, SLOT(reset()));

	connect(camera_move_up, SIGNAL(clicked()), gl_widget, SLOT(move_camera_up()));
	connect(camera_move_down, SIGNAL(clicked()), gl_widget, SLOT(move_camera_down()));
	connect(camera_move_left, SIGNAL(clicked()), gl_widget, SLOT(move_camera_left()));
	connect(camera_move_right, SIGNAL(clicked()), gl_widget, SLOT(move_camera_right()));
	connect(camera_target_move_up, SIGNAL(clicked()), gl_widget, SLOT(move_camera_target_up()));
	connect(camera_target_move_down, SIGNAL(clicked()), gl_widget, SLOT(move_camera_target_down()));
	connect(camera_move_forwards, SIGNAL(clicked()), gl_widget, SLOT(move_camera_forwards()));
	connect(camera_move_backwards, SIGNAL(clicked()), gl_widget, SLOT(move_camera_backwards()));

	connect(add_production_button, SIGNAL(clicked()), this, SLOT(add_production_text_fields()));
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
