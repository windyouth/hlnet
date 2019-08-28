#ifndef _PROTO_H_
#define _PROTO_H_ 

#include <sys/types.h>

#ifdef _HLNET_
    #include "../src/server.h"
#else
    #include "server.h"
#endif

//通信的类型
typedef enum _sock_type
{
    sock_type_user = 1,
    sock_type_manage = 2,
    sock_type_udp = 3
}sock_type;

//网络数据包头
typedef struct _cmd_head
{
	uint		data_size;			//数据部分大小
	ushort		cmd_code;			//命令码
	ushort		proto_ver;			//协议版本号
}cmd_head_t, *pcmd_head;

//协议部分初始化
int proto_init();
//监听端口
int listen_port(sock_type type, ushort port);

//TCP消息函数
//head中需要用到data_size和proto_ver两个值，cmd_code不需要关注。
typedef int (*tcpmsg_hander)(int client_id, cmd_head_t *head, char *data);
//UDP消息函数，ip和port是大端(网络序)
typedef int (*udpmsg_hander)(uint ip, ushort port, cmd_head_t *head, char *data);

//注册TCP消息
int reg_tcp_msg(sock_type sock_type, ushort msg, tcpmsg_hander func);
//注册UDP消息
int reg_udp_msg(ushort msg, udpmsg_hander func);

//发送数据(tcp)
int send_tcp_data(uint client_id, char *data, uint len);
//发送数据(udp) ip, port必须是大端(网络序)
int send_udp_data(uint ip, ushort port, char *data, uint len);

#endif //_PROTO_H_
