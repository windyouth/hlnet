/**
* @file  thread.h
* @author chenxueyou
* @modifier heluan
* @version 0.1
* @brief A asymmetric coroutine library for C
* History
*   1. Date: 2014-12-12 
*      Author: chenxueyou
*      Modification: this file was created 
*	2. Date: 2017-07-28
*      Author: heluan
*      Modification: translate C++ to C and add sleep function
*/

#ifndef MY_UTHREAD_H
#define MY_UTHREAD_H

#ifdef __APPLE__
#define _XOPEN_SOURCE
#endif 

#include <ucontext.h>

#define DEFAULT_STACK_SZIE (1024*128)
#define MAX_UTHREAD_SIZE   1024

//错误码
#define 				UTHREAD_ERR_SUCCESS					0				//成功
#define 				UTHREAD_ERR_ERROR					-1				//失败

//协程状态
enum uthread_state
{
	UTHREAD_STATE_FREE,					//空闲
	UTHREAD_STATE_READY,				//就绪
	UTHREAD_STATE_RUNNING,				//运行
	UTHREAD_STATE_SUSPEND				//暂停
};

typedef void (*Fun)(void *arg);

typedef struct uthread_t
{
    ucontext_t ctx;
    Fun func;
    void *arg;
    enum uthread_state state;
    char stack[DEFAULT_STACK_SZIE];
}uthread_t;

//调度器结构体
typedef struct _schedule_t
{
    ucontext_t main;
    int running_thread;
    uthread_t *threads;
    int max_index; 				// 曾经使用到的最大的index + 1
}schedule_t;

/*
 * 构造调度器
 */
int schedule_create();

/*help the thread running in the schedule*/
static void uthread_body(schedule_t *ps);

/*Create a user's thread
*    @param[in]:
*        schedule_t &schedule 
*        Fun func: user's function
*        void *arg: the arg of user's function
*    @param[out]:
*    @return:
*        return the index of the created thread in schedule
*/
int uthread_create(Fun func);

/*
 * 将创建的协程加入到运行数组中
 */
int uthread_add(int id);

/*
 * 运转协程调度器
 */
int uthread_run();

/* Hang the currently running thread, switch to main thread */
void uthread_yield(schedule_t *schedule);

/* resume the thread which index equal id*/
void uthread_resume(schedule_t *schedule, int id);

/* decription: 协程休眠
 * parameter: millisec-要休眠的毫秒数
 */
void uthread_sleep(schedule_t* schedule, int msec);

/*test whether all the threads in schedule run over
* @param[in]:
*    const schedule_t & schedule 
* @param[out]:
* @return:
*    return 1 if all threads run over,otherwise return 0
*/
int  schedule_finished(const schedule_t *schedule);

#endif
