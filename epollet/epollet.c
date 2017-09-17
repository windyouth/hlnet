#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include "epollet.h"
#include "buffer.h"
#include "../common/algorithm.h"
#include "../uthread/uthread.h"


#define				MAX_EVENT_COUNT			1024			//一次能接收的最多事件个数
#define				MAX_CMDDATA_LEN			65000			//数据体的最大长度

#define				GLOBAL_BUF_ORIGIN_SIZE	4096 * 5		//全局缓冲区初始大小

struct epoll_event  *g_events = NULL;						//事件数组指针 
int 				g_epoll_fd = INVALID_SOCKET;			//epoll元套接字


//创建一个tcp套接字并监听端口
int create_tcp_socket(uint16_t port)
{
	int sock_fd = INVALID_SOCKET;

	//创建socket
	sock_fd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0);
	if (sock_fd == INVALID_SOCKET)
	{
		return FAILURE;
	}
	
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
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;						//ipv4
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int res = bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr));
	if (res == FAILURE)
	{
		closesocket(sock_fd);
		sock_fd = INVALID_SOCKET;
		return INVALID_SOCKET;
	}

	//监听端口
	if (listen(sock_fd, 32) == FAILURE)
	{
		closesocket(sock_fd);
		sock_fd = INVALID_SOCKET;
		return INVALID_SOCKET;
	}

	return sock_fd;
}

//创建udp套接字并绑定端口
int create_udp_socket(uint16_t port)
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
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int res = bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr));
	if (res == FAILURE)
	{
		closesocket(sock_fd);
		sock_fd = INVALID_SOCKET;
		return INVALID_SOCKET;
	}

	return sock_fd;
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
				//uthread_sleep(g_schedule, 1000);
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
	g_epoll_fd = epoll_create1(EPOLL_CLOEXEC);
	if (g_epoll_fd <= 0) return FAILURE;

	g_events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * MAX_EVENT_COUNT);
	if (!g_events) return MEM_ERROR;
}

//epollet添加函数
int epollet_add(int fd, void *data_ptr, int flag)
{
	struct epoll_event ev;
	zero(&ev);

	ev.events = flag; //EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET;
	ev.data.fd = fd;
	ev.data.ptr = data_ptr;

	return epoll_ctl(g_epoll_fd, EPOLL_CTL_ADD, fd, &ev);
}

//-----------------------------------------------------------
// description: 从内核中读数据
// return: >0 正常读数据
//		   =0 内核缓冲区已经读空
//		   <0 网络异常，关闭套接字
//-----------------------------------------------------------
int read_data(struct epoll_event *ev, client_t *cli, buffer *global_buf)
{
	//参数检查
	assert(ev && cli && global_buf);
	if (!ev || !cli || !global_buf) return PARAM_ERROR;

	int len = cli->status.need;

	//调整缓冲区结构
	int res = buffer_rectify(cli->in, len);
	if (res < 0)
	{
		//由于是边缘触发，为防止该套接字变僵尸，直接删除之。
		close_socket(ev);
		return res;
	}

	//写客户端ID
	if (cli->in->len == 0)
		buffer_write_int(cli->in, cli->id);

	//接收数据
	res = circle_recv(ev->data.fd, write_ptr(cli->in), len);
	if (res < 0) 
	{
		close_socket(ev);
		return res;
	}

	//更新索引和状态
	seek_write(cli->in, res);
	cli->status.need = len - res;

	//只读了一部分，提前返回。
	if (res < len) return 0;

	if (cli->status.part == READ_PART_HEAD)
	{
		//检查参数
		cmd_head_t *head = (cmd_head_t *)(cli->in->buf);
		if (head->data_size > MAX_CMDDATA_LEN) 
		{
			close_socket(ev);
			return FAILURE;
		}

		//如果客户端只发送一个数据头过来，此处就不做任何处理。
		if (head->data_size > 0) 
		{
			cli->status.part = READ_PART_DATA;
			cli->status.need = head->data_size;
		}
		else
		{
			//写入全局缓冲区
			buffer_write(global_buf, read_ptr(cli->in), cli->in->len);
			//状态重置
			buffer_reset(cli->in);
		}
	}
	else
	{
		//写入全局缓冲区
		buffer_write(global_buf, read_ptr(cli->in), cli->in->len);
		//状态重置
		buffer_reset(cli->in);
		cli->status.part = READ_PART_HEAD;
		cli->status.need = sizeof(packet_head_t);
	}

	return res;
}

//tcp事件读取函数
static void tcp_read(struct epoll_event *ev)
{
	//对端关闭连接或者其他错误，关闭套接字。
	if (ev->events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
	{
		close_socket(ev);
	}

	client_t *client = (client_t *)ev->data.ptr;
	buffer *cur_buf = (client->parent == g_client_tcp_fd) ? g_client_buf : g_manage_buf;

	while (read_data(ev, client->in, cur_buf) > 0);
}

//接收tcp连接
static void tcp_accept(int fd)
{
	//变量定义
	struct sockaddr_in cli_addr;
	client_t *client = NULL;
	int len = sizeof(struct sockaddr_in);
	int sock_fd;
	struct epoll_event ev;
	
	//循环接收，收完为止。
	do
	{
		int sock_fd = accept(fd, (struct sockaddr *)&cli_addr, &len);
		if (sock_fd < 0) break;
		
		client = extract_client();
		if (!client) continue;

		client->fd = sock_fd;
		client->ip = cli_addr.sin_addr.s_addr;
		client->parent = fd;

		epollet_add(sock_fd, client, EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET);
	}while (1);
}

//epollet运行函数
int epollet_run(void *arg)
{
	int i, count;

	for (;;)
	{
		//超时时间：0 立即返回，-1 无限期阻塞。
		count = epoll_wait(g_epoll_fd, g_events, MAX_EVENT_COUNT, 0);
		
		//分发处理
		for (i = 0; i < count; ++i)
		{
			//读事件
			if (g_events[i].events & EPOLLIN)
			{
				if (g_events[i].data.fd == g_client_tcp_fd || 
					g_events[i].data.fd == g_manage_tcp_fd)
				{
					tcp_accept(g_events[i].data.fd);
				}
				else if (g_events[i].data.fd == g_udp_fd)
				{
					g_udp_reader(g_events[i].data.fd);
				}
				else
				{
					tcp_read(g_events + i);
				}
			}
			//写事件
			else if (g_events[i].events & EPOLLOUT)
			{
				client_t *cli = g_events[i].data.ptr;
				if (cli->out->len > 0)
					circle_send(g_events[i].data.fd, read_ptr(cli->out), cli->out->len);
			}
		}

		//切换协程
		uthread_yield((schedule_t *)arg);
	}//end for
}

//关闭套接字
void close_socket(struct epoll_event *ev)
{
	//从epoll中删除
	struct epoll_event event = { 0 };
	epoll_ctl(g_epoll_fd, EPOLL_CTL_DEL, ev->data.fd, &event);

	//通知上层
	client_t *cli = ev->data.ptr;
	if (cli->parent == g_client_tcp_fd)
	{	
		if (g_client_shut)
			g_client_shut(ev->data.fd); 
	}
	else if (cli->parent == g_client_tcp_fd)
	{
		if (g_manage_shut)
			g_manage_shut(ev->data.fd); 
	}

	//关闭套接字并将客户端结构回收
	close(ev->data.fd);
	recycle_client((client_t *)ev->data.ptr);
}
