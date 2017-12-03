#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "define.h"

void main()
{
	int epoll_fd = epoll_create(256);
	if (-1 == epoll_fd)
	{
		puts("epoll_create() failed");
		exit(1);
	}

	int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd == -1)
	{
		puts("创建listen_fd失败");
		exit(1);
	}

	int opt = fcntl(listen_fd, F_GETFL);
	if (opt < 0)
	{
		puts("fcntl(listen_fd, F_GETFL) failed");
		exit(1);
	}
	opt = opt | O_NONBLOCK;
	if (fcntl(listen_fd, F_SETFL, opt) < 0)
	{
		puts("fcntl(listen_fd, F_SETFL, opt) failed");
		exit(1);
	}

	//注册epoll事件
	struct epoll_event ev, events[32];
	ev.data.fd = listen_fd;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev);

	//绑定端口
	struct sockaddr_in addr, cli_addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	char *ip = "0.0.0.0";
	inet_aton(ip, &(addr.sin_addr));
	addr.sin_port = htons(PORT_CLIENT);
	bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr));
	//监听
	listen(listen_fd, 32);

	//接收数据
	int fd_count, i, n, conn_fd, sock_fd;
	socklen_t cli_len;
	char buf[5120];
	char empty[16];
	send(0, empty, sizeof(empty), MSG_DONTWAIT);
	for (;;)
	{
		fd_count = epoll_wait(epoll_fd, events, 20, 500);
		
		for (i = 0; i < fd_count; ++i)
		{
			//接收连接
			if (events[i].data.fd == listen_fd)
			{
				conn_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &cli_len);
				if (conn_fd < 0)
				{
					puts("accept error");
					exit(1);
				}

				char *str = inet_ntoa(cli_addr.sin_addr);
				printf("accept a connection from : %s", str);

				ev.data.fd = conn_fd;
				ev.events = EPOLLIN|EPOLLET;
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_fd, &ev);
			}
			//读取数据
			else if (events[i].events & EPOLLIN)
			{
				if ((sock_fd = events[i].data.fd) < 0)
					continue;

				if ((n = read(sock_fd, buf, 5120)) < 0)
				{
					if (errno == ECONNRESET)
					{
						close(sock_fd);
						events[i].data.fd = -1;
					}
					else
					{
						puts("read(sock_fd, buf, 5120) error");
					}
				}
				else if (n == 0)
				{
					close(sock_fd);
					events[i].data.fd = -1;
				}
				
				buf[n] = 0;
				printf("read data: %s", buf);
				//last_fd = sock_fd;
			}//end else if
		}//end for
		usleep(50);
	}//end for



}
