#pragma once
#include <QBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include "ForestWidget.h"
#include "MLSystemWidget.h"

class AppWindow: public QWidget
{
public:
	AppWindow(QWidget* parent);
	void init();
private:
	QBoxLayout* layout;
	ForestGLWidget* gl_widget;
	MLSystemWidget* m_l_widget;
};
