/**
* file: uthread.c
* author: chenxueyou
* modifier: heluan
* version: 0.1
* brief: A asymmetric coroutine library for C
* History
*   1. Date: 2014-12-12 
*      Author: chenxueyou
*      Modification: this file was created 
*	2. Date: 2017-07-28
*      Author: heluan
*      Modification: translate C++ to C and add sleep function
*/

#ifndef MY_UTHREAD_CPP
#define MY_UTHREAD_CPP

#include <sys/time.h>
#include "uthread.h"
#include "../c-stl/array.h"
#include "../common/common.h"
#include "../common/algorithm.h"

//协程参数
typedef struct _uthread_arg
{
	schedule_t			*schedule;			//所属调度器
	void				*parameter;			//参数
}uthread_arg_t;

schedule_t				*g_schedule = NULL;						//调度器
array				 	*g_uthread_array = NULL;				//协程数组


/*
 * 构造调度器
 */
int schedule_create()
{
	/* 构造调度器 */
	g_schedule = (schedule_t *)malloc(sizeof(schedule_t));
	if (!g_schedule) return MEM_ERROR;
	zero(g_schedule);
	
	int len = MAX_UTHREAD_SIZE * sizeof(uthread_t);
	g_schedule->threads = (uthread_t *)malloc(len);
	if (!g_schedule->threads) return MEM_ERROR;
	zero(g_schedule->threads);

	/* 构造协程数组 */
	g_uthread_array = (array *)malloc(sizeof(array));
	if (!g_uthread_array) return MEM_ERROR;
	zero(g_uthread_array);

	return array_init(g_uthread_array, 16);
}

void uthread_resume(schedule_t *schedule, int id)
{
    if(id < 0 || id >= schedule->max_index){
        return;
    }

    uthread_t *t = &(schedule->threads[id]);

    if (t->state == UTHREAD_STATE_SUSPEND) {
        swapcontext(&(schedule->main), &(t->ctx));
    }
}

void uthread_yield(schedule_t *schedule)
{
    if(schedule->running_thread != -1 )
	{
        uthread_t *t = &(schedule->threads[schedule->running_thread]);
        t->state = UTHREAD_STATE_SUSPEND;
        schedule->running_thread = -1;

        swapcontext(&(t->ctx), &(schedule->main));
    }
}

void uthread_body(schedule_t *ps)
{
    int id = ps->running_thread;

    if(id != -1){
        uthread_t *t = &(ps->threads[id]);

        t->func(t->arg);

        t->state = UTHREAD_STATE_FREE;
        
        ps->running_thread = -1;
    }
}

int uthread_create(Fun func)
{
	if (!g_schedule) return UTHREAD_ERR_ERROR;
	
	int id = 0;
    
    for(id = 0; id < g_schedule->max_index; ++id )
	{
        if(g_schedule->threads[id].state == UTHREAD_STATE_FREE)
		{
            break;
        }
    }
    
    if (id == g_schedule->max_index) {
        g_schedule->max_index++;
    }

    uthread_t *t = &(g_schedule->threads[id]);

    t->state = UTHREAD_STATE_READY;
    t->func = func;
    t->arg = g_schedule;

    getcontext(&(t->ctx));
    
    t->ctx.uc_stack.ss_sp = t->stack;
    t->ctx.uc_stack.ss_size = DEFAULT_STACK_SZIE;
    t->ctx.uc_stack.ss_flags = 0;
    t->ctx.uc_link = &(g_schedule->main);
    g_schedule->running_thread = id;
    
    makecontext(&(t->ctx), (void (*)(void))(uthread_body), 1, g_schedule);
    swapcontext(&(g_schedule->main), &(t->ctx));
    
    return id;
}

/*
 * 将创建的协程加入到运行数组中
 */
int uthread_add(int id)
{
	array_push_back(g_uthread_array, id);
}

/*
 * 运转协程调度器
 */
int uthread_run()
{
	int id = NULL;
	while (!schedule_finished(g_schedule))
	{
		array_foreach(g_uthread_array, id, 0)
		{
			uthread_resume(g_schedule, id);
		}
	}
}

/* 
 * decription: 协程休眠
 * parameter: millisec-要休眠的毫秒数 
 */
void uthread_sleep(schedule_t* schedule, int msec)
{
	struct timeval start, now;
	gettimeofday(&start, 0);
	gettimeofday(&now, 0);
	int diff = 0;

	while (diff < msec)
	{
		uthread_yield(schedule);
		gettimeofday(&now, 0);
		diff = (now.tv_sec * 1000 + now.tv_usec / 1000) - 
				(start.tv_sec * 1000 + start.tv_usec / 1000);
	}
}

int schedule_finished(const schedule_t *schedule)
{
    if (schedule->running_thread != -1)
	{
        return 0;
    }
	else
	{
        for(int i = 0; i < schedule->max_index; ++i)
		{
            if(schedule->threads[i].state != UTHREAD_STATE_FREE)
			{
                return 0;
            }
        }
    }

    return 1;
}

#endif
