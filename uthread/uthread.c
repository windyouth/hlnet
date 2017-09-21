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
#include "../common/common.h"
#include "../common/algorithm.h"


//协程参数
typedef struct _uthread_arg
{
	schedule_t			*schedule;			//所属调度器
	void				*parameter;			//参数
}uthread_arg_t;


/*
 * 构造调度器
 */
schedule_t *schedule_create()
{
	/* 构造调度器 */
	schedule_t *scheduler = (schedule_t *)malloc(sizeof(schedule_t));
	if (!scheduler) return NULL;
	zero(scheduler);
	
	int len = MAX_UTHREAD_SIZE * sizeof(uthread_t);
	scheduler->threads = (uthread_t *)malloc(len);
	if (!scheduler->threads) return NULL;
	zero(scheduler->threads);

	return scheduler;
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

    if(id != -1)
	{
        uthread_t *t = &(ps->threads[id]);

        t->state = UTHREAD_STATE_RUNNING;
		ps->active_count++;

        t->func(t->arg);

        t->state = UTHREAD_STATE_FREE;
		if(ps->active_count > 0) ps->active_count--;

        ps->running_thread = -1;
    }
}

int uthread_create(schedule_t *sche, Fun func)
{
	if (!sche) return PARAM_ERROR;
	
	int id = 0;
    
    for(id = 0; id < sche->max_index; ++id)
	{
        if(sche->threads[id].state == UTHREAD_STATE_FREE)
		{
            break;
        }
    }
    
    if (id == sche->max_index) 
	{
		if (sche->max_index == MAX_UTHREAD_SIZE)
			return FULL_ERROR;
        sche->max_index++;
    }

    uthread_t *t = &(sche->threads[id]);

    t->state = UTHREAD_STATE_READY;
    t->func = func;
    t->arg = sche;

    getcontext(&(t->ctx));
    
    t->ctx.uc_stack.ss_sp = t->stack;
    t->ctx.uc_stack.ss_size = DEFAULT_STACK_SZIE;
    t->ctx.uc_stack.ss_flags = 0;
    t->ctx.uc_link = &(sche->main);
    sche->running_thread = id;
    
    makecontext(&(t->ctx), (void (*)(void))(uthread_body), 1, sche);
    swapcontext(&(sche->main), &(t->ctx));
    
    return id;
}

/*
 * 运转协程调度器
 */
void uthread_run(schedule_t *sche)
{
	int id;
	while (sche->active_count)
	{
		for (id = 0; id < sche->max_index; ++id)
			uthread_resume(sche, id);
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


#endif
