#include<stdlib.h>
#include<stdio.h>
#include<assert.h>
#include<string.h>

#ifndef __ARRAY_H_
#define __ARRAY_H_

#define ARRAY_INIT_CAPACITY 16

typedef struct _array
{
    size_t size;
    size_t capacity;
    void **table;
}array;

#define 			OP_ARRAY_SUCCESS 				(0)
#define 			OP_ARRAY_FAILURE 				(-1)

//数组大小
#define 			array_size(array) 				(array)->size
//根据索引查询
#define 			array_find(array, index)		(array)->table[index]	

#define array_foreach(array, temp, from)							\
           for(int i = (from);										\
               (temp) = *((array)->table + i), i < (array)->size;	\
               ++i)


int array_init(array *array, size_t capacity);

int array_insert_before(array *array, int index, void *item);

int array_index_of(array *array, void *item);

void *array_remove_last(array *array);

void *array_remove_first(array *array);

void *array_remove_by_index(array *array, int index);

int array_push_back(array *array, void *item);

void array_free_shallow(array *array);

void array_free_deep(array *array);

#endif

