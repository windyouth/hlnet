#ifndef _SERVER_H_
#define _SERVER_H_

#ifdef _HLNET_		//如果是项目内部
#include "../common/common.h"
#else
#include "common.h"
#endif


#define				MAX_UDP_LENGTH				2048				//UDP数据包最大长度

//监听方式
typedef enum _sock_type 
{
	socktype_tcp = 1,	    		//tcp方式	
    socktype_udp = 2				//udp方式
}sock_type;


//创建服务器
int serv_create();
//添加服务器参数
int serv_ctl(sock_type type, short port);
//运行服务器
int serv_run();

//设置初次接收数据包的长度
int set_first_length(sock_type sock_type, uint length);
//设置下次接收数据包的长度
int set_next_length(uint client_id, uint length);

//注册连接事件处理函数
int reg_link_event(sock_type type, link_hander func);
//注册中断事件处理函数
int reg_shut_event(sock_type type, shut_hander func);

//注册TCP处理函数
int reg_net_msg(sock_type sock_type, tcpmsg_hander func);
//注册UDP消息
int reg_udp_msg(uint16_t msg, udpmsg_hander func);

//发送数据(tcp)
int tcp_send(uint client_id, uint16_t cmd, char *data, uint len);
//发送数据(udp) ip, port必须是大端(网络序)
int udp_send(uint ip, uint16_t port, uint16_t cmd, char *data, uint len);

#endif //_SERVER_H_
