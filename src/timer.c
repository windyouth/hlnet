#include <stdlib.h>
#include <time.h>
#include "timer.h"
#include "../common/common.h"
#include "../common/store.h"

static store_t			*g_timer_store = NULL;			//定时器仓库
heap					*g_timers = NULL;				//定时器小顶堆
clock_t					g_now = 0;						//启动机器到现在的时间

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

	g_now = clock();
	if (g_now == -1) return FAILURE;

	return SUCCESS;
}

//添加定时器
_timer *add_timer(int after, int repeat, timer_cb cb, void *data)
{
	//取出一个定时器
	_timer *timer = extract_timer();
	if (!timer) return NULL;

	//设置参数
	timer->key = g_now + after; 
	timer->after = after;
	timer->repeat = repeat;
	timer->cb = cb;
	timer->data = data;

	//加入到堆中
	if (HEAP_SUCCESS != heap_push(g_timers, (heap_node *)timer))
		return NULL;

	return timer;
}

//删除定时器
int del_timer(_timer *timer)
{
	assert(timer);
	if (!timer) return PARAM_ERROR;

	if (NULL == heap_erase(g_timers, timer->index))
		return FAILURE;

	return SUCCESS;
}

//检查定时器
void check_timers(struct schedule *sche, void *arg)
{
	struct _timer *timer = NULL;

	for (;;)
	{
		//更新时间
		g_now = clock();

		//是否有已超时的定时器
		while(!heap_empty(g_timers) && heap_top(g_timers)->key < g_now) 
		{
			//执行回调函数
			timer = (struct _timer *)heap_top(g_timers);
			timer->cb(timer);

			//如果是无限次重复
			if (timer->repeat == -1)
			{
				timer->key += timer->after;
				heap_down(g_timers, timer->index);
			}
			else
			{
				//如果还有重复次数
				if (timer->repeat > 0) 
				{
					timer->repeat--;
					timer->key += timer->after;
					heap_down(g_timers, timer->index);
				}
				else //如果重复次数已用完
				{
					del_timer(timer);
				}
			}
		}//end while 

		//让出协程控制权
		coroutine_yield(sche);
	}//end for
}
