#ifndef _SERVER_H_
#define _SERVER_H_

#ifdef _HLNET_		//如果是项目内部
#include "../common/common.h"
#else
#include "common.h"
#endif

//创建服务器
int serv_init();
//监听tcp端口
int listen_tcp(ushort port, ushort need, cb_guide guide, cb_tcp hander);
//监听udp端口
int listen_udp(short port, cb_udp hander);
//运行服务器
int serv_run();

//注册连接事件处理函数
void reg_link_event(int listen_fd, link_hander func);
//注册中断事件处理函数
void reg_shut_event(int listen_fd, shut_hander func);

//发送数据(tcp)
int send_tcp(uint client_id, char *data, uint len);
//发送数据(udp) ip, port必须是大端(网络序)
int send_udp(int fd, uint ip, ushort port, char *data, uint len);

#endif //_SERVER_H_
