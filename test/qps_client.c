#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include "define.h"
#include "../hlnet/include/algorithm.h"
#include "../hlnet/include/common.h"

#define         CONN_COUNT      25000
#define         MSG_COUNT       2
#define         USLEEP_TIME     2

int g_sent_count = 0;
int g_recv_count = 0;

struct timespec g_time_start = { 0, 0 };
struct timespec g_time_end = { 0, 0 };

int64_t g_usec_start = 0;
int64_t g_usec_end = 0;

void send_qps_msg(int fd)
{
	int len = sizeof(cmd_head_t) + sizeof(reg_info);
	char buf[len];

	cmd_head_t *head = (cmd_head_t *)buf;
	head->data_size = sizeof(reg_info);
	head->cmd_code = MSG_QPS;
	head->proto_ver = 2;

	reg_info *data = (reg_info *)(head + 1);
	snprintf(data->account, 32, "fuck008");
	snprintf(data->password, 32, "123456");
	snprintf(data->secret_key, 32, "secret008");
	snprintf(data->corporation, 32, "阿里音乐娱乐公司");
	snprintf(data->phone, 32, "13666221607");

	int res = send(fd, buf, len, 0);
	if (res <= 0)
	{
		printf("发送失败，res: %d\n", res);
		return;
	}
    else
    {
        ++g_sent_count;
        //printf("发送第%d个数据包成功，长度：%d \n", g_sent_count, res);
    }
}

int client_tcp_socket(char *ip, ushort port)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == fd)
    { 
        perror("创建socket失败。");
        return INVALID_SOCKET;
    }

	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_aton(ip, &(addr.sin_addr));

	if (FAILURE == connect(fd, (struct sockaddr *)&addr, sizeof(addr)))
	{
        perror("connect失败。");
        printf("连接失败，fd: %d \n", fd);
		close(fd);
		return INVALID_SOCKET;
	}

	return fd;
}

int         *g_fds;

void create_fds()
{
    int i; 
    g_fds = (int *)malloc(sizeof(int) * CONN_COUNT);
    if (!g_fds)
    {
        puts("fds molloc failure");
        exit(1);
    }

	for (i = 0; i < CONN_COUNT; ++i)
	{
		g_fds[i] = client_tcp_socket("0.0.0.0", PORT_CLIENT);
		if (g_fds[i] == INVALID_SOCKET)
		{
			puts("client_tcp_socket failure");
			exit(1);
		}

        usleep(USLEEP_TIME);
	}
}

sem_t       sem_ok;

void send_msg()
{
    clock_gettime(CLOCK_MONOTONIC, &g_time_start);
    printf("开始时间：%d秒，%d纳秒 \n", g_time_start.tv_sec, g_time_start.tv_nsec);
    g_usec_start = g_time_start.tv_sec * 1000000 + g_time_start.tv_nsec / 1000;
    printf("开始时间：%lld微秒 \n", g_usec_start);

    for (int i = 0; i < MSG_COUNT; ++i)
	{
		for (int j = 0; j < CONN_COUNT; j++)
		{
			send_qps_msg(g_fds[j]);

            static int flag;
            if (!flag)
            {
                flag++;
                //发出通知
                sem_post(&sem_ok);
            }

            if (j % 10 == 0)
                usleep(USLEEP_TIME);
		}
	}

    //发出通知
    sem_post(&sem_ok);
}

void *recv_msg(void *arg)
{
    int res = 0;
    int i;
    char buf[sizeof(db_reg_info)];

    //等待通知
    sem_wait(&sem_ok);

    for (i = 0; i < MSG_COUNT; ++i)
	{
		for (int j = 0; j < CONN_COUNT; j++)
		{
	        res = recv(g_fds[j], buf, sizeof(db_reg_info), 0);
	        if (res <= 0)
	        {
                puts("接收消息失败");
	        }
            else
            {
                ++g_recv_count;
                //printf("收到第%d个消息包，长度为：%d \n", g_recv_count, res);
            }
		}
        usleep(USLEEP_TIME);
	}

    clock_gettime(CLOCK_MONOTONIC, &g_time_end);
    printf("结束时间：%d秒，%d纳秒 \n", g_time_end.tv_sec, g_time_end.tv_nsec);
    g_usec_end = g_time_end.tv_sec * 1000000 + g_time_end.tv_nsec / 1000;
    printf("结束时间：%lld微秒，消息包数量: %d \n", g_usec_end, g_recv_count);

    int64_t one_time = (g_usec_end - g_usec_start) / g_recv_count;
    int qps = 1000000 / one_time;
    printf("qps的值为：%d \n", qps);

    //等待通知
    sem_wait(&sem_ok);

    //关闭所有连接
    for (i = 0; i < CONN_COUNT; i++)
	{
		close(g_fds[i]);
	}
}

void main()
{
    printf("连接的客户端数：%d \n", CONN_COUNT);

    pthread_t pt_recv = 0;
    sem_init(&sem_ok, 0, 0);

    pthread_create(&pt_recv, NULL, (void *)recv_msg, NULL);

    create_fds();
    send_msg();

    pthread_join(pt_recv, NULL);

    sem_destroy(&sem_ok);
}
