#include "render_data.h"

/************************************************/

/*		Render Queue			*/

/************************************************/

render_queue create_render_queue(int max_queue_length)
{
	render_queue r_queue = {};
	r_queue.to_render_buffer = (tree_node**)malloc(sizeof(tree_node*)*max_queue_length);
	memset(r_queue.to_render_buffer, 0, sizeof(tree_node*)*max_queue_length);
	r_queue.head = r_queue.to_render_buffer;
	r_queue.max_length = max_queue_length;
	r_queue.length = 0;
	return r_queue;
}

void destroy_render_queue(render_queue r_queue)
{
	free(r_queue.to_render_buffer);
}

void merge(tree_node** left_node_list, tree_node** right_node_list, int left_list_length, int right_list_length)
{
	tree_node* temp_node_list[4096] = {};
	int left_list_counter = 0;
	int right_list_counter = 0;
	int temp_list_counter = 0;
	while(left_list_counter < left_list_length || right_list_counter < right_list_length)
	{
		if(right_list_counter == right_list_length)
		{
			temp_node_list[temp_list_counter++] = left_node_list[left_list_counter++];
			continue;
		}
		else if(left_list_counter == left_list_length)
		{
			temp_node_list[temp_list_counter++] = right_node_list[right_list_counter++];
			continue;
		}


		if(left_node_list[left_list_counter]->species < right_node_list[right_list_counter]->species)
		{
			temp_node_list[temp_list_counter++] = left_node_list[left_list_counter++];
		}
		else if(left_node_list[left_list_counter]->species > right_node_list[right_list_counter]->species)
		{
			temp_node_list[temp_list_counter++] = right_node_list[right_list_counter++];
		}
		else
		{
			if(left_node_list[left_list_counter]->seed < right_node_list[right_list_counter]->seed)
			{
				temp_node_list[temp_list_counter++] = left_node_list[left_list_counter++];
			}
			else if(left_node_list[left_list_counter]->seed > right_node_list[right_list_counter]->seed)
			{

				temp_node_list[temp_list_counter++] = right_node_list[right_list_counter++];
			}
			else
			{
				if(left_node_list[left_list_counter]->age < right_node_list[right_list_counter]->age)
				{
					temp_node_list[temp_list_counter++] = left_node_list[left_list_counter++];
				}
				else
				{
					temp_node_list[temp_list_counter++] = right_node_list[right_list_counter++];
				}
			}
		}
	}
	for(int i = 0; i < temp_list_counter; i++)
	{
		left_node_list[i] = temp_node_list[i];
	}
}

void _sort(tree_node** node_list, int list_length)
{
	if(list_length > 1)
	{
		int left_list_length = list_length/2;
		int right_list_length = list_length - left_list_length;
		_sort(node_list, left_list_length);
		_sort(node_list+left_list_length, right_list_length);
		merge(node_list, node_list+left_list_length, left_list_length, right_list_length);
	}
}
void render_queue::sort()
{
	_sort(to_render_buffer, length);
}

void render_queue::push(tree_node* t_node)
{
	to_render_buffer[length] = t_node;
	length++;
	assert(length <= max_length);
}

tree_node* render_queue::pop()
{
	tree_node* t = *head;
	head++;
	length--;
	return t;
}

tree_node* render_queue::find_item(int i)
{
	return to_render_buffer[i];
}

void render_queue::reset()
{
	head = to_render_buffer;
	length = 0;
}

void render_queue::print()
{
	for(tree_node** ptr = head; ptr < head + length; ptr++)
	{
		tree_node* t_node = *ptr;
		printf("R_QUEUE[%d] = %d %ld %d\n", ptr-head, t_node->species, t_node->seed, t_node->age);
	}
}

/************************************************/

/*		Tree model map			*/

/************************************************/

tree_model_map create_tree_model_map(int number_of_model_refs, int number_of_model_caches)
{
	tree_model_map t = {};
	t.tree_model_refs = (tree_model_ref*)malloc(sizeof(tree_model_ref)*number_of_model_refs);
	memset(t.tree_model_refs, 0, sizeof(tree_model_ref)*number_of_model_refs);
	t.number_of_tree_model_refs = number_of_model_refs;
	t.tree_models = (model_cache*)malloc(sizeof(model_cache)*number_of_model_caches);
	memset(t.tree_models, 0, sizeof(model_cache)*number_of_model_caches);
	t.number_of_tree_model_caches = number_of_model_caches;
	return t;
}

void destroy_tree_model_map(tree_model_map t)
{
	free(t.tree_models);
	free(t.tree_model_refs);
}

int tree_model_map::find_available_model_cache()
{
	for(int j = 0; j < number_of_tree_model_caches; j++) if(tree_models[j].ref_count == 0) return j;
	return -1;
}

void tree_model_map::add_model_ref(tree_node* t_node, int model_index)
{
	for(int i = 0; i < number_of_tree_model_refs; i++)
	{
		if(!tree_model_refs[i].tree)
		{
			tree_model_refs[i].tree = t_node;
			tree_model_refs[i].model_ref = (model_index >= 0) ? model_index : find_available_model_cache();
			tree_models[tree_model_refs[i].model_ref].ref_count++;
			break;
		}
	}
}

tree_model_ref* tree_model_map::find_tree_model_ref(tree_node* t_node)
{
	for(int i = 0; i < number_of_tree_model_refs; i++) if(tree_model_refs[i].tree == t_node) return tree_model_refs+i;
	return NULL;
}

int tree_model_map::model_ref(tree_node* t_node)
{
	return find_tree_model_ref(t_node)->model_ref;
}

int tree_model_map::model_ref_count(tree_node* t_node)
{
	return tree_models[model_ref(t_node)].ref_count;
}

int tree_model_map::model_ref_count(int model_index)
{
	return tree_models[model_index].ref_count;
}

int tree_model_map::lod(tree_node* t_node)
{
	return tree_models[model_ref(t_node)].lod;
}

int tree_model_map::lod(int model_index)
{
	return tree_models[model_index].lod;
}

void tree_model_map::release_ref(tree_model_ref* ref)
{
	model_cache* m_cache = tree_models+ref->model_ref;
	m_cache->ref_count--;
	if(m_cache->ref_count == 0) 
	{//If the model will be cleared from VRAM
		buffered_model_data_size -= m_cache->model_size;
		m_cache->model_size = 0;
	}
	ref->tree = NULL;
	ref->model_ref = -1;
}

void tree_model_map::release_ref(tree_node* t_node)
{
	tree_model_ref* ref = find_tree_model_ref(t_node);
	release_ref(ref);
}

void tree_model_map::release_ref(int ref_index)
{
	tree_model_ref* ref = tree_model_refs+ref_index;
	release_ref(ref);
}

tree_buffer_object tree_model_map::find_model(tree_node* t_node)
{
	tree_model_ref* ref = find_tree_model_ref(t_node);
	return tree_models[ref->model_ref].model;
}

tree_buffer_object tree_model_map::find_model(int ref_index)
{
	tree_model_ref* ref = tree_model_refs+ref_index;
	return tree_models[ref->model_ref].model;
}

void tree_model_map::set_model(tree_node* t_node, tree_buffer_object model, int model_size, int lod)
{
	tree_model_ref* ref = find_tree_model_ref(t_node);
	tree_models[ref->model_ref].model = model;
	tree_models[ref->model_ref].lod = lod;
	tree_models[ref->model_ref].model_size = model_size;
	buffered_model_data_size += model_size;
}

bool tree_model_map::tree_has_model(tree_node* t_node)
{
	return find_tree_model_ref(t_node) != NULL;
}

bool tree_model_map::ref_in_use(int ref_index)
{
	return tree_model_refs[ref_index].tree != NULL;
}

bool tree_model_map::tree_in_use(int ref_index)
{
	return tree_model_refs[ref_index].tree->in_use;
}

void tree_model_map::print()
{
	for(int i = 0; i < number_of_tree_model_refs; i++)
	{
		if(ref_in_use(i)) printf("%d: TREE = %x MODEL = %d\n", i, tree_model_refs[i].tree, tree_model_refs[i].model_ref);
	}
}
