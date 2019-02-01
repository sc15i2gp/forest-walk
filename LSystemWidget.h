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

public slots:
	void l_system_derivation();

private:
	GLWidget* renderer;
	l_system l_system_in_use;
	char axiom[64] = {};
	char current_string[MAX_STRING_SIZE] = {};
};
