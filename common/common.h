#ifndef _COMMON_H_
#define _COMMON_H_

#include <sys/types.h>
#ifdef _HLNET_
#include "../c-stl/map.h"
#else
#include "map.h"
#endif

//错误码
#define				SUCCESS					0					//成功
#define				FAILURE					-1					//失败
#define				PARAM_ERROR				-2					//参数错误
#define				MEM_ERROR				-3					//内存错误
#define				NET_ERROR				-4					//网络错误
#define				IO_ERROR				-5					//IO错误
#define				FULL_ERROR				-6					//容器已满
#define				EMPTY_ERROR				-7					//容器已空
#define				REPEAT_ERROR			-8					//重复操作

#define 			INVALID_SOCKET			-1					//非法套接字编号

#define				YES						1
#define				NO						0

#define				PATH_LENGTH				256					//地址长度
#define				MAX_UDP_LENGTH			2048				//UDP数据包最大长度

//消息处理函数在map中所用的结构
typedef struct _msg_func_item
{
	as_map_item;
	void 			*msg_func;			//消息处理函数指针
}msg_func_item, *pmsg_func_item;

//连接事件
typedef int (*link_hander)(int client_id, uint ip);
//关闭事件
typedef int (*shut_hander)(int client_id);

//TCP消息函数
typedef int (*cb_tcp)(int client_id);
//引导epoll边缘触发接收数据长度的函数
typedef int (*cb_guide)(int client_id);
//UDP消息函数，ip和port是大端(网络序)
typedef int (*cb_udp)(uint ip, ushort port, char *data, uint len);
//数据库消息函数
typedef int (*dbmsg_hander)(char *data, uint len);

typedef			unsigned char		uchar;			//无符号8位整数

#define			TEST			//测试宏

#endif //_COMMON_H_

