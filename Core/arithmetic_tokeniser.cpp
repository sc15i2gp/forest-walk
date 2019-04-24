#include "arithmetic_tokeniser.h"

void arithmetic_tokeniser::set_current_token(token_type type, char* token_str, int token_length)
{
	current_token.type = type;
	current_token.str = token_str;
	current_token.length = token_length;
}

//NOTE: This algorithm assumes that, when an arithmetic string is transformed to postfix, then '-' used to denote a negative
//	real, then there is no whitespace between '-' and the number's digits
//	If '-' denotes the minus operator, then there is whitespace between it and the previous and next symbols
arithmetic_token arithmetic_tokeniser::get_next_postfix_token()
{
	current_pos += current_token.length;
	while(is_whitespace(arithmetic_str[current_pos])) current_pos++;
	char* t = arithmetic_str + current_pos; //Candidate for the next token
	
	if(current_pos >= str_length) set_current_token(token_end, t, 0);
	else switch(current_token.type)
	{//If the end of the arithmetic string hasn't been reached, determine what the next token, pointed to by t, is
		case token_begin:
		{

			if	(is_real(t)) 		set_current_token(token_real, t, length_of_real(t));
			else 				set_current_token(token_invalid, t, 0);

			break;

		}
		default:
		{

			if	(is_real(t)) 		set_current_token(token_real, t, length_of_real(t));
			else if	(is_operator(t)) 	set_current_token(token_operator, t, length_of_operator(t));
			else				set_current_token(token_invalid, t, 0);

			break;

		}
	}
	
	return current_token;
}

arithmetic_token arithmetic_tokeniser::get_next_infix_token()
{
	current_pos += current_token.length;
	while(is_whitespace(arithmetic_str[current_pos])) current_pos++;
	char* t = arithmetic_str + current_pos; //Candidate for the next token
	
	if(current_pos >= str_length) set_current_token(token_end, t, 0);
	else switch(current_token.type)
	{//If the end of the arithmetic string hasn't been reached, determine what the next token pointed to by t is
		case token_begin:
		case token_operator:
		{//If token begin or token operator then accept real or '('
			
			if	(is_real(t)) 				set_current_token(token_real, t, length_of_real(t));
			else if	(*t == '(') 				set_current_token(token_open_paren, t, 1);
			else 						set_current_token(token_invalid, t, 0);
			
			break;

		}
		case token_real:
		{//If token real then accept operator or ')'
			
			//second half of condition necessary to see if '-' is part of a negative real or not
			if	(is_operator(t) && !is_real(t)) 	set_current_token(token_operator, t, length_of_operator(t));
			else if	(*t == ')') 				set_current_token(token_close_paren, t, 1);
			else 						set_current_token(token_invalid, t, 0);
			
			break;

		}
		case token_open_paren:
		{
			if	(is_real(t))				set_current_token(token_real, t, length_of_real(t));
			else if	(is_operator(t) && !is_real(t))		set_current_token(token_operator, t, length_of_operator(t));
			else if	(*t == '(')				set_current_token(token_open_paren, t, 1);
			else if	(*t == ')')				set_current_token(token_close_paren, t, 1);
			else 						set_current_token(token_invalid, t, 0);
			break;

		}
		case token_close_paren:
		{//If token ')' then accept operator
			
			//second half of condition necessary to see if '-' is part of a negative real or not
			if	(is_operator(t) && !is_real(t)) 	set_current_token(token_operator, t, length_of_operator(t));
			else 						set_current_token(token_invalid, t, 0);
			
			break;

		}
	}
	
	return current_token;
}

void print_token(arithmetic_token t)
{
	printf("token: %.*s %d\n", t.length, t.str, t.type);
}

