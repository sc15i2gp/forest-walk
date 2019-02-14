#pragma once
#include <stdio.h>
#include <QWidget>
#include <QObject>
#include "l_system.h"
#include "RenderWidget.h"

#define MAX_STRING_SIZE 0x400000
class LSystemWidget: public QWidget
{
	Q_OBJECT
public:
	LSystemWidget(QWidget* parent, GLWidget* renderer);
	virtual ~LSystemWidget() { free(current_string); }

	void reset_current_string();
	void l_system_derivation();
	void load(const char* = NULL);
	void save(const char*);
	int number_of_productions();
	char* predecessor(int);
	char* l_context(int);
	char* r_context(int);
	char* successor(int);
	char* condition(int);
	float probability(int);
	char* _axiom();
	void add_production_to_set(char*,char*,char*,char*,char*,char*);
	void clear_production_set();
	void clear_global_parameters();
	void _add_global_parameter(char, char*);
	void print();
	void set_axiom(const char*);
	char* current_str();
	
public slots:
	void reset();

signals:
	void l_system_loaded();

private:
	production* get_p(int);
	GLWidget* renderer;
	l_system l_system_in_use;
	char axiom[64] = {};
	char* current_string;
};
