#pragma once
#include <cstdlib>
#include <cstring>
#include <cstdio>

//General string functions
bool is_whitespace(char);

bool is_operator(char*); 
bool is_real(char*); 
bool is_minus(char);
int length_of_real(char*); 
int length_of_operator(char*);
bool are_operators_equal(char*,char*); 

float read_real(char*);

int overwrite_string(char*,char*,int,int); 
void overwrite_token_occurrences(char*,char,char*); 

//Module string functions
void print_module(char* module);

int number_of_parameters(char*); 
char* find_parameter(char*, int = 0);
float read_real_parameter_value(char*, int = 0); 
bool is_parameter_expression(char*, int = 0); 
void write_into_parameter(char*,int,float, int = 5); 
void replace_parameter_tokens_with_values(char*,char*,char*); 
int length_of_parameter(char* module, int param_index); //Does not include padding whitespace before actual parameter string but does include whitespace padding up to , or )

int length_of_module(char*);
char* find_next_module(char*);
int number_of_modules(char*); 
bool modules_match(char*,char*); 
bool module_strings_match(char*,char*); //Used for comparing context strings

//Branched string functions
char* find_end_of_branch(char*); 
void prune_branches(char*,char* = NULL); 
