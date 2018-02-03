#ifndef _TIMER_H_
#define _TIMER_H_

#ifdef _HLNET_		//如果是项目内部
#include "../c-stl/heap.h"
#else
#include "heap.h"
#endif

//定时器结构体
typedef struct _timer
{
	as_heap_node;		//堆节点头部

	int				at;						//执行时刻
	int 			repeat;					//重复次数，-1为无限次。
	void	 		*data;					//数据
	int (*cb)(struct _timer *timer);		//回调函数
}_timer;

//创建定时器管理器
int timer_manager();
//取出一个定时器
_timer *extract_timer();

//添加定时器
int add_timer(_timer *timer);
//删除定时器
int del_timer(_timer *timer);

#endif //_TIMER_H_
