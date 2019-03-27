#pragma once
#include <QObject>
#include <QWidget>
#include <time.h>
#include <cstdlib>
#include "ForestWidget.h"
#include "multiset_l_system.h"
#include "string_functions.h"

class MLSystemWidget: public QWidget
{
	Q_OBJECT
public:
	MLSystemWidget(QWidget*,ForestGLWidget*);
	~MLSystemWidget();
	void set_succession(bool);
	void set_propagation(bool);

public slots:
	void run_derivation();
	void init_system();

private:
	void load_productions_and_parameters();
	void push_str_set_to_chart_and_render();
	long int seed;
	long int tree_seeds[8] = {};
	int forest_length;
	ForestGLWidget* chart;
	m_l_system m_l_sys;
	bool should_propagate;
	bool should_succeed;
};
