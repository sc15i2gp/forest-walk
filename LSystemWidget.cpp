#include "LSystemWidget.h"
LSystemWidget::LSystemWidget(QWidget* parent, GLWidget* gl_widget): QWidget(parent)
{
	this->renderer = gl_widget;
}

void LSystemWidget::clear_production_set()
{
	this->l_system_in_use.p_set_size = 0;
}

int LSystemWidget::number_of_productions()
{
	return l_system_in_use.p_set_size;
}

production* LSystemWidget::get_p(int p_index)
{
	return l_system_in_use.p_set+p_index;
}	

void LSystemWidget::add_production_to_set(char* l_context, char* strict, char* r_context, char* successor, char* condition, char* probability)
{
	add_production(&l_system_in_use, l_context, strict, r_context, successor, condition, probability);
}

void LSystemWidget::print()
{
	print_l_system(&l_system_in_use, "Widget");
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

char* LSystemWidget::current_str()
{
	return current_string;
}


void LSystemWidget::load(const char* path)
{
	if(!path)
	{
		//This path will only be traversed when the program is starting up
		add_production(&l_system_in_use, "<I(d)>", "[+(60)F(0.5,0.125,1.25)][-(60)F(0.5,0.125,1.25)]F(1.0,0.25,1.25)I(d-1)", "d>0");
		set_axiom("F(1.0,0.5,1.25)I(3)");
		reset_current_string();
		emit l_system_loaded();
	}
	else if(strlen(path) > 0)
	{
		load_l_system(&l_system_in_use, path, axiom);
		emit l_system_loaded();
	}
}

void LSystemWidget::save(const char* path)
{
	if(strlen(path) > 0)
	{
		//TODO: Move this to l_system.cpp
		save_l_system(&l_system_in_use, path, axiom);
	}
}

void LSystemWidget::set_axiom(const char* new_axiom)
{
	strcpy(axiom, new_axiom);
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
