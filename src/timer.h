#ifndef _TIMER_H_
#define _TIMER_H_

#ifdef _HLNET_		//如果是项目内部
#include "../c-stl/heap.h"
#else
#include "heap.h"
#endif

//是否启用定时管理器
#define				use_timer			(g_timers != NULL)

struct _timer;
//定时器回调函数
typedef int (*timer_cb)(struct _timer *timer);

//定时器结构体
typedef struct _timer
{
	as_heap_node;		//堆节点头部

	int 			repeat;					//重复次数，-1为无限次。
	void	 		*data;					//数据
	timer_cb		cb;						//回调函数
}_timer;


//创建定时器管理器
int timer_manager();
//添加定时器
_timer *add_timer(int after, int repeat, timer_cb cb, void *data);
//删除定时器
int del_timer(_timer *timer);


extern heap					*g_timers;				//堆管理器

#endif //_TIMER_H_
