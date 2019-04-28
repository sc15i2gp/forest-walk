#include "LSystemEditorWindow.h"

AppWindow::AppWindow(QWidget* parent): QWidget(parent)
{
	layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	signal_map = new QSignalMapper(this);
}

void AppWindow::reset()
{
	l_system_widget->reset();
	current_str_label->setText(l_system_widget->current_str());
}

void AppWindow::clear_production_layouts()
{
	for(int i = 0; i < number_of_production_layouts; i++)
	{
		remove_production_by_index(i);
		l_system_production_layouts[i] = NULL;
		production_layout_ids[i] = 0;
	}
	number_of_production_layouts = 0;
}

void AppWindow::reset_production_list()
{
	l_system_widget->clear_production_set();
	for(int i = 0; i < number_of_production_layouts; i++)
	{
		QBoxLayout* p_l = l_system_production_layouts[i];
		char l_context[256]; 
		char strict[256];
		char r_context[256];
		char successor[256];
		char condition[256];
		char probability[16];
		strcpy(l_context, ((QLineEdit*)p_l->itemAt(0)->widget())->text().toStdString().c_str());
		strcpy(strict, ((QLineEdit*)p_l->itemAt(2)->widget())->text().toStdString().c_str());
		strcpy(r_context, ((QLineEdit*)p_l->itemAt(4)->widget())->text().toStdString().c_str());
		strcpy(successor, ((QLineEdit*)p_l->itemAt(6)->widget())->text().toStdString().c_str());
		strcpy(condition, ((QLineEdit*)p_l->itemAt(8)->widget())->text().toStdString().c_str());
		strcpy(probability, ((QLineEdit*)p_l->itemAt(10)->widget())->text().toStdString().c_str());
		l_system_widget->add_production_to_set(l_context, strict, r_context, successor, condition, probability);
	}
	clear_production_layouts();
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
		successor->setFixedWidth(350);
		remove->setMaximumWidth(50);
		remove->setMinimumWidth(20);
		predecessor->setFixedWidth(80);
		pre_l_context->setFixedWidth(60);
		pre_r_context->setFixedWidth(60);
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
		l_system_production_layout->setSizeConstraint(QLayout::SetMinimumSize);
		l_system_productions_list_layout->addLayout(l_system_production_layout);

		l_system_production_layouts[number_of_production_layouts] = l_system_production_layout;
		production_layout_ids[number_of_production_layouts] = layout_id;

		connect(remove, SIGNAL(clicked()), signal_map, SLOT(map()));
		signal_map->setMapping(remove, layout_id);
		connect(signal_map, SIGNAL(mapped(int)), this, SLOT(remove_production_text_fields(int)));
		connect(successor, SIGNAL(textEdited(const QString&)), this, SLOT(enable_save()));
		connect(pre_l_context, SIGNAL(textEdited(const QString&)), this, SLOT(enable_save()));
		connect(pre_r_context, SIGNAL(textEdited(const QString&)), this, SLOT(enable_save()));
		connect(predecessor, SIGNAL(textEdited(const QString&)), this, SLOT(enable_save()));
		connect(condition, SIGNAL(textEdited(const QString&)), this, SLOT(enable_save()));
		connect(probability, SIGNAL(textEdited(const QString&)), this, SLOT(enable_save()));
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

void copy_std_string_to_array(std::string str, char* c)
{
	//NOTE: This element-wise copy is necessary because, if axiom_edit's text > 16 chars, then c_str() ptr contains garbage for some reason
	for(std::string::iterator it = str.begin(); it != str.end(); it++)
	{
		*c = *it;
		c++;
	}
}

void AppWindow::change_axiom()
{
	const char* new_axiom = axiom_edit->text().toStdString().c_str();
	std::string str = axiom_edit->text().toStdString();
	char a[64] = {};
	char* b = a;
	copy_std_string_to_array(str, b);
	l_system_widget->set_axiom(a);
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
	clear_production_layouts();
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
	current_str_label->setText(l_system_widget->current_str());
}

void AppWindow::reload_global_parameters()
{
	l_system_widget->clear_global_parameters();
	for(int i = 0; i < 8; i++)
	{
		QLineEdit* param_symbol = param_symbols[i];
		QLineEdit* param_value = param_values[i];
		char symbol = param_symbol->text().toStdString()[0];
		const char* value = param_value->text().toStdString().c_str();
		char _value[16];
		copy_std_string_to_array(param_value->text().toStdString(), _value);
		if(symbol > 0) l_system_widget->_add_global_parameter(symbol, _value);
	}
}

void AppWindow::init()
{
	gl_widget = new GLWidget(this);
	QPushButton* derive_button = new QPushButton("Derive", this);
	QPushButton* reset_button  = new QPushButton("Reset", this);
	QPushButton* capture_button = new QPushButton("Capture", this);

	QBoxLayout* simulation_interact_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	QBoxLayout* camera_controls_layout = new QBoxLayout(QBoxLayout::LeftToRight, NULL);
	l_system_widget = new LSystemWidget(this, gl_widget);
	layout->addWidget(gl_widget);
	gl_widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
	simulation_interact_layout->addWidget(derive_button);
	simulation_interact_layout->addWidget(reset_button);
	simulation_interact_layout->addWidget(capture_button);
	layout->addLayout(simulation_interact_layout);

	QBoxLayout* axiom_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	layout->addLayout(axiom_layout);
	QLabel* axiom_label = new QLabel("Axiom:", this);
	axiom_layout->addWidget(axiom_label);
	axiom_edit = new QLineEdit(this);
	axiom_layout->addWidget(axiom_edit);

	current_str_label = new QLabel("N/A");
	//layout->addWidget(current_str_label);
	current_str_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum);
	current_str_label->setWordWrap(true);

	QBoxLayout* param_layout_0 = new QBoxLayout(QBoxLayout::LeftToRight);
	QBoxLayout* param_layout_1 = new QBoxLayout(QBoxLayout::LeftToRight);
	QBoxLayout* p_layouts[2] = {param_layout_0, param_layout_1};
	for(int i = 0; i < 2; i++)
	{
		QLabel* pipe_label = new QLabel("|", this);
		pipe_label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
		p_layouts[i]->addWidget(pipe_label);
		for(int j = 0; j < 4; j++)
		{
			pipe_label = new QLabel("|", this);
			pipe_label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
			QLineEdit* param_symbol = new QLineEdit(this);
			QLineEdit* param_value = new QLineEdit(this);
			param_value->setText("0.000");
			p_layouts[i]->addWidget(param_symbol);
			p_layouts[i]->addWidget(new QLabel(":",this));
			p_layouts[i]->addWidget(param_value);
			p_layouts[i]->addWidget(pipe_label);
			param_symbols[4*i+j] = param_symbol;
			param_values[4*i+j] = param_value;
			connect(param_symbol, SIGNAL(editingFinished()), this, SLOT(reload_global_parameters()));
			connect(param_value, SIGNAL(editingFinished()), this, SLOT(reload_global_parameters()));
		}
		layout->addLayout(p_layouts[i]);
	}
	QBoxLayout* production_interact_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	QPushButton* add_production_button = new QPushButton("Add production");
	production_interact_layout->addWidget(add_production_button);
	QPushButton* reload_productions_button = new QPushButton("Reload productions");
	production_interact_layout->addWidget(reload_productions_button);
	layout->addLayout(production_interact_layout);

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

	QBoxLayout* file_io_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	save_button = new QPushButton("Save L-system");
	QPushButton* load_button = new QPushButton("Load L-system");
	save_button->setEnabled(false);
	file_io_layout->addWidget(save_button);
	file_io_layout->addWidget(load_button);
	layout->addLayout(file_io_layout);
	connect(derive_button, SIGNAL(clicked()), this, SLOT(update_render()));
	connect(reset_button, SIGNAL(clicked()), this, SLOT(reset()));
	connect(capture_button, SIGNAL(clicked()), this->gl_widget, SLOT(capture()));

	connect(add_production_button, SIGNAL(clicked()), this, SLOT(add_production_text_fields()));
	connect(reload_productions_button, SIGNAL(clicked()), this, SLOT(reset_production_list()));

	connect(gl_widget, SIGNAL(initialised()), this->l_system_widget, SLOT(reset()));
	connect(this->l_system_widget, SIGNAL(l_system_loaded()), this, SLOT(add_production_list()));
	connect(axiom_edit, SIGNAL(editingFinished()), this, SLOT(change_axiom()));
	connect(save_button, SIGNAL(clicked()), this, SLOT(open_save_dialog()));
	connect(load_button, SIGNAL(clicked()), this, SLOT(open_load_dialog()));
	l_system_widget->load();
}

void AppWindow::open_save_dialog()
{
	QString file = QFileDialog::getSaveFileName(this, 
			tr("Save L-system"), ".lsys",
			tr("L-system (*.lsys);;All Files(*)"));
	l_system_widget->save(file.toStdString().c_str());
}

void AppWindow::open_load_dialog()
{
	QString file = QFileDialog::getOpenFileName(this, 
			tr("Load L-system"), "",
			tr("L-system (*.lsys);;All Files(*)"));
	l_system_widget->load(file.toStdString().c_str());
}

void AppWindow::enable_save()
{
	save_button->setEnabled(true);
}

AppWindow::~AppWindow()
{
	delete layout;
}

void AppWindow::update_render()
{
	l_system_widget->l_system_derivation();
	gl_widget->update();
	current_str_label->setText(l_system_widget->current_str());
}
