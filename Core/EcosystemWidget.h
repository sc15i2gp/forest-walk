#pragma once
#include <QObject>
#include <QWidget>
#include <time.h>
#include <cstdlib>
#include "ForestWidget.h"
#include "ecosystem.h"
#include "string_functions.h"

class EcosystemWidget: public QWidget
{
	Q_OBJECT
public:
	EcosystemWidget(QWidget*,ForestGLWidget*);
	virtual ~EcosystemWidget();
	void set_self_thinning(bool);
	void set_succession(bool);
	void set_propagation(bool);
	void set_plastochron_count(int);
	void populate_initial_ecosystem();

public slots:
	void iterate_plastochron();
	void init_ecosystem();

private:
	int plastochron_count;
	long int seed;
	long int tree_seeds[8] = {};
	ForestGLWidget* gl_widget;
	forest_ecosystem ecosystem;
};
