#include <stdlib.h>
#include "timer.h"
#include "../common/store.h"

static store_t			*g_timer_store = NULL;			//定时器仓库

//释放定时器
#define			recycle_timer(timer)		recycle_chunk(g_timer_store, timer)

//创建定时器管理器
int timer_manager()
{
	return 0;
}

//取出一个定时器
_timer *extract_timer()
{
	return NULL;
}

//添加定时器
int add_timer(_timer *timer)
{
	return 0;
}

//删除定时器
int del_timer(_timer *timer)
{
	return 0;
}
