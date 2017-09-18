#ifndef _EPOLLET_H_
#define	_EPOLLET_H_ 1

#include <stdint.h>
#include "buffer.h"
#include "../common/common.h"
#include "client_store.h"


//创建一个tcp套接字并监听端口
int create_tcp_socket(uint16_t port);
//创建udp套接字并绑定端口
int create_udp_socket(uint16_t port);

//--------------------------------------------------------------------
// description: 循环发送
// return: 实际发送的字节数，如小于零则为异常
//--------------------------------------------------------------------
int circle_send(int fd, char *buf, int len);

//--------------------------------------------------------------------
// description: 循环接收
// return: 实际接收的字节数，如小于零则为异常
//--------------------------------------------------------------------
int circle_recv(int fd, char *buf, int len);

//epollet创建函数
int epollet_create();
//epollet添加函数
int epollet_add(int fd, void *data_ptr, int flag);
//epollet运行函数
int epollet_run(void *arg);

//关闭套接字
void close_socket(struct epoll_event *ev);

//全局变量
extern int 				g_client_tcp_fd;		//监听的套接字ID(用户端)
extern int 				g_manage_tcp_fd;		//监听的套接字ID(管理端)
extern int 				g_udp_fd;				//监听的套接字ID(UDP)

extern buffer			*g_client_buf;			//数据队列(用户端)
extern buffer			*g_manage_buf;			//数据队列(管理端)

extern link_hander		g_client_link;			//连接事件函数指针(用户端)
extern shut_hander		g_client_shut;			//断开事件函数指针(用户端)

extern link_hander		g_manage_link;			//连接事件函数指针(管理端)
extern shut_hander		g_manage_shut;			//断开事件函数指针(管理端)

extern udp_reader		g_udp_reader;			//udp读取函数指针


#endif //_EPOLLET_H_
