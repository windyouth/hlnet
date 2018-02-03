#ifndef _HEAP_H_
#define _HEAP_H_

#include <sys/types.h>

#define			HEAP_SUCCESS		0							//成功
#define			HEAP_FAILURE		-1							//失败
#define			HEAP_PARAM_ERROR	-2							//参数错误
#define			HEAP_MEM_ERROR		-3							//参数错误

#define			cmp					<							//大顶堆：>; 小顶堆：<

#define			heap_top(heap)		(heap)->table[1]					//取得顶元素
#define			heap_empty(heap)	((heap)->count == 0)				//是否为空
#define			heap_full(heap)		((heap)->count + 1 == (heap)->size)	//是否已满

//遍历堆数组
#define heap_foreach(heap, item) 			\
			for (int i = 1; (item) = (heap)->table[i], i <= (heap)->count; ++i)

//判断并调整到合适位置
#define heap_adjust(heap, k) do 							\
{															\
	if (k > 1 && heap->table[k] cmp heap->table[k >> 1])	\
		heap_up(heap, k);									\
	else													\
		heap_down(heap, k);									\
} while(0)


#define 		as_heap_node								\
	uint			index;			/* 在数组中的索引 */	\
	ulong			key				/* 比较大小的依据 */

//堆元素头
typedef struct _heap_node
{
	as_heap_node;					//比较大小的依据
}heap_node;

//堆结构体
typedef struct _heap
{
	uint			size;			//大小
	uint			count;			//堆中元素个数
	heap_node		**table;		//存储堆元素的数组
}heap;

//初始化堆
int heap_init(heap *heap, uint size);
//加入元素
int heap_push(heap *heap, heap_node *item);
//弹出元素
heap_node *heap_pop(heap *heap);
//上浮
int heap_up(heap *heap, uint k);
//下沉
int heap_down(heap *heap, uint i);
//移除
heap_node *heap_erase(heap *heap, uint k);

#endif //_HEAP_H_
