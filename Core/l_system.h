#pragma once
#include <stdio.h>
#include "string_functions.h"
#include "expression_evaluator.h"
#include "char_queue.h"
#include "char_queue_stack.h"

#define CONTEXT_MAX 32
#define STRICT_MAX 16
#define SUCCESSOR_MAX 256
#define CONDITION_MAX 256

#define R_CONTEXT_MAX 64

#define MAX_DERIVED_OUTPUT_SIZE 0x800000

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

//Parameter map symbol -> real string
struct param_mapping
{
	char symbol;
	char real_str[31];
};

#define P_SET_MAX 32
#define PARAM_MAP_MAX 8
struct l_system
{
	production p_set[P_SET_MAX];
	int p_set_size;
	int max_l_context_size;
	int max_r_context_size;
	param_mapping param_map[PARAM_MAP_MAX];
};

//Change l_system properties
void add_production(l_system*,char* pre, char* succ, char* cond = NULL, float = 1.0);
void add_production(l_system* l, char* pre_l_context, char* pre_strict, char* pre_r_context, char* successor, char* condition, float probability);
void add_production(l_system* l, char* pre_l_context, char* pre_strict, char* pre_r_context, char* successor, char* condition, char* probability);
void add_global_parameter(l_system*, char token, char* initial_value = NULL);
void set_global_parameter(l_system*, char token, char* value);
void set_production_probability(l_system*, int production, float probability);

//Derive
void derive_str(l_system*,char*);

//File I/O
void load_l_system(l_system*,const char* path, char* axiom_buffer);
void save_l_system(l_system*,const char* path, char* axiom_buffer);

//Print
void print_production(production*);
void print_l_system(l_system*, const char* name);
