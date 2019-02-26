#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include "epollet.h"
#include "../common/internal.h"
#include "../c-stl/list.h"
#include "../c-stl/array.h"


#define				MAX_EVENT_COUNT			1024			//一次能接收的最多事件个数

#define				GLOBAL_BUF_ORIGIN_SIZE	4096 * 5		//全局缓冲区初始大小


static struct epoll_event  	*g_events = NULL;						//事件数组指针 
static int 					g_epoll_fd = INVALID_SOCKET;			//epoll元套接字

//全局变量
int 				g_udp_fd = INVALID_SOCKET;				//监听的套接字ID(UDP)

list                *g_ready_list = NULL;                   //就绪链表

link_hander			g_user_link = NULL;					    //连接事件函数指针(用户端)
shut_hander			g_user_shut = NULL;					    //断开事件函数指针(用户端)

link_hander			g_manage_link = NULL;					//连接事件函数指针(管理端)
shut_hander			g_manage_shut = NULL;					//断开事件函数指针(管理端)

udp_reader			g_udp_reader = NULL;					//udp读取函数指针

uint8_t				g_is_keep_alive = NO;					//是否保持长连接

uint                g_user_first_length = 0;                //用户端首次接收的长度
uint                g_manage_first_length = 0;              //管理端首次接收的长度

array               *g_tcp_listener = NULL;                 //tcp监听套接字数组
array               *g_udp_listener = NULL;                 //udp监听套接字数组


//设备套接字为非阻塞
static int set_nonblock(int fd)
{
	int opt = fcntl(fd, F_GETFL);
	if (opt < 0) return FAILURE;

	opt = opt|O_NONBLOCK;
	if (fcntl(fd, F_SETFL, opt) < 0)
		return FAILURE;
}

//创建一个tcp套接字并监听端口
static int create_tcp_socket(uint16_t port)
{
	//创建socket
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == INVALID_SOCKET) return FAILURE;

	set_nonblock(sock_fd);
	
	//TIME_WAIT过程中可重用该socket
	int sockopt = 1;
	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&sockopt, sizeof(sockopt));
	//没发完的数据发送出去后再关闭socket
	struct linger ling;
	ling.l_onoff = 1;
	ling.l_linger = 0;
	setsockopt(sock_fd, SOL_SOCKET, SO_LINGER, (const char *)&ling, sizeof(ling));

	//绑定端口
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;						//ipv4
	addr.sin_port = htons(port);
	//addr.sin_addr = *(get_addr());
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int res = bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr));
	if (res == FAILURE)
	{
		close(sock_fd);
		sock_fd = INVALID_SOCKET;
		return INVALID_SOCKET;
	}

	//监听端口
	if (listen(sock_fd, 32) == FAILURE)
	{
		close(sock_fd);
		sock_fd = INVALID_SOCKET;
		return INVALID_SOCKET;
	}

	return sock_fd;
}

//创建udp套接字并绑定端口
static int create_udp_socket(uint16_t port)
{
	int sock_fd = INVALID_SOCKET;

	//创建socket
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_fd < 0)
	{
		return FAILURE;
	}

	//TIME_WAIT过程中可重用该socket
	int sockopt = 1;
	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&sockopt, sizeof(sockopt));

	//绑定端口
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;						//ipv4
	addr.sin_port = htons(port);
	addr.sin_addr = *(get_addr());
	int res = bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr));
	if (res == FAILURE)
	{
		close(sock_fd);
		sock_fd = INVALID_SOCKET;
		return INVALID_SOCKET;
	}

	return sock_fd;
}

//将套接字添加进epoll监听链表
static int epoll_add(int fd, int flag)
{
	struct epoll_event ev;
	zero(&ev);
	ev.events = flag;
	ev.data.fd = fd;

	return epoll_ctl(g_epoll_fd, EPOLL_CTL_ADD, fd, &ev);
}

//创建客户端监听套接字
int create_tcp_fd(uint16_t port)
{
	int fd = create_tcp_socket(port);
	if (fd == INVALID_SOCKET) return INVALID_SOCKET;

	//注册epoll事件
	if (0 != epoll_add(fd, EPOLLIN | EPOLLET))
	{
		close(fd);
		return INVALID_SOCKET;
	}

    if (g_tcp_listener)
        array_push_back(g_tcp_listener, (void *)fd);

	return INVALID_SOCKET;
}

//创建UDP套接字
int create_udp_fd(uint16_t port)
{
	int fd = create_udp_socket(port);
	if (fd == INVALID_SOCKET) return INVALID_SOCKET;

	//注册epoll事件
	if (0 != epoll_add(fd, EPOLLIN))
	{
		close(fd);
		return INVALID_SOCKET;
	}

    if (g_tcp_listener)
        array_push_back(g_tcp_listener, (void *)fd);

	return INVALID_SOCKET;
}

//--------------------------------------------------------------------
// description: 循环发送
// return: 实际发送的字节数，如小于零则为异常
//--------------------------------------------------------------------
int circle_send(int fd, char *buf, int len)
{
	//参数检查
	if (fd == INVALID_SOCKET || buf == 0 || len <=0)
	{
		return PARAM_ERROR;
	}

	int sent = 0, actual = 0;
	
	do
	{
		actual = send(fd, buf + sent, len - sent, 0);
		if (actual < 0)
		{
			if (errno == EAGAIN)
			{
				continue;
			}
			if (errno == EINTR)
			{
				continue;
			}

			break;
		}

		sent += actual;

	}while (sent < len);

	return sent;
}

//--------------------------------------------------------------------
// description: 循环接收
// return: 实际接收的字节数，如小于零则为异常
//--------------------------------------------------------------------
int circle_recv(int fd, char *buf, int len)
{
	//参数检查
	assert(fd != INVALID_SOCKET && buf && len > 0);
	if (fd == INVALID_SOCKET || buf == 0 || len <= 0)
	{
		return PARAM_ERROR;
	}

	int received = 0, actual = 0, diff = 0;

	//循环接收
	do
	{
		actual = recv(fd, buf + received, len - received, MSG_DONTWAIT);

		//错误处理
		if (actual == 0)
		{
			break;
		}
		else if (actual < 0)
		{
			//被中断唤醒，需要继续读。
			if (errno == EINTR)
			{
				continue;
			}

			//非阻塞模式下，表示缓冲区被读空了
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				break;
			}

			return NET_ERROR;
		}

		received += actual;

	}while (received < len);

	return received;
}

//epollet创建函数
int epollet_create()
{
	g_epoll_fd = epoll_create(1024);
	if (g_epoll_fd == INVALID_SOCKET) return FAILURE;

	g_events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * MAX_EVENT_COUNT);
	if (!g_events) return MEM_ERROR;

   	//初始化就绪链表
	g_user_ready = list_create();
	if (!g_user_ready) return MEM_ERROR;
    
    //创建tcp监听套接字数组
    if (!g_tcp_listener)
    {
        //初始化监听套接字数组
        g_tcp_listener = (array *)malloc(sizeof(array));
        if (!g_tcp_listener) return MEM_ERROR;

        array_init(g_tcp_listener, 16);
    }
    
    //创建tcp监听套接字数组
    if (!g_udp_listener)
    {
        //初始化监听套接字数组
        g_udp_listener = (array *)malloc(sizeof(array));
        if (!g_udp_listener) return MEM_ERROR;

        array_init(g_udp_listener, 16);
    }

	return client_store_init();
}

//-----------------------------------------------------------
// description: 从内核中读数据
// return: >0 正常读数据
//		   =0 内核缓冲区已经读空
//		   <0 网络异常，关闭套接字
//-----------------------------------------------------------
static int read_data(struct epoll_event *ev)
{
	//参数检查
	assert(ev && ready_list);
	if (!ev || !ready_list) return PARAM_ERROR;

	client_t *cli = (client_t *)ev->data.ptr;
	//需要的已读完，读无可读。
	if (cli->need == 0) return SUCCESS;

	//调整缓冲区结构
	int res = buffer_rectify(cli->in, cli->need);
	if (res < 0)
	{
		//由于是边缘触发，为防止该套接字变僵尸，直接删除之。
		close_socket(cli);
		//只有客户端没有设置keep_alive时，为短连接，此时需要提前回收。
		//其他情况都是长连接，由keep_alive对象中的代码来进行回收。
		if (!g_is_keep_alive) 
			recycle_client(cli);

		return res;
	}

	//接收数据
	res = circle_recv(cli->fd, write_ptr(cli->in), cli->need);
	if (res < 0) 
	{
		close_socket(cli);
		if (!g_is_keep_alive) 
			recycle_client(cli);
		
		return res;
	}

	//更新索引和状态
	seek_write(cli->in, res);

	//已经读空了，返回零。此处包括res=0的情况。
	if (res < cli->need) 
        return 0;
    else
	    return res;
}

//tcp事件读取函数
static void tcp_read(struct epoll_event *ev)
{
	client_t *cli = (client_t *)ev->data.ptr;
	//对端关闭连接或者其他错误，关闭套接字。
	if (ev->events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
	{
		close_socket(cli);
		if (!g_is_keep_alive) 
			recycle_client(cli);
		return;
	}

    //读空为止
	while (read_data(ev) > 0);

    //如果已经达到要求长度
    if (buffer_length(cli->in) >= cli->need)
    {
        //加入就绪链表
        if (cli->is_ready == NO)
        {
	        list *ready_list = (cli->parent == g_user_tcp_fd) ? 
                                g_user_ready : g_manage_ready;

            if (OP_LIST_SUCCESS == list_push_back(ready_list, cli))
            {
                cli->is_ready = YES;
            }
        }
    }
}

//接收tcp连接
static void tcp_accept(int fd)
{
	//变量定义
	struct sockaddr_in cli_addr;
	client_t *client = NULL;
	int len = sizeof(struct sockaddr_in);
	int sock_fd, res;
	link_hander hander;

	//循环接收，收完为止。
	for (;;)
	{
		sock_fd = accept(fd, (struct sockaddr *)&cli_addr, &len);
		if (sock_fd < 0) break;
		
		client = extract_client();
		if (!client) continue;

		client->fd = sock_fd;
		client->ip = cli_addr.sin_addr.s_addr;
		client->parent = fd;
	
		//注册epoll事件
		struct epoll_event ev;
		zero(&ev);
		ev.events = EPOLLIN | EPOLLOUT | EPOLLET| EPOLLRDHUP ; 
		ev.data.ptr = client;
		res = epoll_ctl(g_epoll_fd, EPOLL_CTL_ADD, sock_fd, &ev);
		if (res == 0)
		{
			//添加到心跳检测
			if (fd == g_manage_tcp_fd || g_is_keep_alive == YES)
				add_alive(client->id);
            
            //赋值处理
            if (fd == g_user_tcp_fd)
            {
                hander = g_user_link;
                client->status.need = g_user_first_length;
            }
            else
            {
                hander = g_manage_link;
                client->status.need = g_manage_first_length;
            }

			//通知应用层
            if (hander)
			    hander(client->id, client->ip);
		}			
	}//end for
}

//epollet运行函数
void epollet_run(struct schedule *sche, void *arg)
{
	int i, count;
	char empty[16];
	send(0, empty, sizeof(empty), MSG_DONTWAIT);

	for (;;)
	{
        //最后一个值：0 立即返回，-1 一直阻塞直到有消息前来
		count = epoll_wait(g_epoll_fd, g_events, MAX_EVENT_COUNT, 0);
		
		//分发处理
		for (i = 0; i < count; ++i)
		{
			if (array_exist(g_tcp_listener, g_events[i].data.fd))
			{
                //如果遇到fd为0的极端情况，会accept失败，不用担心。
				tcp_accept(g_events[i].data.fd);
			}
			else if (g_events[i].data.fd == g_udp_fd)
			{
				g_udp_reader(g_events[i].data.fd);
			}
			else if (g_events[i].data.fd > g_epoll_fd)
			{
				//读事件
			 	if (g_events[i].events & EPOLLIN)
				{
					tcp_read(g_events + i);
				}
				//写事件
				else if (g_events[i].events & EPOLLOUT)
				{
					client_t *cli = g_events[i].data.ptr;
					if (cli->out->len > 0)
						circle_send(g_events[i].data.fd, read_ptr(cli->out), 
									cli->out->len);
				}
			}//end if
		}//end for
		
		/* 事关整个线程的休眠 */
		usleep(2);	
		//切换协程
		coroutine_yield(sche);
	}//end for
}

//关闭套接字
void close_socket(client_t *cli)
{
	//检查参数
	assert(cli);
	if (!cli) return;

	//从epoll中删除
	struct epoll_event event = { 0 };
	epoll_ctl(g_epoll_fd, EPOLL_CTL_DEL, cli->fd, &event);

	//通知上层
	if (cli->parent == g_user_tcp_fd)
	{	
		if (g_user_shut)
			g_user_shut(cli->id); 
	}
	else if (cli->parent == g_manage_tcp_fd)
	{
		if (g_manage_shut)
			g_manage_shut(cli->id); 
	}

	//关闭套接字并将客户端结构回收
	close(cli->fd);
	cli->fd = INVALID_SOCKET;
}
