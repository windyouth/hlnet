#ifndef _PROTO_H_
#define _PROTO_H_ 

#include <sys/types.h>

//网络数据包头
typedef struct _cmd_head
{
	uint		data_size;			//数据部分大小
	ushort		cmd_code;			//命令码
	ushort		proto_ver;			//协议版本号
}cmd_head_t, *pcmd_head;

//TCP消息函数
//head中需要用到data_size和proto_ver两个值，cmd_code不需要关注。
typedef int (*tcpmsg_hander)(int client_id, cmd_head_t *head, char *data);
//UDP消息函数，ip和port是大端(网络序)
typedef int (*udpmsg_hander)(uint ip, ushort port, cmd_head_t *head, char *data);

#endif //_PROTO_H_
