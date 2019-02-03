#pragma once
#include <stdio.h>
#include <QWidget>
#include <QObject>
#include "l_system.h"
#include "RenderWidget.h"

#define MAX_STRING_SIZE 0x19000
class LSystemWidget: public QWidget
{
	Q_OBJECT
public:
	LSystemWidget(QWidget* parent, GLWidget* renderer);
	virtual ~LSystemWidget() {}

	void reset_current_string();
	void l_system_derivation();
	void load_l_system();
	int number_of_productions();
	char* predecessor(int);
	char* l_context(int);
	char* r_context(int);
	char* successor(int);
	char* condition(int);
	float probability(int);
	char* _axiom();
public slots:
	void reset();

signals:
	void l_system_loaded();

private:
	production* get_p(int);
	GLWidget* renderer;
	l_system l_system_in_use;
	char axiom[64] = {};
	char current_string[MAX_STRING_SIZE] = {};
};
