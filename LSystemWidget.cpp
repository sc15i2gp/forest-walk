#include "LSystemWidget.h"
LSystemWidget::LSystemWidget(QWidget* parent, GLWidget* gl_widget): QWidget(parent)
{
	this->renderer = gl_widget;
}

int LSystemWidget::number_of_productions()
{
	return l_system_in_use.p_set_size;
}

production* LSystemWidget::get_p(int p_index)
{
	return l_system_in_use.p_set+p_index;
}	
char* LSystemWidget::predecessor(int p_index)
{
	return get_p(p_index)->predecessor.strict;
}

char* LSystemWidget::l_context(int p_index)
{
	return get_p(p_index)->predecessor.l_context;
}

char* LSystemWidget::r_context(int p_index)
{
	return get_p(p_index)->predecessor.r_context;
}

char* LSystemWidget::successor(int p_index)
{
	return  get_p(p_index)->successor;
}

char* LSystemWidget::condition(int p_index)
{
	return get_p(p_index)->condition;
}

float LSystemWidget::probability(int p_index)
{
	return get_p(p_index)->probability;
}

char* LSystemWidget::_axiom()
{
	return axiom;
}

void LSystemWidget::load_l_system()
{
	add_production(&l_system_in_use, "<A>", "BC");
	add_production(&l_system_in_use, "<B>", "ABC");
	add_production(&l_system_in_use, "<C>", "+F-F");
	strcpy(axiom, "FA");
	reset_current_string();
	emit l_system_loaded();
}

void LSystemWidget::reset_current_string()
{
	strcpy(current_string, axiom);
}

void LSystemWidget::reset()
{
	reset_current_string();
	this->renderer->load_tree_model(current_string);
	this->renderer->update();
}

void LSystemWidget::l_system_derivation()
{
	derive_str(&l_system_in_use, current_string);
	renderer->load_tree_model(&current_string[0]);
	parentWidget()->update();
}
