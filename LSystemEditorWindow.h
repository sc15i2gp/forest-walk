#pragma once
#include <QBoxLayout>
#include <QPushButton>
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
	void init_GLwidget();
	~AppWindow();
public slots:
	void update_render();
	void reset();
private:
	QPushButton* derive_button;
	QPushButton* reset_button;
	GLWidget* gl_widget;
	QBoxLayout* layout;
	LSystemWidget* l_system_widget;
};
