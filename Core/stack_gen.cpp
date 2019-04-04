#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//NOTE: Purpose of this module is to avoid the need for C++ templates because they are abominable and worthless

#define IGNORE_STACK_GEN
IGNORE_STACK_GEN

char* read_file_contents(const char* file_path)
{
	printf("Reading contents of file: %s\n", file_path);
	FILE* f = fopen(file_path, "r");
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, 0, SEEK_SET);
	char* file = (char*)malloc(size + 1);
	fread(file, 1, size, f);
	fclose(f);
	printf("Size: %db\n", size);
	file[size] = 0;
	return file;
}

struct size_list
{	
	char* sizes[8] = {};
	int size_count = 0;
};

void push_stack_code_to_file(FILE* f, char* type_string, char* state_count_string)
{
	fprintf(f, "struct %s_stack_%s\n{\n", type_string, state_count_string);
	
	fprintf(f, "%s states[%s] = {};\nint top = 0;\n\n", type_string, state_count_string);
	fprintf(f, "bool is_empty()\n{\nreturn this->top == 0;\n}\n\n");
	
	fprintf(f, "inline\n");
	fprintf(f, "%s* top_state()\n", type_string);
	fprintf(f, "{\n");
	fprintf(f, "return this->states + this->top;\n");
	fprintf(f, "}\n\n");
	
	fprintf(f, "%s read_state()\n", type_string);
	fprintf(f, "{\n");
	fprintf(f, "return *(this->top_state() - 1);\n");
	fprintf(f, "}\n\n");
	
	fprintf(f, "void push_state(%s state)\n", type_string);
	fprintf(f, "{\n");
	fprintf(f, "*(this->top_state()) = state;\n");
	fprintf(f, "this->top++;\n");
	fprintf(f, "}\n\n");

/*	
	fprintf(f, "void deep_push_state(%s state)\n", type_string);
	fprintf(f, "{\n");
	fprintf(f, "memcpy(this->top_state(), &state, sizeof(%s));\n");
	fprintf(f, "this->top++;\n");
	fprintf(f, "}\n\n");
*/
	fprintf(f, "%s pop_state()\n", type_string);
	fprintf(f, "{\n");
	fprintf(f, "this->top--;\n");
	fprintf(f, "return *this->top_state();\n");
	fprintf(f, "}\n");
	
	fprintf(f, "};\n");
}

bool gen_stack_code(char* type_string, size_list sizes)
{
	printf("Generating stack of type: %s\n\n", type_string);
	char stack_file[128] = {};
	sprintf(stack_file, "%s_stack.h", type_string);
	printf("File Name: %s\n", stack_file);
	FILE* f = fopen(stack_file, "w");

	char* default_count_string = "32";
	if(!f) return false;
	else
	{
		printf("type_string: %s length: %d\n", type_string, strlen(type_string));
		printf("number of sizes: %d\n", sizes.size_count);
		fprintf(f,"#pragma once\n");
		fprintf(f,"#define STACK_DEFAULT(type) type##_stack_32\n#define STACK_WITH_LIMIT(type, limit) type##_stack_##limit\n\n#define GET_3RD_ARG(a1, a2, a3, ...) a3\n#define CHOOSE_STACK_MACRO(...) GET_3RD_ARG(__VA_ARGS__, STACK_WITH_LIMIT, STACK_DEFAULT)\n\n#define STACK(...) CHOOSE_STACK_MACRO(__VA_ARGS__)(__VA_ARGS__)\n");
		for(int i = 0; i < sizes.size_count; i++)
		{
			char* state_count_string = sizes.sizes[i];
			if(!state_count_string)
			{
				state_count_string = default_count_string;
			}
			push_stack_code_to_file(f, type_string, state_count_string);
			fprintf(f, "\n");
		}
		fclose(f);
		return true;
	}
}

bool is_whitespace(char c)
{
	return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

//Only moves past single line comments
char* move_over_comment(char* at)
{
	for(; *at != '\n'; at++);
	at++;
	return at;
}

char* move_over_comment_block(char* at)
{
	for(; at[0] != '*' && at[1] != '/'; at++);
	at += 2;
	return at;
}

char* move_over_define(char* at)
{
	for(; at[0] != '\n'; at++);
	at++;
	return at;
}

char* move_to_next_token(char* at)
{
	at++;
	for(;;)
	{
		if(is_whitespace(*at)) at++;
		else if(at[0] == '/' && at[1] == '/') at = move_over_comment(at);
		else if(at[0] == '/' && at[1] == '*') at = move_over_comment_block(at);
		else if(at[0] == '#' && at[1] == 'd') at = move_over_define(at);
		else break;
	}
	return at;
}

void print_word(char* word)
{
	char str_buffer[512] = {};
	char* c = str_buffer;
	while(!is_whitespace(*word) && *word != 0 && *word != ')')
	{
		*c = *word;
		word++;
		c++;
	}
	printf("%s\n", str_buffer);
}

bool substr_equals(char* str, char* sub_str)
{
	int l = strlen(sub_str);
	for(int i = 0; i < l; i++) if(str[i] != sub_str[i]) return false;
	return true;
}

bool is_stack_macro(char* at)
{
	return substr_equals(at, "STACK(");
}

bool is_ignore_macro(char* at)
{
	return substr_equals(at, "IGNORE_STACK_GEN");
}

char* move_over_stack_macro(char* at)
{
	for(; *at != ')'; at++);
	at++;
	return at;
}

char* get_macro_type_param(char* at)
{
	int l = strlen("STACK(");
	for(int i = 0; i < l; i++) at++;
	return at;
}


bool is_numeric(char c)
{
	return (c >= '0' && c <= '9');
}

bool is_alphanumeric(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'a' && c <= 'z') || is_numeric(c);
}

char* get_macro_size_param(char* at)
{
	at = get_macro_type_param(at);
	while(is_alphanumeric(*at) || *at == '_') at++;
	if(*at != ',') return NULL;
	else
	{
		at++;
		while(is_whitespace(*at)) at++;
		return at;
	}
}

bool alphanumeric_strs_equal(char* s, char* t)
{
	while(is_alphanumeric(*s) && is_alphanumeric(*t))
	{
		if(*s != *t)
		{
			return false;
		}
		s++;
		t++;
	}
	return true;
}

bool is_str_in_str_list(char* str, char** str_list, int str_list_max)
{
	for(int i = 0; i < str_list_max; i++)
	{
		if(str_list[i] != NULL && str != NULL)
		{
			char* s = str;
			char* str_in_list = str_list[i];
			bool matches = alphanumeric_strs_equal(s, str_in_list);
			if(matches) return true;
		}
		else
		{
			if(str_list[i] == NULL && str == NULL) return true;
		}
	}
	return false;
}

int main(int argc, char** argv)
{
	printf("argc: %d argv: ", argc);
	for(int i = 0; i < argc; i++) printf("%s ", argv[i]);
	printf("\n");

	if(argc > 1)
	{
		for(int i = 1; i < argc; i++)
		{//For each file to look for macros in	
			bool should_gen_code = true;
			int stack_type_index = 0;
			char* stack_types[32] = {};
			size_list stack_sizes[32] = {};
			char* contents = read_file_contents(argv[i]);

			char* at = contents;
			while(*at != 0)
			{
				if(is_stack_macro(at))
				{
					char* type = get_macro_type_param(at);
					char* size = get_macro_size_param(at);
					for(int j = 0; j < stack_type_index; j++)
					{
						if(alphanumeric_strs_equal(stack_types[j], type))
						{
							if(!is_str_in_str_list(size, stack_sizes[j].sizes, stack_sizes[j].size_count))
							{
								stack_sizes[j].sizes[stack_sizes[j].size_count] = size;
								stack_sizes[j].size_count++;
							}
						}
					}
					if(!is_str_in_str_list(type, stack_types, 32))
					{
						stack_types[stack_type_index] = type;
						stack_sizes[stack_type_index].sizes[0] = size;
						stack_sizes[stack_type_index].size_count++;
						stack_type_index++;
					}
					at = move_over_stack_macro(at);
				}
				else if(is_ignore_macro(at))
				{
					should_gen_code = false;
					break;
				}
				at = move_to_next_token(at);
			}
			if(should_gen_code)
			{
				for(int j = 0; j < stack_type_index; j++)
				{
					char* _stack_type = stack_types[j];
					while(is_alphanumeric(*_stack_type) || *_stack_type == '_') _stack_type++;
					*_stack_type = 0;

					for(int k = 0; k < stack_sizes[j].size_count; k++)
					{
						char* _stack_size = stack_sizes[j].sizes[k];
						if(_stack_size)
						{
							while(is_numeric(*_stack_size)) _stack_size++;
							*_stack_size = 0;
						}
					}
					gen_stack_code(stack_types[j], stack_sizes[j]);
				}
			}
			free(contents);
		}
	}
	else
	{
		printf("No files provided for code generation\n");
		return 0;
	}
}
