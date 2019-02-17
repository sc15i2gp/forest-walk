#pragma once
#include <QBoxLayout>
#include <QWidget>
#include <QPushButton>
#include "ChartWidget.h"
#include "MLSystemWidget.h"

class AppWindow: public QWidget
{
public:
	AppWindow(QWidget* parent);
	void init();
private:
	QBoxLayout* layout;
	ChartGLWidget* gl_widget;
	MLSystemWidget* m_l_widget;
};