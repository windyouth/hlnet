#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "heap.h"

//交换两个元素的值
#define	heap_swap(a, b, temp) do				\
{												\
	temp = a;									\
	a = b;										\
	b = temp;									\
} while(0)

//初始化堆
int heap_init(heap *heap, uint size)
{
	//参数检查
	assert(heap);
	if (!heap) return HEAP_PARAM_ERROR;

	//申请内存
	int len = size * sizeof(heap_node *);
	heap->table = (heap_node **)malloc(len);
	assert(heap->table != NULL);
	if (!heap->table) return HEAP_MEM_ERROR;

	//初始化值
	heap->size = size;
	heap->count = 0;
	bzero(heap->table, len);

	return HEAP_SUCCESS;
}

//加入元素
int heap_push(heap *heap, heap_node *item)
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
		heap_node **new_table = (heap_node **)realloc(heap->table, 
								new_size * sizeof(heap_node *));
		if (!new_table) return HEAP_MEM_ERROR;

		heap->table = new_table;
		heap->size = new_size;
	}

	//添加数据
	heap->table[++heap->count] = item;

	//上浮尾结点并返回
	return heap_up(heap, heap->count);
}

//弹出元素
heap_node *heap_pop(heap *heap)
{
	//参数检查
	assert(heap && heap->count > 0);
	if (!heap || heap->count == 0) return NULL;

	heap_node *temp;
	heap_swap(heap->table[1], heap->table[heap->count], temp);
	temp = heap->table[heap->count];
	heap->count--;
	//下沉根结点
	heap_down(heap, 1);

	return temp;
}

//上浮
int heap_up(heap *heap, uint i)
{
	//参数检查
	assert(heap && i > 0);
	if (!heap || i == 0) return HEAP_PARAM_ERROR; 

	heap_node *temp;

	while (i > 1)	/* 如果不是根结点 */
	{
		if (heap->table[i]->key cmp heap->table[i >> 1]->key)
			heap_swap(heap->table[i], heap->table[i >> 1], temp);

		//跳到父结点
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
	heap_node *temp;

	while (i << 1 <= heap->count)	/* 如果还有子结点 */
	{
		next = i << 1;
		//是否还有右叶子并比较两个叶子结点的值
		if (next < heap->count && heap->table[next + 1]->key cmp heap->table[next]->key)
			++next;

		if (heap->table[next]->key cmp heap->table[i]->key)
		{
			heap_swap(heap->table[next], heap->table[i], temp);
			//转到新位置
			i = next;
		}
		else
		{
			return HEAP_SUCCESS;
		}
	}

	return HEAP_SUCCESS;
}

//移除
heap_node *heap_erase(heap *heap, uint k)
{
	//参数检查
	assert(heap && k > 0);
	if (!heap || k == 0 || k > heap->count) return NULL; 

	//尾结点填充过来并重新整理顺序
	heap_node *temp = heap->table[k];
	heap->table[k] = heap->table[heap->count--];
	heap_adjust(heap, k);

	return temp;
}
