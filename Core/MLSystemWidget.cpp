#include "MLSystemWidget.h"

MLSystemWidget::MLSystemWidget(QWidget* parent): QWidget(parent)
{
	m_l_sys = create_m_l_system(32);
	add_production(&m_l_sys, "<T(x,r)>?(c)", "", "c == 0", 1.0f);
	add_production(&m_l_sys, "<T(x,r)>", "T(x,R)", "r >= R", 1.0f);
	add_global_parameter(&m_l_sys, 'R', "30.0");
	add_production(&m_l_sys, "<T(x,r)>?(c)", "T(x, r + 5.0)", NULL, 1.0f);
	add_str(&m_l_sys, 1, 15.0f, 1);
}

MLSystemWidget::~MLSystemWidget()
{
	destroy_m_l_system(&m_l_sys);
}

void MLSystemWidget::run_derivation()
{
	derive_set(&m_l_sys);
	print_str_set(&m_l_sys);
}
