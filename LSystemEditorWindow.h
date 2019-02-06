#pragma once
#include <QBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QScrollArea>
#include <QSignalMapper>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include "maths.h"
#include "string_functions.h"
#include "mesh.h"
#include "turtle.h"
#include "l_system.h"
#include "LSystemWidget.h"
#include "RenderWidget.h"

class AppWindow: public QWidget
{
	Q_OBJECT
public:
	AppWindow(QWidget* parent);
	void init();
	~AppWindow();
public slots:
	void reset();
	void change_axiom();
	void reset_production_list();
	void remove_production_text_fields(int);
	void add_production_text_fields();
	void update_render();
	void add_production_list();
	void enable_save();
	void open_save_dialog();
	void open_load_dialog();

private:
	void shift_production_text_fields(int);
	void remove_production_by_index(int);
	void clear_production_layouts();
	int number_of_production_layouts = 0;
	QBoxLayout* l_system_production_layouts[16];
	int production_layout_ids[16] = {};
	QSignalMapper* signal_map;
	GLWidget* gl_widget;
	QBoxLayout* layout;
	QVBoxLayout* l_system_productions_list_layout;
	LSystemWidget* l_system_widget;
	QLineEdit* axiom_edit;
	QLabel* current_str_label;
	QPushButton* save_button;
};
