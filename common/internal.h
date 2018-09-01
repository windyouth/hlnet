#ifndef _INTERNAL_H_
#define _INTERNAL_H_

/*
 * description: 库内部通用的数据结构定义
 * author: 何峦
 * email: heluan123132@163.com
 */

#include "common.h"
#include "../c-stl/map.h"


//消息处理函数在map中所用的结构
typedef struct _msg_func_item
{
	as_map_item;
	void 			*msg_func;			//消息处理函数指针
}msg_func_item, *pmsg_func_item;

#endif //_INTERNAL_H_
