#pragma once
#include <QObject>
#include <QBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QCheckBox>
#include <QPixmap>
#include "ForestWidget.h"
#include "MLSystemWidget.h"

class AppWindow: public QWidget
{
	Q_OBJECT
public:
	AppWindow(QWidget* parent);
	void init();

public slots:
	void show_domination_checked(int);
	void show_old_age_checked(int);
	void show_view_range_checked(int);
	void show_grid_checked(int);
	void propagation_checked(int);
	void succession_checked(int);

private:
	QBoxLayout* layout;
	ForestGLWidget* gl_widget;
	MLSystemWidget* m_l_widget;
};
