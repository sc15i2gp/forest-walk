#include "arithmetic_tokeniser.h"

void arithmetic_tokeniser::set_current_token(token_type type, char* token_str, int token_length)
{
	current_token.type = type;
	current_token.str = token_str;
	current_token.length = token_length;
}

arithmetic_token arithmetic_tokeniser::get_next_rpn_token()
{
	if(current_token.type != token_begin)
	{
		//printf("Current ");
		//print_token(current_token);
		current_pos += current_token.length;
		while(is_whitespace(arithmetic_str[current_pos])) current_pos++;
		char* t = arithmetic_str + current_pos;
		if(current_pos < str_length)
		{
			if(is_real(t))
			{
				set_current_token(token_real, t, length_of_real(t));
			}
			else if(is_operator(t))
			{
				set_current_token(token_operator, t, length_of_operator(t));
			}
			else
			{
				set_current_token(token_invalid, t, 0);
			}
		}
		else set_current_token(token_end, t, 0);
	}
	else
	{
		char* t = arithmetic_str + current_pos;
		if(is_real(t))
		{
			set_current_token(token_real, t, length_of_real(t));
		}
		else
		{
			set_current_token(token_invalid, t, 0);
		}
	}
	return current_token;
}

arithmetic_token arithmetic_tokeniser::get_next_token()
{
	//Work out type of next token
	if(current_token.type != token_begin)
	{
		current_pos += current_token.length;
		while(is_whitespace(arithmetic_str[current_pos])) current_pos++;
		char* t = arithmetic_str + current_pos;
		if(current_pos < str_length)
		{
			if(is_minus(*t) && current_token.type == token_operator && is_real(t))
			{
				set_current_token(token_real, t, length_of_real(t));
			}
			else if((is_operator(t) && current_token.type == token_real) || !is_real(t))
			{
				set_current_token(token_operator, t, length_of_operator(t));
			}
			else if(is_real(t))
			{
				set_current_token(token_real, t, length_of_real(t));
			}
			else if(*t == '(')
			{
				set_current_token(token_open_paren, t, 1);
			}
			else if(*t == ')')
			{
				set_current_token(token_close_paren, t, 1);
			}
			else
			{
				set_current_token(token_invalid, t, 0);
			}
		}
		else set_current_token(token_end, t, 0);
	}
	else
	{
		while(is_whitespace(arithmetic_str[current_pos])) current_pos++;
		char* t = arithmetic_str + current_pos;
		if(is_real(t))
		{
			set_current_token(token_real, t, length_of_real(t));
		}
		else
		{
			set_current_token(token_invalid, t, 0);
		}
	}
	return current_token;
}

void print_token(arithmetic_token t)
{
	printf("token: %.*s %d\n", t.length, t.str, t.type);
}

