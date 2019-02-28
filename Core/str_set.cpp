#include "str_set.h"

str_m_set create_str_m_set(int size)
{
	str_m_set new_set;
	new_set.max_size = size;
	new_set.strs = (char*)malloc(SET_STR_MAX_SIZE*size);
	new_set.allocated = (bool*)malloc(sizeof(bool)*size);
	memset(new_set.allocated, 0, sizeof(bool)*size);
	return new_set;
}

void destroy_str_m_set(str_m_set* set)
{
	free(set->allocated);
	free(set->strs);
}

void str_m_set::print()
{
	for(int i = 0; i < size(); i++)
	{
		if(is_allocated(i)) printf("%d: %s\n", i, find_str(i));
	}
}

char* str_m_set::find_str(int i)
{
	return strs+(i*SET_STR_MAX_SIZE);
}

char* str_m_set::find_str_and_alloc(int* index)
{
	for(int i = 0; i < size(); i++)
	{
		if(!is_allocated(i))
		{
			alloc(i);
			if(index) *index = i;
			return find_str(i);
		}
	}
	return NULL;
}

void str_m_set::alloc(int i)
{
	allocated[i] = true;
}

int str_m_set::number_allocated()
{
	int total = 0;
	for(int i = 0; i < size(); i++) if(is_allocated(i)) total++;
	return total;
}

int str_m_set::size()
{
	return max_size;
}

bool str_m_set::is_allocated(int i)
{
	return allocated[i];
}

void str_m_set::free(int i)
{
	allocated[i] = false;
}

void str_m_set::free_all()
{
	for(int i = 0; i < size(); i++) free(i);
}
