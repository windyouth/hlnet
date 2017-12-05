#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "define.h"
#include "../bin/include/algorithm.h"

#define				MAX_EVENT_COUNT			1024			//一次能接收的最多事件个数

int 				g_epoll_fd = -1;
struct epoll_event  *g_events = NULL;						//事件数组指针 

int 				listen_fd = -1;

//设备套接字为非阻塞
int set_nonblock(int fd)
{
	int opt = fcntl(fd, F_GETFL);
	if (opt < 0) return FAILURE;

	opt = opt|O_NONBLOCK;
	if (fcntl(fd, F_SETFL, opt) < 0)
		return FAILURE;
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

void main()
{
	g_epoll_fd = epoll_create(256);
	if (-1 == g_epoll_fd)
	{
		puts("epoll_create() failed");
		exit(1);
	}

	g_events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * MAX_EVENT_COUNT);
	if (!g_events) return;

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd == -1)
	{
		puts("创建listen_fd失败");
		exit(1);
	}

	set_nonblock(listen_fd);
	
	//注册epoll事件
	/*epollet_add(listen_fd, NULL, EPOLLIN | EPOLLET);
	struct epoll_event ev;*/
	
	struct epoll_event ev;
	ev.data.ptr = NULL;
	ev.data.fd = listen_fd;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(g_epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev);

	//绑定端口
	struct sockaddr_in addr, cli_addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	//char *ip = "0.0.0.0";
	//inet_aton(ip, &(addr.sin_addr));
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(PORT_CLIENT);
	bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr));
	//监听
	listen(listen_fd, 32);

	//接收数据
	int fd_count, i, n, conn_fd, sock_fd;
	socklen_t cli_len;
	char buf[5120] = { 0 };
	char empty[16];
	//send(0, empty, sizeof(empty), MSG_DONTWAIT);
	for (;;)
	{
		fd_count = epoll_wait(g_epoll_fd, g_events, MAX_EVENT_COUNT, 500);
		
		for (i = 0; i < fd_count; ++i)
		{
			printf("g_epoll_fd: %d, listen_fd: %d, recv_fd：%d \n", 
					g_epoll_fd, listen_fd, g_events[i].data.fd);
			//接收连接
			if (g_events[i].data.fd == listen_fd)
			{
				conn_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &cli_len);
				if (conn_fd < 0)
				{
					puts("accept error");
					exit(1);
				}

				char *str = inet_ntoa(cli_addr.sin_addr);
				printf("accept a connection from : %s \n", str);

				ev.data.fd = conn_fd;
				ev.events = EPOLLIN|EPOLLET;
				epoll_ctl(g_epoll_fd, EPOLL_CTL_ADD, conn_fd, &ev);
			}
			//读取数据
			else if (g_events[i].events & EPOLLIN)
			{
				if ((sock_fd = g_events[i].data.fd) < 0)
					continue;

				if ((n = read(sock_fd, buf, 5120)) < 0)
				{
					if (errno == ECONNRESET)
					{
						close(sock_fd);
						g_events[i].data.fd = -1;
					}
					else
					{
						puts("read(sock_fd, buf, 5120) error");
					}
				}
				else if (n == 0)
				{
					close(sock_fd);
					g_events[i].data.fd = -1;
				}
				
				buf[n] = 0;
				login_info *login = (login_info *)buf;
				printf("recv data: \n");
				printf("account: %s \n", login->account);
				printf("password: %s \n", login->password);
			}//end else if
		}//end for
		usleep(50);
	}//end for

}
