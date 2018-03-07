#ifndef _MOMENT_H_
#define _MOMENT_H_

/*
 * 绝对时间定时器
 */

#ifdef _HLNET_		//如果是项目内部
#include "../c-stl/heap.h"
#include "../coroutine/coroutine.h"
#else
#include "heap.h"
#include "coroutine.h"
#endif

//是否启用定时管理器
#define				use_moment			(g_moment_heap != NULL)

struct _moment;
//定时器回调函数
typedef int (*moment_cb)(struct _moment *moment);

//定时器结构体
typedef struct _moment
{
	as_heap_node;		//堆节点头部

	void	 		*data;						//数据
	moment_cb		cb;							//回调函数
}_moment;


//创建定时器管理器
int moment_manager();
//添加定时器
_moment *add_moment(time_t at, moment_cb cb, void *data);
//删除定时器
int del_moment(_moment *moment);
//检查定时器
void check_moments(struct schedule *sche, void *arg);

extern heap				*g_moment_heap;				//堆管理器

#endif //_TIMER_H_
