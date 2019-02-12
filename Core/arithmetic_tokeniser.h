#pragma once
#include "string_functions.h"
enum token_type
{
	token_begin,
	token_operator,
	token_real,
	token_open_paren,
	token_close_paren,
	token_end,
	token_invalid
};

struct arithmetic_token
{
	token_type type;
	char* str;
	int length;
};

struct arithmetic_tokeniser
{
	char* arithmetic_str;
	int str_length;
	int current_pos;
	arithmetic_token current_token; 
	
	void set_current_token(token_type, char*, int);
	arithmetic_token get_next_token();
	arithmetic_token get_next_rpn_token();
};

void print_token(arithmetic_token t);
