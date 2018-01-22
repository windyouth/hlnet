#ifndef _HEAP_H_
#define _HEAP_H_

#include <sys/types.h>


#define			cmp					<							//大顶堆：>; 小顶堆：<
#define 		as_heap_item		heap_item	heap_item_		//堆元素头

#define			heap_top(heap)		(heap)->array[0]			//取得顶元素
#define			heap_empty(heap)	((heap)->count == 0)		//是否为空

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
	heap_item		**array;		//存储堆元素的数组
}heap;

//初始化堆
void heap_init(heap *heap, uint size);
//加入元素
int heap_push(heap *heap, heap_item *item);
//弹出元素
heap_item *heap_pop(heap *heap);
//上浮
int heap_up(heap *heap, int i);
//下沉
int heap_down(heap *heap, int i);

#endif //_HEAP_H_
