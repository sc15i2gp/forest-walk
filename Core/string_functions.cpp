#include "string_functions.h"

bool is_whitespace(char c)
{
	return c == '\n' || c == '\t' || c == ' ' || c == '\r';
}

bool is_minus(char c)
{
	return c == '-';
}

bool is_digit(char c)
{
	return c >= '0' && c <= '9';
}

bool is_point(char c)
{
	return c == '.';
}

bool is_letter(char c)
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool is_real_char(char c)
{
	return is_digit(c) || is_point(c) || is_letter(c);
}

char* operators[13] = 
{
        "+", "-", "*", "/", "^", ">", "<", ">=", "<=", "==", "!=", "&&", "||"
};

bool is_operator_char(char c)
{
        return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' ||
                c == '=' || c == '!' || c == '>' || c == '<' || c == '&' || c == '|';
}

bool is_second_operator_char(char c)
{
        return c == '&' || c == '=' || c == '|';
}

bool are_operators_equal(char* a, char* b)
{
        if(is_operator_char(a[0]) && is_operator_char(b[0]) && a[0] == b[0])
        {
                //First operator characters match
                if((is_second_operator_char(a[1]) && is_second_operator_char(b[1]) && a[1] == b[1]) || (!is_second_operator_char(a[1]) && !is_second_operator_char(b[1])))
                {
                        return true;
                }
                else return false;
        }
        else return false;
}

char* find_matching_operator(char* op)
{
        for(int i = 0; i < 13; i++) if(are_operators_equal(op, operators[i])) return operators[i];
        return NULL;
}

int length_of_operator(char* a)
{
        int length = 0;
        char* op = find_matching_operator(a);

        if(op) while(*op != 0) length++, op++;
        return length;
}

bool is_operator(char* op_str)
{
        char* op = find_matching_operator(op_str);
        return (op != NULL);
}

bool is_real(char* real_str)
{
	if(!is_real_char(*real_str) && !is_minus(*real_str)) return false;
	if(is_minus(*real_str) && !is_real_char(real_str[1])) return false;

	return true;
}

float read_real(char* value_str)
{
	int digit_count = 0;
	char digit_buffer[512] = {};
	char* c = value_str;

	if(is_minus(*c))
	{
		digit_buffer[digit_count] = *c;
		digit_count++;
		c++;
	}
	while(is_real_char(*c))
	{
		digit_buffer[digit_count] = *c;
		digit_count++;
		c++;
	}
	float value_real = atof(digit_buffer);
	return value_real;
}

int length_of_real(char* real_str)
{
	int length = 0;

	if(is_minus(*real_str)) length++;
	while(is_real_char(real_str[length])) length++;

	return length;
}

void overwrite_token_occurrences(char* to_overwrite, char token, char* to_copy)
{
	char* symbol = to_overwrite;
	int symbol_index = 0;

	while(*symbol != 0)
	{
		if(*symbol == token)
		{
			int altered_by = overwrite_string(to_overwrite, to_copy, symbol_index, 1);
			symbol += altered_by;
			symbol_index += altered_by;
		}
		symbol++;
		symbol_index++;
	}
}

//This function overwrites the input string, to_overwrite, at index overwrite_at with to_copy
//overwrite_length is used to describe the range of indices which should be replaced by to_copy
//if overwrite_length < length of to_copy, then to_overwrite will be shrunk to fit to_copy
//else if overwrite_length > length of to_copy, then to_overwrite will be expanded to fit to_copy
int overwrite_string(char* to_overwrite, char* to_copy, int overwrite_at, int overwrite_length)
{
	int length_to_copy = strlen(to_copy);
	int string_length_altered_by = 0;
	int to_move_from, to_move_to, size_to_move;

	if(overwrite_length < length_to_copy)
	{
		int size_diff = length_to_copy - overwrite_length;
		to_move_to = overwrite_at + size_diff;
		to_move_from = overwrite_at;
		size_to_move = strlen(to_overwrite) - overwrite_at + 1;
		string_length_altered_by += size_diff;
	}
	else if(overwrite_length > length_to_copy)
	{
		to_move_from = overwrite_at + overwrite_length;
		to_move_to = to_move_from - (overwrite_length - length_to_copy);
		size_to_move = strlen(to_overwrite) - to_move_from + 1;
		string_length_altered_by -= overwrite_length - length_to_copy;
	}

	if(overwrite_length != length_to_copy) memmove(to_overwrite + to_move_to, to_overwrite + to_move_from, size_to_move);

	memcpy(to_overwrite + overwrite_at, to_copy, length_to_copy);
	return string_length_altered_by;
}


//---------------------------------------------
//		Module string functions
//---------------------------------------------

int number_of_parameters(char* module)
{
	int module_length = length_of_module(module);

	if(module_length <= 1) return 0;
	else
	{
		int n = 1;
		for(int i = 0; i < module_length; i++)
		{
			if(module[i] == ',') n++;
		}
		return n;
	}
}

char* find_parameter(char* module, int parameter_number)
{
	while(*module != '(') module++; //Find beginning of parameters in string
	do module++; while(is_whitespace(*module)); //Move past whitespace between first parameter and '('
	for(int i = 0; i < parameter_number; i++)
	{//For each parameter before the desired parameter
		while(*module != ',') module++; //Find end of parameter delimited by ','
		do module++; while(is_whitespace(*module)); //Move past whitespace between next parameter and ','
	}

	return module;
}

float read_real_parameter_value(char* module, int param_number)
{
	char* param = find_parameter(module, param_number);
	return read_real(param);
}

int length_of_parameter(char* module, int param_number)
{
	char* param = find_parameter(module, param_number);
	int paren_depth = 0;
	char* param_end = param;
	
	//paren_depth needed to ensure that the length isn't incorrectly calculated by finding first instance of ')' in module string
	for(; (*param_end != ',' && *param_end != ')') || paren_depth > 0; param_end++)
	{
		if(*param_end == '(') paren_depth++;
		else if(*param_end == ')') paren_depth--;
	}
	
	return param_end - param;
}

bool is_parameter_expression(char* module, int param_number)
{
	char* param = find_parameter(module, param_number);
	int param_length = length_of_parameter(module, param_number);

	for(int i = 0; i < param_length; i++)
	{
		//If the parameter contains an operator, then the parameter is considered an expression
		if(is_operator(param+i)) return true; 
	}

	return false;
}

//Function takes two modules of the form: F(x_0,x_1...x_n) and F(r_0,r_1...r_n)
//x = variable symbol, r = real number string
void replace_parameter_tokens_with_values(char* variable_module, char* real_module, char* to_overwrite)
{
	char tokens[8] = {};
	char token_values[8][16] = {};
	int number_of_tokens = number_of_parameters(variable_module);

	//Find tokens from variable module
	for(int i = 0; i < number_of_tokens; i++)
	{
		char* param = find_parameter(variable_module, i);
		tokens[i] = *param;
	}

	//Find values of variable tokens from real_module
	for(int i = 0; i < number_of_tokens; i++)
	{
		char* param = find_parameter(real_module, i);
		int length = length_of_parameter(real_module, i);
		memcpy(token_values[i], param, length);
	}

	//Overwrite all occurences of all tokens in to_overwrite
	for(int i = 0; i < number_of_tokens; i++)
	{
		overwrite_token_occurrences(to_overwrite, tokens[i], token_values[i]);
	}
}

void write_into_parameter(char* module, int param_number, float f, int decimal_places)
{
	char to_copy[128];

	snprintf(to_copy, 128, "%.*f\0", decimal_places, f);

	char* param = find_parameter(module, param_number);
	int first = param - module;
	int last = first + length_of_parameter(module, param_number);
	int length = last - first;

	overwrite_string(module, to_copy, first, length);
}

int length_of_module(char* module)
{
	if(module[0] == 0) return 0;
	else if(module[1] == '(')
	{//If module is parameterised
		//Find closing parenthesis
		int paren_depth = 0;
		char* module_end = module + 2;

		for(; *module_end != ')' || paren_depth > 0; module_end++)
		{
			if(*module_end == '(') paren_depth++;
			if(*module_end == ')') paren_depth--;
		}

		return (module_end - module)+1;
	}
	else return 1;
}

char* find_next_module(char* module)
{
	module += length_of_module(module);
	return module;
}

int number_of_modules(char* module_string)
{
	int n = 0;
	char* module = module_string;

	while(*module != 0)
	{
		module = find_next_module(module);
		n++;
	}

	return n;
}

bool modules_match(char* m_0, char* m_1)
{
	if(*m_0 == *m_1)
	{
		return number_of_parameters(m_0) == number_of_parameters(m_1);
	}
	else return false;
}

bool module_strings_match(char* ms_0, char* ms_1)
{
	int ms_0_length = number_of_modules(ms_0);
	int ms_1_length = number_of_modules(ms_1);

	if(ms_0_length == ms_1_length)
	{
		for(int i = 0; i < ms_0_length; i++)
		{
			if(!modules_match(ms_0, ms_1)) return false;
			ms_0 = find_next_module(ms_0);
			ms_1 = find_next_module(ms_1);
		}
		return true;
	}
	else return false;
}

void print_module(char* module)
{
	printf("%.*s\n", length_of_module(module), module);
}



//-------------------------------------------------
//		Branched string functions
//-------------------------------------------------

//Finds ']' for corresponding '[' in branch
char* find_end_of_branch(char* branch)
{
	int state_depth = 1;

	while(state_depth > 0)
	{
		branch = find_next_module(branch);
		if(*branch == '[') state_depth++;
		else if(*branch == ']') state_depth--;
	}
	return branch;
}

//Clears symbols between '[' and ']'
//Copies cleared symbols from the first branch into dest if dest is not NULL
//Removes '[' and ']' if branch is now empty
void prune_branches(char* branched_str, char* dest)
{
	char* module = branched_str;
	int str_length = number_of_modules(branched_str);

	for(int i = 0; i < str_length; i++)
	{
		if(*module == '%')
		{
			int length_to_prune = 1;
			int branch_depth = 1;
			char* branch_end = find_next_module(module);

			while(*branch_end != 0)
			{
				if(*branch_end == '[') branch_depth++;
				else if(*branch_end == ']') branch_depth--;
				if(branch_depth == 0) break;
				length_to_prune += length_of_module(branch_end);
				branch_end = find_next_module(branch_end);
			}

			if(dest)
			{
				memcpy(dest, module+1, length_to_prune-1);
				dest[length_to_prune-1] = 0;
			}

			if(*(module-1) == '[')
			{
				module--;
				length_to_prune += 2;
			}

			overwrite_string(module, "", 0, length_to_prune);
		}

		module = find_next_module(module);
	}
}
