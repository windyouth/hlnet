#ifndef _SERVER_H_
#define _SERVER_H_

#ifdef _HLNET_		//如果是项目内部
#include "../common/common.h"
#else
#include "common.h"
#endif



//监听方式
typedef enum _sock_type 
{
	socktype_tcp = 1,	    		//tcp方式	
    socktype_udp = 2				//udp方式
}sock_type;


//创建服务器
int serv_create();
//监听tcp端口
int listen_tcp(short port, cb_guide guide, cb_tcp hander);
//监听udp端口
int listen_udp(short port, cb_udp hander);
//运行服务器
int serv_run();

//设置初次接收数据包的长度
void set_first_need(uint need);
//设置下次接收数据包的长度
int set_need(uint client_id, uint need);

//注册连接事件处理函数
void reg_link_event(link_hander func);
//注册中断事件处理函数
void reg_shut_event(shut_hander func);

//发送数据(tcp)
int send_tcp(uint client_id, char *data, uint len);
//发送数据(udp) ip, port必须是大端(网络序)
int send_udp(uint ip, ushort port, char *data, uint len);

#endif //_SERVER_H_
