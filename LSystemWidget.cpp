#include "LSystemWidget.h"
LSystemWidget::LSystemWidget(QWidget* parent, GLWidget* gl_widget): QWidget(parent)
{
	this->renderer = gl_widget;
	add_production(&l_system_in_use, "<A>", "BC");
	add_production(&l_system_in_use, "<B>", "ABC");
	add_production(&l_system_in_use, "<C>", "+F-F");
	strcpy(axiom, "FA");
	reset_current_string();
	//this->renderer->load_tree_model(current_string);
}

void LSystemWidget::reset_current_string()
{
	strcpy(current_string, axiom);
}

void LSystemWidget::reset()
{
	reset_current_string();
	this->renderer->load_tree_model(current_string);
}

void LSystemWidget::l_system_derivation()
{
	derive_str(&l_system_in_use, current_string);
	renderer->load_tree_model(&current_string[0]);
	parentWidget()->update();
}
