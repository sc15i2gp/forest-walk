#include "l_system.h"


/************************************************************/

/*************	Change L-system Properties  *****************/

/************************************************************/

//Returns first production in l_system's production set not already being used to describe a production
production* get_next_available_production(l_system* l)
{
	production* p = l->p_set + l->p_set_size;
	l->p_set_size++;
	return p;
}

//Append production to l-system's production set
void add_production(l_system* l, char* pre_l_context, char* pre_strict, char* pre_r_context, char* successor, char* condition, float probability)
{
	production* p = get_next_available_production(l);

	strcpy(p->predecessor.l_context, pre_l_context);
	strcpy(p->predecessor.strict, pre_strict);
	strcpy(p->predecessor.r_context, pre_r_context);
	strcpy(p->successor, successor);
	strcpy(p->condition, condition);
	p->probability = probability;

	//Convert successor expressions to rpn
	char* successor_module = p->successor;
	for(int i = 0; i < number_of_modules(p->successor); i++)
	{
		int module_param_count = number_of_parameters(successor_module);
		for(int j = 0; j < module_param_count; j++)
		{
			if(is_parameter_expression(successor_module, j))
			{
				convert_expression_to_postfix(find_parameter(successor_module,j), length_of_parameter(successor_module, j));
			}
		}
		successor_module = find_next_module(successor_module);
	}

	//Convert condition to rpn
	int condition_length = strlen(p->condition);
	if(condition_length > 0)
	{
		convert_expression_to_postfix(p->condition, condition_length);
	}

	//Context sizes
	int l_context_size = number_of_modules(p->predecessor.l_context);
	int r_context_size = number_of_modules(p->predecessor.r_context);
	
	if(l_context_size > l->max_l_context_size) l->max_l_context_size = l_context_size;
	if(r_context_size > l->max_r_context_size) l->max_r_context_size = r_context_size;
}

//Overloaded function used when probability is passed as a string when l_system is being loaded from a text file
void add_production(l_system* l, char* pre_l_context, char* pre_strict, char* pre_r_context, char* successor, char* condition, char* probability)
{
	float p = atof(probability);
	add_production(l, pre_l_context, pre_strict, pre_r_context, successor, condition, p);
}

//Overloaded function takes a single predecessor string containing contexts and strict predecessor
//Format: L<S>R where L is left context, S is strict predecessor and R is right context
void add_production(l_system* l, char* predecessor, char* successor, char* condition, float probability)
{
	char l_context[64] = {};
	char strict[64] = {};
	char r_context[64] = {};

	//Copy left context from predecessor
	char* l_context_end = predecessor;
	for(; *l_context_end != '<'; l_context_end++);
	memcpy(l_context, predecessor, l_context_end - predecessor);

	//Copy strict predecessor from predecessor
	char* strict_begin = l_context_end + 1;
	char* strict_end = strict_begin;
	for(; *strict_end != '>'; strict_end++);
	memcpy(strict, strict_begin, strict_end - strict_begin);

	//Copy right context from predecessor
	char* r_context_begin = strict_end + 1;
	char* r_context_end = r_context_begin;
	for(; *r_context_end != 0; r_context_end++);
	memcpy(r_context, r_context_begin, r_context_end - r_context_begin);

	//Add production to l_system
	if(!condition) 	add_production(l, l_context, strict, r_context, successor, "\0", probability);
	else 		add_production(l, l_context, strict, r_context, successor, condition, probability);
}

void add_global_parameter(l_system* l, char token, char* initial_value)
{
	char* default_value = "0.000";
	if(!initial_value) initial_value = default_value;

	param_mapping* mapping = l->param_map;
	while(mapping->symbol > 0) mapping++;

	mapping->symbol = token;
	strcpy(mapping->real_str, initial_value);
}

void set_global_parameter(l_system* l, char token, char* value)
{
	param_mapping* mapping = l->param_map;
	while(mapping->symbol != token)
	{
		if(mapping >= l->param_map + PARAM_MAP_MAX) return;
		mapping++;
	}
	strcpy(mapping->real_str, value);
}

void set_production_probability(l_system* l, int p_index, float probability)
{
	production* p = l->p_set + p_index;
	p->probability = probability;
}


/***********************************************/

/*************	Derive String  *****************/

/***********************************************/

bool has_condition(production* p)
{
	return p->condition[0] != 0;
}


bool should_ignore_in_context(char* module)
{
	char symbol = *module;
	return symbol == '+' || symbol == '-' || symbol == '/' || symbol == '\\';
}

bool is_context_sensitive(production* p)
{
	return p->predecessor.l_context[0] != 0 || p->predecessor.r_context[0] != 0;
}

//Replaces parameter tokens from production's left context in the successor string with their values
void replace_l_context_parameter_tokens_with_values(char* production_context, char_queue* context_queue, char* successor)
{
	int context_queue_length = context_queue->push_count;
	int p_context_length = number_of_modules(production_context);

	if(p_context_length <= context_queue_length)
	{
		int start_from = context_queue_length - p_context_length;
		char* context_module = production_context;
		for(int i = start_from; i < context_queue_length; i++)
		{
			char* queue_module = context_queue->get_pushed_string(i);
			replace_parameter_tokens_with_values(context_module, queue_module, successor);
		}
	}
}

//Replaces parameter tokens from production's right context in the successor string with their values
void replace_r_context_parameter_tokens_with_values(char* production_context, char_queue* context_queue, char* successor)
{
	int context_length = number_of_modules(production_context);
	char* context_module = production_context;
	for(int i = 0; i < context_length; i++)
	{
		char* queue_module = context_queue->get_pushed_string(i);
		if(!queue_module) return;
		replace_parameter_tokens_with_values(context_module, queue_module, successor);
		context_module = find_next_module(context_module);
	}
}

//Compares context string to contents of the right context queue and returns true if they all match
bool r_contexts_match(char* context, char_queue* context_queue)
{
	int context_length = number_of_modules(context);
	char* context_module = context;

	for(int i = 0; i < context_length; i++)
	{
		char* queue_module = context_queue->get_pushed_string(i);
		if(!queue_module || !modules_match(queue_module, context_module)) return false;
		context_module = find_next_module(context_module);
	}
	return true;
}

bool l_contexts_match(char* production_context, char_queue* context_queue)
{
	int context_queue_length = context_queue->push_count;
	int p_context_length = number_of_modules(production_context);
	if(p_context_length <= context_queue_length)
	{
		int check_from = context_queue_length - p_context_length;
		char* context_module = production_context;
		for(int i = check_from; i < context_queue_length; i++)
		{
			char* queue_module = context_queue->get_pushed_string(i);
			if(!modules_match(queue_module, context_module)) return false;
			context_module = find_next_module(context_module);
		}
		return true;
	}
	else return false;
}

void replace_global_parameter_tokens_with_values(param_mapping* param_map, char* to_overwrite)
{
	for(int i = 0; param_map[i].symbol > 0; i++)
	{//For each global parameter
		overwrite_token_occurrences(to_overwrite, param_map[i].symbol, param_map[i].real_str);
	}
}

//Returns the result of a production's condition being evaluated
//An empty condition string always evalutes to true
bool condition_met(production* p, char* strict_predecessor, char_queue* l_context_queue, char_queue* r_context_queue, param_mapping* param_map)
{
	if(has_condition(p))
	{
		char condition[CONDITION_MAX] = {};
		strcpy(condition, p->condition);

		replace_l_context_parameter_tokens_with_values(p->predecessor.l_context, l_context_queue, condition);
		replace_l_context_parameter_tokens_with_values(p->predecessor.r_context, r_context_queue, condition);

		replace_parameter_tokens_with_values(p->predecessor.strict, strict_predecessor, condition);
		replace_global_parameter_tokens_with_values(param_map, condition);

		float result = compute_postfix_string_result(condition, strlen(condition));
		return result > 0;
	}
	else return true;
}

//L and R contexts can match with production's if the production's L and R contexts are subsets of the real contexts at a particular end of the set. The following 2 examples show a (2, 4)-l-system where the contexts match:
//L Context: A(1)B(2) Predecessor L Context: B(c)
//R Context: D(2)[E(12)]EF Predecessor R Context: D(a)[E(r+15)]

bool matches(production* p, char* strict_predecessor, char_queue* l_context_queue, char_queue* r_context_queue, param_mapping* param_map)
{
	return 	l_contexts_match(p->predecessor.l_context, l_context_queue) && 
		r_contexts_match(p->predecessor.r_context, r_context_queue) && 
		modules_match(strict_predecessor, p->predecessor.strict) && 
		condition_met(p, strict_predecessor, l_context_queue, r_context_queue, param_map);
}

bool are_predecessors_equal(production* p, production* q)
{
	bool l_contexts_match = module_strings_match(p->predecessor.l_context, q->predecessor.l_context);
	bool r_contexts_match = module_strings_match(p->predecessor.r_context, q->predecessor.r_context);
	bool stricts_match = module_strings_match(p->predecessor.strict, q->predecessor.strict);

	return l_contexts_match && r_contexts_match && stricts_match;
}

//Find all applicable non-deterministic productions and return a randomly chosen one
//Applicable non-deterministic productions are all of the productions following a given starting one
//which share the same predecessors (including contexts)
//The sum of all found matching productions' probabilities = 1
int sample_non_deterministic_productions(production* initial_p, int initial_p_index)
{
	//Non-deterministic context sensitive
	int nd_candidates[8] = {-1};
	float nd_probabilities[8] = {0.0};
	int nd_production_index = 0;

	production* nd_p = initial_p;

	while(are_predecessors_equal(nd_p, initial_p))
	{//Find all non deterministic productions which can be applied
		nd_candidates[nd_production_index] = initial_p_index + nd_production_index;
		nd_probabilities[nd_production_index] = nd_p->probability;
		nd_production_index++;
		nd_p++;
	}

	//Sample matching productions' probabilities as categorical distributions
	int n = rand() % 100 + 1;
	int upper_bound = 1;
	int lower_bound = 1;

	int chosen_candidate = 0;

	for(int k = 0; k < nd_production_index; k++)
	{
		upper_bound += (int)(100*nd_probabilities[k]);
		if(n < upper_bound && n >= lower_bound)
		{
			chosen_candidate = nd_candidates[k];
			break;
		}
		lower_bound = upper_bound;
	}
	return chosen_candidate;
}

//There can be multiple context sensitive productions which match a given predecessor and contexts. The first one is usually returned. However, if the first matching context sensitive production has a probability of less than one, then all productions with the exact same predecessors and contexts should be found. Their probabilities sum to 1. Can it be assumed that these productions will be grouped together? Yes it can.
//In terms of context free productions, the only time multiple can match is the singular set which share the same predecessor.
production* pick_production(l_system* l, char* strict_predecessor, char_queue* l_context_queue, char_queue* r_context_queue)
{
	production* p = NULL;

	for(int j = 0; j < l->p_set_size; j++)
	{//For each production in the l_system
		production* p_j = l->p_set+j;
		bool production_matches = matches(p_j, strict_predecessor, l_context_queue, r_context_queue, l->param_map);
		if(production_matches && p_j->probability == 1.0) 
		{
			//Deterministic context sensitive production found
			p = p_j;
			return p;
		}
		else if(production_matches)
		{
			//Non deterministic context sensitive production found
			int p_index = sample_non_deterministic_productions(p_j, j);
			p = l->p_set + p_index;
			return p;
		}
	}
	return p;
}

//Takes an input string and generates a successor string according to a given l_system
void derive_str(l_system* l, char* input_str, int max_output_size)
{
	MEASURE_TIME_IF_EDITOR;
	char* output_buffer = (char*)malloc(max_output_size); 
	char* output_loc = output_buffer;

	char_queue l_context_queue;
	char_queue r_context_queue;
	STACK(char_queue) l_context_stack;

	int l_context_push_count = 0;
	int r_context_push_count = 0;

	char* current_module = input_str;
	
	for(int i = 0, input_length = number_of_modules(input_str); i < input_length; i++)
	{//For each module in the input
		//Work out input module's right context
		char* r_context_module = find_next_module(current_module);
		for(int j = 0; j < l->max_r_context_size; j++)
		{
			if(!r_context_module || *r_context_module == ']') break;
			while(should_ignore_in_context(r_context_module)) r_context_module = find_next_module(r_context_module);
			if(*r_context_module == '[')
			{
				while(*r_context_module == '[') r_context_module = find_next_module(r_context_module);
				r_context_queue.push("[");
				r_context_queue.push(r_context_module, length_of_module(r_context_module));
				r_context_queue.push("]");
				r_context_module = find_end_of_branch(r_context_module);
			}
			else
			{
				assert(r_context_queue.push(r_context_module, length_of_module(r_context_module)));
			}
			r_context_push_count++;
			r_context_module = find_next_module(r_context_module);
		}

		//Pick production
	       	production* p = pick_production(l, current_module, &l_context_queue, &r_context_queue);

		//Generate and push successor to output queue
		if(p)
		{//If there is a production which can be applied with the current input module

			char successor[512] = {};
			strcpy(successor, p->successor);

			//Replace parameter tokens in successor string with actual values
			replace_l_context_parameter_tokens_with_values(p->predecessor.l_context, &l_context_queue, successor);
			replace_r_context_parameter_tokens_with_values(p->predecessor.r_context, &r_context_queue, successor);
			replace_parameter_tokens_with_values(p->predecessor.strict, current_module, successor);
			replace_global_parameter_tokens_with_values(l->param_map, successor);

			int successor_length = number_of_modules(successor);
			char* successor_module = successor;

			for(int j = 0; j < successor_length; j++)
			{//For each module in the successor string
				int param_count = number_of_parameters(successor_module);
				for(int k = 0; k < param_count; k++)
				{//For each parameter in the module
					if(is_parameter_expression(successor_module, k))
					{//If the parameter is an arithmetic expression, calculate result of expression and replace expression string with result
						char* param = find_parameter(successor_module, k);
						int param_length = length_of_parameter(successor_module, k);
						write_into_parameter(successor_module, k, compute_postfix_string_result(param, param_length));
					}
				}
				successor_module = find_next_module(successor_module);
			}

			//Copy successor string to output queue
			if(output_loc + strlen(successor) < output_buffer + max_output_size)
			{
				memcpy(output_loc, successor, strlen(successor));
				output_loc += strlen(successor);
			}
			else
			{
				printf("%s (%d) error: output queue is full\n",__func__,__LINE__);
				return;
			}
		}
		else
		{//If the identity production should be used
			//Copy input module to output queue
			if(output_loc + length_of_module(current_module) < output_buffer + max_output_size)
			{
				memcpy(output_loc, current_module, length_of_module(current_module));
				output_loc += length_of_module(current_module);
			}
			else
			{
				printf("%s (%d) error: output queue is full\n",__func__,__LINE__);
				return;
			}
		}

		//Clear right context so the next input module's right context  can be determined
		r_context_queue.pop_all(); 
		r_context_push_count = 0;

		//Push current module into l_context stack
		if(*current_module == '[') l_context_stack.push_state(l_context_queue);
		else if(*current_module == ']') l_context_queue = l_context_stack.pop_state();
		else if(!should_ignore_in_context(current_module) && l->max_l_context_size > 0)
		{
			if(l_context_push_count >= l->max_l_context_size)
			{//If number of modules pushed to l_context is greater than max_l_context_size
				l_context_queue.pop();
				l_context_push_count--;
			}
			l_context_queue.push(current_module, length_of_module(current_module));
			l_context_push_count++;
		}

		current_module = find_next_module(current_module);
	}
	*output_loc = 0;
	output_loc++;

	int output_length = output_loc - output_buffer;
	memcpy(input_str, output_buffer, output_length);

	free(output_buffer);
}

/****************************************/

/*************	Print	*****************/

/****************************************/

void print_production(production* p)
{
	if(p)
	{
		printf("%s<%s>%s --> %s | %.2f", p->predecessor.l_context, p->predecessor.strict, p->predecessor.r_context, p->successor, p->probability);
		if(has_condition(p)) printf(" | %s\n", p->condition);
		else printf("\n");
	}
	else printf("identity\n");
}

void print_l_system(l_system* l, const char* name)
{
	printf("L-System %s:\n", name);
	for(int i = 0; i < l->p_set_size; i++)
	{
		printf("p_%d: ", i); 
		print_production(l->p_set + i);
	}
}

/*****************************************/

/*************	File I/O *****************/

/*****************************************/


//L-system is described as an axiom and a number of productions
//A single "unit" is one of:
//	- Axiom
//	- right context
//	- strict predecessor
//	- left context
//	- condition
//	- Successor
//This function copies a unit (delimited by '|' or '\n') into a char buffer
void copy_l_system_unit(char* src, char* dest, int& start, int& end)
{
	while(src[end] != '|' && src[end] != '\n') end++;
	if(end-start > 0) memcpy(dest, src+start, end-start);
	dest[end-start] = 0;
	end++;
	start = end;
}

//Load l-system from a file
void load_l_system(l_system* l, const char* path, char* axiom)
{
	l->p_set_size = 0;

	FILE* f = fopen(path, "r");
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, 0, SEEK_SET);
	char* buffer = (char*)malloc(size+1);
	fread(buffer, 1, size, f);
	fclose(f);

	buffer[size] = 0;
	int start = 0;
	int end = 0;

	copy_l_system_unit(buffer, axiom, start, end);

	for(int i = 0;; i++)
	{//For each production in the file
		production* p = l->p_set+i;

		copy_l_system_unit(buffer, p->predecessor.l_context, start, end);
		copy_l_system_unit(buffer, p->predecessor.strict, start, end);
		copy_l_system_unit(buffer, p->predecessor.r_context, start, end);
		copy_l_system_unit(buffer, p->successor, start, end);
		copy_l_system_unit(buffer, p->condition, start, end);

		char prob[16] = {};
		copy_l_system_unit(buffer, prob, start, end);
		p->probability = atof(prob);

		l->p_set_size++;

		if(buffer[start] == 0 || start >= size - 1) break; //Exit loop when file has been completely read
	}
	free(buffer);
}

//Saves l-system to file
void save_l_system(l_system* l, const char* path, char* axiom)
{
	FILE* f = fopen(path, "w");
	fprintf(f, "%s\n", axiom);
	
	for(int i = 0; i < l->p_set_size; i++)
	{
		production* p = l->p_set + i;

		char prob[16] = {};
		snprintf(prob, 16, "%f", p->probability);
		
		fprintf(f, "%s|%s|%s|%s|%s|%s\n", p->predecessor.l_context, p->predecessor.strict, p->predecessor.r_context, p->successor, p->condition, prob);
	}
	fclose(f);
}

