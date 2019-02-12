#pragma once
#include "maths.h"
#include "float_stack.h"
#include "arithmetic_tokeniser.h"
#include "arithmetic_token_stack.h"
#include "string_functions.h"
#include "char_queue.h"

void convert_expression_to_rpn(char*, int);
float compute_rpn_string_result(char*, int);
