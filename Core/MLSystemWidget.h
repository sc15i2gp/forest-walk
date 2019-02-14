#pragma once
#include <QObject>
#include <QWidget>
#include "multiset_l_system.h"

class MLSystemWidget: public QWidget
{
	Q_OBJECT
public:
	MLSystemWidget(QWidget*);
	~MLSystemWidget();

public slots:
	void run_derivation();

private:
	m_l_system m_l_sys;
};
