#ifndef _EPOLLET_H_
#define	_EPOLLET_H_ 1

#include "client.h"
#include "../coroutine/coroutine.h"

//UDP相关参数
typedef struct _udp_fd
{
    int         fd;         //UDP套接字
    char        *buf;       //缓冲区
}udp_fd;

//UDP读取函数
typedef void (*udp_reader)(udp_fd *ufd);

//创建客户端监听套接字
int create_tcp_fd(uint16_t port);
//创建UDP套接字
int create_udp_fd(uint16_t port);

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
void epollet_run(struct schedule *sche, void *arg);

//关闭套接字
void close_socket(client_t *cli);

//全局变量
extern int 				g_udp_fd;				//监听的套接字ID(UDP)

extern list             *g_ready_list;          //就绪链表

extern link_hander		g_tcp_link;			    //连接事件函数指针(用户端)
extern shut_hander		g_tcp_shut;			    //断开事件函数指针(用户端)

extern udp_reader		g_udp_reader;			//udp读取函数指针

extern uint8_t			g_is_keep_alive;		//是否保持长连接

extern uint             g_first_need;         //TCP端首次接收的数据包长度

#endif //_EPOLLET_H_
