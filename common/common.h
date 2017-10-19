#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>

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


//网络数据包头
typedef struct _cmd_head
{
	uint32_t		data_size;			//数据大小
	uint16_t		cmd_code;			//命令码
	uint16_t		proto_ver;			//协议版本号
}cmd_head_t, *pcmd_head;

//队列元素头
typedef struct _packet_head
{
	uint32_t 		client_id;			//客户端编号	
	cmd_head_t		head;				//网络数据包头
}packet_head_t, *ppacket_head;

//连接事件
typedef int (*link_hander)(int client_id, uint32_t ip);
//关闭事件
typedef int (*shut_hander)(int client_id);

//TCP消息函数
typedef int (*tcpmsg_hander)(int client_id, cmd_head_t *head, char *data);
//UDP消息函数，ip和port是大端(网络序)
typedef int (*udpmsg_hander)(uint32_t ip, uint16_t port, cmd_head_t *head, char *data);
//UDP读取函数
typedef void (*udp_reader)(int fd);
//数据库消息函数
typedef int (*dbmsg_hander)(char *data, uint32_t len);

typedef			unsigned char		uchar;			//无符号8位整数

#endif //_COMMON_H_

