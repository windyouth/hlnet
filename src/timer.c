#include <stdlib.h>
#include "timer.h"
#include "../common/common.h"
#include "../common/store.h"

static store_t			*g_timer_store = NULL;			//定时器仓库
heap					*g_timers = NULL;				//定时器小顶堆

//取出定时器
#define			extract_timer()				(_timer *)extract_chunk(g_timer_store)
//释放定时器
#define			recycle_timer(timer)		recycle_chunk(g_timer_store, timer)

//创建定时器管理器
int timer_manager()
{
	//创建仓库
	if (!g_timer_store)
	{
		g_timer_store = create_store(sizeof(_timer));
		if (!g_timer_store) return FAILURE;
	}

	//创建管理堆容器
	if (!g_timers)
	{
		g_timers = (heap *)malloc(sizeof(heap));
		if (!g_timers) return MEM_ERROR;

		if (HEAP_SUCCESS != heap_init(g_timers, 128))
			return FAILURE;
	}

	return SUCCESS;
}

//添加定时器
_timer *add_timer(int after, int repeat, timer_cb cb, void *data)
{
	//取出一个定时器
	_timer *timer = extract_timer();
	if (!timer) return NULL;

	return NULL;
}

//删除定时器
int del_timer(_timer *timer)
{
	return 0;
}
