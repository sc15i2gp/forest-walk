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

public slots:
	void run_derivation();
	void init_system();

private:
	void push_str_set_to_chart_and_render();
	long int seed;
	int forest_length;
	ForestGLWidget* chart;
	m_l_system m_l_sys;
};
