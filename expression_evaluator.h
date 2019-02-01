#pragma once
#include "maths.h"
#include "float_stack.h"
#include "arithmetic_tokeniser.h"
#include "arithmetic_token_stack.h"
#include "string_functions.h"
#include "char_queue.h"

float evaluate_real_expression(char*);
bool evaluate_bool_expression(char*);
void convert_expression_to_rpn(char*, int);
float compute_rpn_string_result(char*, int);
