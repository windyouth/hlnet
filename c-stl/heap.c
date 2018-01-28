#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "heap.h"
#include "../common/algorithm.h"

//初始化堆
int heap_init(heap *heap, uint size)
{
	//参数检查
	assert(heap);
	if (!heap) return HEAP_PARAM_ERROR;

	//申请内存
	int len = size * sizeof(heap_item *);
	heap->table = (heap_item **)malloc(len);
	assert(heap->table != NULL);
	if (!heap->table) return HEAP_MEM_ERROR;

	//初始化值
	heap->size = size;
	heap->count = 0;
	bzero(heap->table, len);

	return HEAP_SUCCESS;
}

//加入元素
int heap_push(heap *heap, heap_item *item)
{
	//参数检查
	assert(heap != NULL && item != NULL);
	if (NULL == heap || NULL == item)
	{
		return HEAP_PARAM_ERROR;
	}

	//如果满了，扩展内存。
	if (heap_full(heap))
	{
		int new_size = heap->size + heap->size / 2;
		heap_item **new_table = (heap_item **)realloc(heap->table, 
								new_size * sizeof(heap_item *));
		if (!new_table) return HEAP_MEM_ERROR;

		heap->table = new_table;
		heap->size = new_size;
	}

	//添加数据
	heap->table[++heap->count] = item;

	return heap_up(heap, heap->count);
}

//弹出元素
heap_item *heap_pop(heap *heap)
{
	//参数检查
	assert(heap && heap->count > 0);
	if (!heap || heap->count == 0) return NULL;

	heap_item *temp;
	swap(heap->table[1], heap->table[heap->count], temp);
	temp = heap->table[heap->count];
	heap->count--;
	heap_down(heap, 1);

	return temp;
}

//上浮
int heap_up(heap *heap, uint i)
{
	//参数检查
	assert(heap && i > 0);
	if (!heap || i == 0) return HEAP_PARAM_ERROR; 

	heap_item *temp;

	while (i > 1)
	{
		if (heap->table[i]->key cmp heap->table[i >> 1]->key)
			swap(heap->table[i], heap->table[i >> 1], temp);

		i >>= 1;
	}

	return HEAP_SUCCESS;
}

//下沉
int heap_down(heap *heap, uint i)
{
	//参数检查
	assert(heap && i > 0);
	if (!heap || i == 0) return HEAP_PARAM_ERROR; 

	uint next;
	heap_item *temp;

	while (i << 1 <= heap->count)
	{
		next = i << 1;
		if (next < heap->count && heap->table[next + 1]->key cmp heap->table[next]->key)
			++next;

		if (heap->table[next]->key cmp heap->table[i]->key)
		{
			swap(heap->table[next], heap->table[i], temp);
			i = next;
		}
		else
		{
			return HEAP_SUCCESS;
		}
	}

	return HEAP_SUCCESS;
}
