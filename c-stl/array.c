#include"array.h"

int array_init(array *array, size_t capacity)
{
    assert(array != NULL);
    array->size = 0;
    array->capacity = capacity;
    array->table = (void**)malloc(capacity * sizeof(void**));
    assert(array->table != NULL);
	if (!array->table) return OP_ARRAY_FAILURE;
    memset(array->table, 0, capacity * sizeof(void**));

	return OP_ARRAY_SUCCESS;
}

int array_insert_before(array *array, int index, void *item)
{
    assert(array != NULL);
    if(index >= array->size || item == NULL || index < 0)
    {
        return OP_ARRAY_FAILURE;
    }
    if(array->size >= array->capacity)
    {
        //no capacity, need realloc.
        size_t new_capacity = array->capacity + array->capacity / 2;
        void **newtable = (void**)realloc(array->table, new_capacity * sizeof(void**));
        if(newtable == NULL)
        {
            //no memory.
            return OP_ARRAY_FAILURE;
        }
        else
        {
            array->table = newtable;
            array->capacity = new_capacity;
        }
    }
    //do insert.
    int i;
    for(i = array->size; i >= index; --i)
    {
        array->table[i] = array->table[i - 1];
    }
    array->table[index] = (void*)item;
    ++(array->size);
    return OP_ARRAY_SUCCESS;
}

int array_index_of(array *array, void *item)
{
    assert(array != NULL);
    int i, index = OP_ARRAY_FAILURE;
    if(item == NULL)
    {
        return OP_ARRAY_FAILURE;
    }
    for(i = 0; i < array->size; ++i)
    {
        if(array->table[i] == (void*)item)
        {
            index = i;
            break;
        }
    }
    return index;
}

void *array_remove_last(array *array)
{
    assert(array != NULL);
    if(array->size <= 0)
    {
        return NULL;
    }
    void *last = array->table[array->size - 1];
    --(array->size);
    return last;
}

void *array_remove_first(array *array)
{
    assert(array != NULL);
    if(array->size <= 0)
    {
        return NULL;
    }
    void *first = array->table[0];
    int i;
    for(i = 1; i < array->size; ++i)
    {
        array->table[i - 1] = array->table[i];
    }
    --(array->size);
    return first;
}

void *array_remove_by_index(array *array, int index)
{
    assert(array != NULL);
    if(index < 0 || index >= array->size)
    {
        return NULL;
    }
    void *item = array->table[index];
    int i;
    for(i = index + 1; i < array->size; ++i)
    {
        array->table[i - 1] = array->table[i];
    }
    --(array->size);
    return item;
}

int array_push_back(array *array, void *item)
{
    assert(array != NULL);
    if(array->size < array->capacity)
    {
        //have capacity.
        array->table[array->size] = (void*) item;
        ++(array->size);
        return OP_ARRAY_SUCCESS;
    }
    else
    {
        //no capacity, need realloc.
        size_t new_capacity = array->capacity + array->capacity / 2;
        void **newtable = (void**)realloc(array->table, new_capacity * sizeof(void**));
        if(newtable != NULL)
        {
            array->table = newtable;
            array->capacity = new_capacity;
            array->table[array->size] = (void*) item;
            ++(array->size);
            return OP_ARRAY_SUCCESS;
        }
        else
        {
            //no memory.
            return OP_ARRAY_FAILURE;
        }
    }
}

void array_free_shallow(array *array)
{
    assert(array != NULL);
    free(array->table);
    array->size = 0;
    array->capacity = 0;
    array->table = NULL;
}


void array_free_deep(array *array)
{
    assert(array != NULL);
    int i;
    for(i = 0; i < array->size; ++i)
    {
        if(array->table[i] != NULL)
        {
            free(array->table[i]);
        }
    }
    free(array->table);
    array->size = 0;
    array->capacity = 0;
    array->table = NULL;
}

//检查该元素在数组中是否存在
int array_exist(array *array, void *item)
{
    void *temp;
    array_foreach(array, temp, 0)
    {
        if (temp == item)
            return 1;
    }

    return 0;
}
