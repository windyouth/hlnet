#ifndef _HEAP_H_
#define _HEAP_H_

#include <sys/types.h>

#define			HEAP_SUCCESS		0							//成功
#define			HEAP_FAILURE		-1							//失败
#define			HEAP_PARAM_ERROR	-2							//参数错误
#define			HEAP_MEM_ERROR		-3							//参数错误

#define			cmp					<							//大顶堆：>; 小顶堆：<
#define 		as_heap_item		heap_item	heap_item_		//堆元素头

#define			heap_top(heap)		(heap)->table[1]					//取得顶元素
#define			heap_empty(heap)	((heap)->count == 0)				//是否为空
#define			heap_full(heap)		((heap)->count + 1 == (heap)->size)	//是否已满

//堆元素头
typedef struct _heap_item
{
	ulong			key;			//比较大小的依据
}heap_item;

//堆结构体
typedef struct _heap
{
	uint			size;			//大小
	uint			count;			//堆中元素个数
	heap_item		**table;		//存储堆元素的数组
}heap;

//初始化堆
int heap_init(heap *heap, uint size);
//加入元素
int heap_push(heap *heap, heap_item *item);
//弹出元素
heap_item *heap_pop(heap *heap);
//上浮
int heap_up(heap *heap, uint i);
//下沉
int heap_down(heap *heap, uint i);

#endif //_HEAP_H_
