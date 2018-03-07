#include <stdlib.h>
#include <time.h>
#include "moment.h"
#include "../common/common.h"
#include "../common/store.h"

static store_t			*g_moment_store = NULL;			//定时器仓库
heap					*g_moment_heap = NULL;			//定时器小顶堆

//取出定时器
#define			extract_moment()			(_moment *)extract_chunk(g_moment_store)
//释放定时器
#define			recycle_moment(moment)		recycle_chunk(g_moment_store, moment)

//创建定时器管理器
int moment_manager()
{
	//创建仓库
	if (!g_moment_store)
	{
		g_moment_store = create_store(sizeof(_moment));
		if (!g_moment_store) return FAILURE;
	}

	//创建管理堆容器
	if (!g_moment_heap)
	{
		g_moment_heap = (heap *)malloc(sizeof(heap));
		if (!g_moment_heap) return MEM_ERROR;

		if (HEAP_SUCCESS != heap_init(g_moment_heap, 128))
			return FAILURE;
	}

	return SUCCESS;
}

//添加定时器
_moment *add_moment(time_t at, moment_cb cb, void *data)
{
	//取出一个定时器
	_moment *moment = extract_moment();
	if (!moment) return NULL;

	//设置参数
	moment->key = at; 
	moment->cb = cb;
	moment->data = data;

	//加入到堆中
	if (HEAP_SUCCESS != heap_push(g_moment_heap, (heap_node *)moment))
		return NULL;

	return moment;
}

//删除定时器
int del_moment(_moment *moment)
{
	assert(moment);
	if (!moment) return PARAM_ERROR;

	//从堆中移除
	if (NULL == heap_erase(g_moment_heap, moment->index))
		return FAILURE;

	//回收
	recycle_moment(moment);

	return SUCCESS;
}

//检查定时器
void check_moments(struct schedule *sche, void *arg)
{
	struct _moment *moment = NULL;
	time_t now;

	for (;;)
	{
		//更新时间
		now = time(NULL);

		//是否有已超时的定时器
		while(!heap_empty(g_moment_heap) && heap_top(g_moment_heap)->key < now) 
		{
			//执行回调函数
			moment = (struct _moment *)heap_top(g_moment_heap);
			moment->cb(moment);
			//执行完就删除
			del_moment(moment);
		}//end while 

		//让出协程控制权
		coroutine_yield(sche);
	}//end for
}
