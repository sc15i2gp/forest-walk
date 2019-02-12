#pragma once
#include <QBoxLayout>
#include <QWidget>
#include <QPushButton>
#include "RenderWidget.h"

class AppWindow: public QWidget
{
public:
	AppWindow(QWidget* parent);
	void init();
private:
	QBoxLayout* layout;
	GLWidget* gl_widget;
};
