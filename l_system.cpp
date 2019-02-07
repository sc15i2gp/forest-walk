#include "l_system.h"

bool has_condition(production* p)
{
	return p->condition[0] != 0;
}

void copy_l_system_unit(char* src, char* dest, int& start, int& end)
{
	while(src[end] != '|' && src[end] != '\n') end++;
	if(end-start > 0) memcpy(dest, src+start, end-start);
	dest[end-start] = 0;
	end++;
	start = end;
}

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
		if(buffer[start] == 0 || start >= size - 1) break;
	}
	free(buffer);
}

void save_l_system(l_system* l, const char* path, char* axiom)
{
	FILE* f = fopen(path, "w");
	fprintf(f, axiom);
	fprintf(f, "\n");
	
	for(int i = 0; i < l->p_set_size; i++)
	{
		production* p = l->p_set + i;
		fprintf(f, p->predecessor.l_context);
		fprintf(f, "|");
		fprintf(f, p->predecessor.strict);
		fprintf(f, "|");
		fprintf(f, p->predecessor.r_context);
		fprintf(f, "|");
		fprintf(f, p->successor);
		fprintf(f, "|");
		fprintf(f, p->condition);
		fprintf(f, "|");
		char prob[16] = {};
		snprintf(prob, 16, "%f", p->probability);
		fprintf(f, prob);
		fprintf(f, "\n");
	}
	fclose(f);
}

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

production* get_next_available_production(l_system* l)
{
	production* p = l->p_set + l->p_set_size;
	l->p_set_size++;
	return p;
}

void add_production(l_system* l, char* pre_l_context, char* pre_strict, char* pre_r_context, char* successor, char* condition, char* probability)
{
	float p = atof(probability);
	add_production(l, pre_l_context, pre_strict, pre_r_context, successor, condition, p);
}

void add_production(l_system* l, char* pre_l_context, char* pre_strict, char* pre_r_context, char* successor, char* condition, float probability)
{
	production* p = get_next_available_production(l);
	assert(p);
	
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
				convert_expression_to_rpn(find_parameter(successor_module,j), length_of_parameter(successor_module, j));
			}
		}
		successor_module = find_next_module(successor_module);
	}
	//Convert condition to rpn
	int condition_length = strlen(p->condition);
	if(condition_length > 0)
	{
		convert_expression_to_rpn(p->condition, condition_length);
	}

	//Context sizes
	int l_context_size = number_of_modules(p->predecessor.l_context);
	int r_context_size = number_of_modules(p->predecessor.r_context);
	if(l_context_size > l->max_l_context_size) l->max_l_context_size = l_context_size;
	if(r_context_size > l->max_r_context_size) l->max_r_context_size = r_context_size;
}

void add_production(l_system* l, char* predecessor, char* successor, char* condition, float probability)
{
	int pre_length = number_of_modules(predecessor);

	char l_context[64] = {};
	char strict[64] = {};
	char r_context[64] = {};

	char* l_context_end = predecessor;
	for(; *l_context_end != '<'; l_context_end++);
	memcpy(l_context, predecessor, l_context_end - predecessor);

	char* strict_begin = l_context_end + 1;
	char* strict_end = strict_begin;
	for(; *strict_end != '>'; strict_end++);
	memcpy(strict, strict_begin, strict_end - strict_begin);

	char* r_context_begin = strict_end + 1;
	char* r_context_end = r_context_begin;
	for(; *r_context_end != 0; r_context_end++);
	memcpy(r_context, r_context_begin, r_context_end - r_context_begin);

	if(!condition) add_production(l, l_context, strict, r_context, successor, "\0", probability);
	else add_production(l, l_context, strict, r_context, successor, condition, probability);
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

void input_l_context_parameter_values(char* production_context, char_queue* context_queue, char* successor)
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

void input_r_context_parameter_values(char* production_context, char_queue* context_queue, char* successor)
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

bool condition_met(production* p, char* strict_predecessor, char_queue* l_context_queue, char_queue* r_context_queue)
{
	if(has_condition(p))
	{
		char condition[CONDITION_MAX] = {};
		strcpy(condition, p->condition);
		input_l_context_parameter_values(p->predecessor.l_context, l_context_queue, condition);
		input_r_context_parameter_values(p->predecessor.r_context, r_context_queue, condition);
		replace_parameter_tokens_with_values(p->predecessor.strict, strict_predecessor, condition);
		float result = compute_rpn_string_result(condition, strlen(condition));
		return result > 0;
	}
	else return true;
}

//L and R contexts can match with production's if the production's L and R contexts are subsets of the real contexts at a particular end of the set. The following 2 examples show a (2, 4)-l-system where the contexts match:
//L Context: A(1)B(2) Predecessor L Context: B(c)
//R Context: D(2)[E(12)]EF Predecessor R Context: D(a)[E(r+15)]

bool matches(production* p, char* strict_predecessor, char_queue* l_context_queue, char_queue* r_context_queue)
{
	if(l_contexts_match(p->predecessor.l_context, l_context_queue) && r_contexts_match(p->predecessor.r_context, r_context_queue))
	{
		return modules_match(strict_predecessor, p->predecessor.strict) && condition_met(p, strict_predecessor, l_context_queue, r_context_queue);
	}
	return false;
}

bool are_predecessors_equal(production* p, production* q)
{
	bool l_contexts_match = module_strings_match(p->predecessor.l_context, q->predecessor.l_context);
	bool r_contexts_match = module_strings_match(p->predecessor.r_context, q->predecessor.r_context);
	bool stricts_match = module_strings_match(p->predecessor.strict, q->predecessor.strict);
	return l_contexts_match && r_contexts_match && stricts_match;
}

int sample_non_deterministic_productions(production* initial_p, int initial_p_index)
{
	//Non-deterministic context sensitive
	int nd_candidates[8] = {-1};
	float nd_probabilities[8] = {0.0};
	int nd_production_index = 0;
	production* nd_p = initial_p;
	while(are_predecessors_equal(nd_p, initial_p))
	{
		nd_candidates[nd_production_index] = initial_p_index + nd_production_index;
		nd_probabilities[nd_production_index] = nd_p->probability;
		nd_production_index++;
		nd_p++;
	}

	//Sample matching productions' probabilities as categorical distributions
	//TODO: Seeds associated with particular derivation strings to produce the same output each time
	int n = rand() % 100 + 1;
	int upper_bound = 1;
	int lower_bound = 1;
	for(int k = 0; k < nd_production_index; k++)
	{
		upper_bound += (int)(100*nd_probabilities[k]);
		if(n < upper_bound && n >= lower_bound)
		{
			return nd_candidates[k];
		}
		lower_bound = upper_bound;
	}
}
//There can be multiple context sensitive productions which match a given predecessor and contexts. The first one is usually returned. However, if the first matching context sensitive production has a probability of less than one, then all productions with the exact same predecessors and contexts should be found. Their probabilities sum to 1. Can it be assumed that these productions will be grouped together? Yes it can.
//In terms of context free productions, the only time multiple can match is the singular set which share the same predecessor.
production* pick_production(l_system* l, char* strict_predecessor, char_queue* l_context_queue, char_queue* r_context_queue)
{
	production* p = NULL;
	int context_free_candidate = -1;
	for(int j = 0; j < l->p_set_size; j++)
	{
		production* p_j = l->p_set+j;
		bool production_matches = matches(p_j, strict_predecessor, l_context_queue, r_context_queue);
		if(production_matches && is_context_sensitive(p_j) && p_j->probability == 1.0) 
		{
			//Deterministic context sensitive
			p = p_j;
			return p;
		}
		else if(production_matches && is_context_sensitive(p_j))
		{
			//Non deterministic context sensitive
			int p_index = sample_non_deterministic_productions(p_j, j);
			p = l->p_set + p_index;
			return p;
		}
		else if(production_matches && !is_context_sensitive(p_j) && context_free_candidate < 0)
		{
			//Context free
			if(p_j->probability == 1.0) context_free_candidate = j; // Deterministic
			else //Non deterministic
			{
				int p_index = sample_non_deterministic_productions(p_j, j);
				context_free_candidate = p_index;
			}
		}
	}
	if(!p && context_free_candidate >= 0) p = l->p_set+context_free_candidate;
	return p;
}

#define MAX_DERIVED_OUTPUT_SIZE 0x400000
void derive_str(l_system* l, char* input_str)
{
	char* start = input_str;
	prune_branches(input_str);
	//printf("Deriving %s\n", input_str);
	//TODO far future: Move this to function parameter
	char* output_buffer = (char*)malloc(MAX_DERIVED_OUTPUT_SIZE); 
	char* output_loc = output_buffer;
	char_queue l_context_queue;
	char_queue r_context_queue;
	STACK(char_queue) l_context_stack;
	char* current_module = input_str;
	int l_context_push_count = 0;
	int r_context_push_count = 0;
	for(int i = 0, input_length = number_of_modules(input_str); i < input_length; i++)
	{
		/*
		printf("current module = ");
		print_module(current_module);
		*/
		//Work out r_context
		char* r_context_module = find_next_module(current_module);
		for(int j = 0; j < l->max_r_context_size; j++)
		{
			if(!r_context_module || *r_context_module == ']') break;
			while(should_ignore_in_context(r_context_module)) r_context_module = find_next_module(r_context_module);
			if(*r_context_module == '[')
			{
				char* branch_start = r_context_module;
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
		/*
		printf("L_CONTEXT ");
		print_queue_contents(&l_context_queue);
		printf("R_CONTEXT ");
		print_queue_contents(&r_context_queue);*/
		//Pick production
	       	production* p = pick_production(l, current_module, &l_context_queue, &r_context_queue);
		/*
		printf("Picked production ");
		print_production(p);
		*/
		//Push successor to output queue
		if(p)
		{
			//FAILURE POINT: If p->successor > 512
			//NOTE: Would be good if successor could be strcpy'd straight to output_queue
			char successor[512] = {};
			strcpy(successor, p->successor);

			input_l_context_parameter_values(p->predecessor.l_context, &l_context_queue, successor);
			input_r_context_parameter_values(p->predecessor.r_context, &r_context_queue, successor);
			replace_parameter_tokens_with_values(p->predecessor.strict, current_module, successor);

			int successor_length = number_of_modules(successor);
			char* successor_module = successor;
			for(int j = 0; j < successor_length; j++)
			{
				int param_count = number_of_parameters(successor_module);
				for(int k = 0; k < param_count; k++)
				{
					if(is_parameter_expression(successor_module, k))
					{
						char* param = find_parameter(successor_module, k);
						int param_length = length_of_parameter(successor_module, k);
						write_into_parameter(successor_module, k, compute_rpn_string_result(param, param_length));
					}
				}
				successor_module = find_next_module(successor_module);
			}

			//FAILURE POINT: If output_queue is full
			if(output_loc + strlen(successor) < output_buffer + MAX_DERIVED_OUTPUT_SIZE)
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
		{
			//FAILURE POINT: If output_queue is full
			if(output_loc + length_of_module(current_module) < output_buffer + MAX_DERIVED_OUTPUT_SIZE)
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
		r_context_queue.pop_all(); //TODO: Fix infinte loop
		r_context_push_count = 0;

		//Push current module into l_context
		if(*current_module == '[')
		{
			//FAILURE POINT: If no more space on l_context_stack
			l_context_stack.push_state(l_context_queue);
		}
		else if(*current_module == ']')
		{
			//FAILURE POINT: If ] has no matching [
			l_context_queue = l_context_stack.pop_state();
		}
		else if(!should_ignore_in_context(current_module))
		{
			if(l_context_push_count >= l->max_l_context_size)
			{//If number of modules pushed to l_context is greater than max_l_context_size
				l_context_queue.pop();
				l_context_push_count--;
			}
			assert(l_context_queue.push(current_module, length_of_module(current_module)));
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
