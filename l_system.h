#pragma once
#include <stdio.h>
#include <time.h>
#include "string_functions.h"
#include "expression_evaluator.h"
#include "char_queue.h"
#include "char_queue_stack.h"

#define CONTEXT_MAX 32
#define STRICT_MAX 16
#define SUCCESSOR_MAX 256
#define CONDITION_MAX 256

#define R_CONTEXT_MAX 64
struct production
{
	struct
	{
		char l_context[CONTEXT_MAX];
		char r_context[CONTEXT_MAX];
		char strict[STRICT_MAX];
	} predecessor;
	char successor[SUCCESSOR_MAX];
	char condition[CONDITION_MAX];
	float probability;
};

#define P_SET_MAX 32
struct l_system
{
	production p_set[P_SET_MAX] = {};
	int p_set_size = 0;
	int max_l_context_size = 0;
	int max_r_context_size = 0;
};

void print_production(production*);
void print_l_system(l_system*, const char* name);
void add_production(l_system*,char* pre, char* succ, char* cond = NULL, float = 1.0);
void add_production(l_system* l, char* pre_l_context, char* pre_strict, char* pre_r_context, char* successor, char* condition, float probability);
void add_production(l_system* l, char* pre_l_context, char* pre_strict, char* pre_r_context, char* successor, char* condition, char* probability);
void derive_str(l_system*,char*);
