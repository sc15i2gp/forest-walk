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

public slots:
	void run_derivation();
	void init_system();

private:
	long int seed;
	long int tree_seeds[8] = {};
	int forest_length;
	ForestGLWidget* gl_widget;
	forest_ecosystem ecosystem;
};
