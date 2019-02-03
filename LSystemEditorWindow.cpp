#include "LSystemEditorWindow.h"

AppWindow::AppWindow(QWidget* parent): QWidget(parent)
{
	layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	signal_map = new QSignalMapper(this);
}

void AppWindow::reset_production_list()
{
	l_system_widget->clear_production_set();
	for(int i = 0; i < number_of_production_layouts; i++)
	{
		QBoxLayout* p_l = l_system_production_layouts[i];
		char l_context[64]; 
		char strict[64];
		char r_context[64];
		char successor[64];
		char condition[64];
		char probability[8];
		strcpy(l_context, ((QLineEdit*)p_l->itemAt(0)->widget())->text().toStdString().c_str());
		strcpy(strict, ((QLineEdit*)p_l->itemAt(2)->widget())->text().toStdString().c_str());
		strcpy(r_context, ((QLineEdit*)p_l->itemAt(4)->widget())->text().toStdString().c_str());
		strcpy(successor, ((QLineEdit*)p_l->itemAt(6)->widget())->text().toStdString().c_str());
		strcpy(condition, ((QLineEdit*)p_l->itemAt(8)->widget())->text().toStdString().c_str());
		strcpy(probability, ((QLineEdit*)p_l->itemAt(10)->widget())->text().toStdString().c_str());
		l_system_widget->add_production_to_set(l_context, strict, r_context, successor, condition, probability);
	}
	for(int i = 0; i < number_of_production_layouts; i++)
	{
		remove_production_by_index(i);
		l_system_production_layouts[i] = NULL;
		production_layout_ids[i] = 0;
	}
	number_of_production_layouts = 0;
	add_production_list();
	l_system_widget->reset();
}

void AppWindow::add_production_text_fields()
{
	static int layout_id = 1;
	if(number_of_production_layouts < 16)
	{
		QBoxLayout* l_system_production_layout = new QBoxLayout(QBoxLayout::LeftToRight, NULL);
		QLineEdit* pre_l_context = new QLineEdit(this);
		QLineEdit* predecessor = new QLineEdit(this);
		QLineEdit* pre_r_context = new QLineEdit(this);
		QLineEdit* successor = new QLineEdit(this);
		QLineEdit* condition = new QLineEdit(this);
		QLineEdit* probability = new QLineEdit(this);
		probability->setText("1.000");
		QPushButton* remove = new QPushButton("-", this);
		remove->setMaximumWidth(55);
		remove->setMinimumWidth(20);
		predecessor->setFixedWidth(120);
		condition->setFixedWidth(80);
		probability->setFixedWidth(60);
		l_system_production_layout->addWidget(pre_l_context);
		l_system_production_layout->addWidget(new QLabel("<"));
		l_system_production_layout->addWidget(predecessor);
		l_system_production_layout->addWidget(new QLabel(">"));
		l_system_production_layout->addWidget(pre_r_context);
		l_system_production_layout->addWidget(new QLabel("-->"));
		l_system_production_layout->addWidget(successor);
		l_system_production_layout->addWidget(new QLabel(":"));
		l_system_production_layout->addWidget(condition);
		l_system_production_layout->addWidget(new QLabel("|"));
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
	for(int i = removed_index; i < number_of_production_layouts; i++)
	{
		production_layout_ids[i] = production_layout_ids[i+1];
		l_system_production_layouts[i] = l_system_production_layouts[i+1];
	}
}

void AppWindow::change_axiom()
{
	const char* new_axiom = axiom_edit->text().toStdString().c_str();
	l_system_widget->set_axiom(new_axiom);
	l_system_widget->reset();
}

void AppWindow::remove_production_by_index(int i)
{
	QBoxLayout* p_l = l_system_production_layouts[i];
	for(int j = 0; j < p_l->count(); j++)
	{
		p_l->itemAt(j)->widget()->setVisible(false);
	}
	l_system_productions_list_layout->removeItem(p_l);
	delete p_l;
}

void AppWindow::remove_production_text_fields(int production_id)
{
	for(int i = 0; i < number_of_production_layouts; i++)
	{
		if(production_layout_ids[i] == production_id)
		{
			remove_production_by_index(i);
			number_of_production_layouts--;
			shift_production_text_fields(i);
			break;
		}
	}
}

void AppWindow::add_production_list()
{
	char* axiom = l_system_widget->_axiom();
	axiom_edit->setText(axiom);
	
	for(int i = 0; i < l_system_widget->number_of_productions(); i++)
	{
		add_production_text_fields();
		QBoxLayout* p_l = l_system_production_layouts[i];
		QLineEdit* e = (QLineEdit*)p_l->itemAt(0)->widget();
		e->setText(l_system_widget->l_context(i));
		e = (QLineEdit*)p_l->itemAt(2)->widget();
		e->setText(l_system_widget->predecessor(i));
		e = (QLineEdit*)p_l->itemAt(4)->widget();
		e->setText(l_system_widget->r_context(i));
		e = (QLineEdit*)p_l->itemAt(6)->widget();
		e->setText(l_system_widget->successor(i));
		e = (QLineEdit*)p_l->itemAt(8)->widget();
		e->setText(l_system_widget->condition(i));
		e = (QLineEdit*)p_l->itemAt(10)->widget();
		char prob[8] = {};
		snprintf(prob, 8, "%.3f", l_system_widget->probability(i));
		e->setText(prob);
	}
}

void AppWindow::init()
{
	gl_widget = new GLWidget(this);
	QPushButton* derive_button = new QPushButton("Derive", this);
	QPushButton* reset_button  = new QPushButton("Reset", this);
	QBoxLayout* camera_controls_layout = new QBoxLayout(QBoxLayout::LeftToRight, NULL);
	l_system_widget = new LSystemWidget(this, gl_widget);
	layout->addWidget(gl_widget);
	gl_widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
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
	
	QBoxLayout* axiom_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	layout->addLayout(axiom_layout);
	QLabel* axiom_label = new QLabel("Axiom:", this);
	axiom_layout->addWidget(axiom_label);
	axiom_edit = new QLineEdit(this);
	axiom_layout->addWidget(axiom_edit);
	QPushButton* add_production_button = new QPushButton("+");
	layout->addWidget(add_production_button);
	QPushButton* reload_productions_button = new QPushButton("Reload productions");
	layout->addWidget(reload_productions_button);

	QScrollArea* l_system_scroll_area = new QScrollArea(this);
	QGroupBox* l_system_production_box = new QGroupBox(l_system_scroll_area);
	l_system_productions_list_layout = new QVBoxLayout;
	l_system_production_box->setLayout(l_system_productions_list_layout);
	l_system_productions_list_layout->setSizeConstraint(QLayout::SetFixedSize);
	l_system_production_box->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	l_system_scroll_area->setWidget(l_system_production_box);
	l_system_scroll_area->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
	l_system_scroll_area->setMinimumHeight(150);
	layout->addWidget(l_system_scroll_area);

	connect(derive_button, SIGNAL(clicked()), this, SLOT(update_render()));
	connect(reset_button, SIGNAL(clicked()), l_system_widget, SLOT(reset()));

	connect(camera_move_up, SIGNAL(clicked()), gl_widget, SLOT(move_camera_up()));
	connect(camera_move_down, SIGNAL(clicked()), gl_widget, SLOT(move_camera_down()));
	connect(camera_move_left, SIGNAL(clicked()), gl_widget, SLOT(move_camera_left()));
	connect(camera_move_right, SIGNAL(clicked()), gl_widget, SLOT(move_camera_right()));
	connect(camera_target_move_up, SIGNAL(clicked()), gl_widget, SLOT(move_camera_target_up()));
	connect(camera_target_move_down, SIGNAL(clicked()), gl_widget, SLOT(move_camera_target_down()));
	connect(camera_move_forwards, SIGNAL(clicked()), gl_widget, SLOT(move_camera_forwards()));
	connect(camera_move_backwards, SIGNAL(clicked()), gl_widget, SLOT(move_camera_backwards()));

	connect(add_production_button, SIGNAL(clicked()), this, SLOT(add_production_text_fields()));
	connect(reload_productions_button, SIGNAL(clicked()), this, SLOT(reset_production_list()));

	connect(gl_widget, SIGNAL(initialised()), this->l_system_widget, SLOT(reset()));
	connect(this->l_system_widget, SIGNAL(l_system_loaded()), this, SLOT(add_production_list()));
	connect(axiom_edit, SIGNAL(editingFinished()), this, SLOT(change_axiom()));
	l_system_widget->load_l_system();
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
