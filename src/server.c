#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include "server.h"
#include "keep_alive.h"
#include "log.h"
#include "../common/internal.h"
#include "../c-stl/queue.h"
#include "../c-stl/map.h"
#include "../epollet/epollet.h"
#include "../coroutine/coroutine.h"


#define				UDP_BUFFER_SIZE				(MAX_UDP_LENGTH + 1)	//UDP缓冲区大小

/*
//线程状态
typedef enum _thread_state
{
	threadstate_running = 1,
	threadstate_stopping = 2,
	threadstate_stopped = 3
}thread_state_e;
*/

map						*g_net_client_msg = NULL;			//网络消息映射(TCP用户端口)
map						*g_net_manage_msg = NULL;			//网络消息映射(TCP管理端口)
map						*g_net_udp_msg = NULL;				//网络消息映射(UDP端口)

char					*g_udp_buffer = NULL;				//UDP缓冲区

static struct schedule	*g_schedule = NULL;					//协程调度器

//处理内核消息
static void kernel_message(int client_id, cmd_head_t *head, char *data)
{
	switch (head->cmd_code)
	{
		case CMD_KERNEL_HEARTBEAT:
		{
			/* 暂时无事可做 */
		}
		break;
		default:
		break;
	}
}

//网络消息分发(客户端)
void issue_client_msg(void *arg)
{
	packet_head_t *head = NULL;
	char *data = NULL;
	msg_func_item *func_item;
	tcpmsg_hander hander;
	char cmd[8];

	for (;;)
	{
#ifdef TEST
			//puts("执行issue_client_msg()");
#endif
		while (g_client_buf->len > 0)
		{
#ifdef TEST
			//puts("issue_client_msg()进入while循环");
#endif
			//从全局缓冲区读数据
			buffer_read(g_client_buf, head, sizeof(*head));
			buffer_read(g_client_buf, data, head->head.data_size);

			//更新活跃时间
			alive(head->client_id);

			//如果是内核消息
			if (head->head.cmd_code <= CMD_KERNEL_END)
			{
				kernel_message(head->client_id, &(head->head), data);
				continue;
			}

			//取出消息处理函数派发消息
			snprintf(cmd, sizeof(cmd), "%u", head->head.cmd_code);
			func_item = (msg_func_item *)map_get(g_net_client_msg, cmd, strlen(cmd));
			if (func_item && func_item->msg_func)
			{
				hander = (tcpmsg_hander)func_item->msg_func;
				hander(head->client_id, &(head->head), data);
			}
		} //end while

#ifdef TEST
		//puts("issue_client_msg()让出协程");
#endif
		uthread_yield((schedule_t *)arg);
	}//end for
}

//网络消息分发(管理端)
void issue_manage_msg(void *arg)
{
	packet_head_t *head = NULL;
	char *data = NULL;
	msg_func_item *func_item;
	tcpmsg_hander hander;
	char cmd[8];

	for (;;)
	{
		while (g_manage_buf->len > 0)
		{
			//从全局缓冲区读数据
			buffer_read(g_manage_buf, head, sizeof(*head));
			buffer_read(g_manage_buf, data, head->head.data_size);

			//更新活跃时间
			alive(head->client_id);

			//如果是内核消息
			if (head->head.cmd_code <= CMD_KERNEL_END)
			{
				kernel_message(head->client_id, &(head->head), data);
				continue;
			}

			//取出消息处理函数派发消息
			snprintf(cmd, sizeof(cmd), "%u", head->head.cmd_code);
			func_item = (msg_func_item *)map_get(g_net_manage_msg, cmd, strlen(cmd));
			if (func_item && func_item->msg_func)
			{
				hander = (tcpmsg_hander)func_item->msg_func;
				hander(head->client_id, &(head->head), data);
			}
		}

		uthread_yield((schedule_t *)arg);
	}
}

//udp事件读取函数
static void udp_read(int fd)
{
	cmd_head_t *head = NULL;
	char *data = NULL;
	udpmsg_hander hander;
	char cmd[8];

	struct sockaddr_in addr;
	int len = sizeof(addr);

	//接收数据
	int size = recvfrom(fd, g_udp_buffer, MAX_UDP_LENGTH, 0, 
						(struct sockaddr *)&addr, (socklen_t *)&len);
	if (size < 0 || addr.sin_family != AF_INET)
		return;

	//检验数据
	g_udp_buffer[size] = 0;
	head = (packet_head_t *)g_udp_buffer;
	if (head->data_size + sizeof(*head) != size)
		return;

	hander(addr.sin_addr.s_addr, addr.sin_port, (cmd_head_t *)g_udp_buffer, g_udp_buffer + sizeof(*head));
}

//创建tcp客户端相关
int create_tcp_client(uint16_t port)
{
	if (g_client_tcp_fd != INVALID_SOCKET) return FAILURE;

	int fd = create_tcp_socket(port);
	if (fd < 0) return FAILURE;

	//初始化网络消息映射器
	g_net_client_msg = (map *)malloc(sizeof(map));
	if (!g_net_client_msg) return MEM_ERROR;
	if (map_init(g_net_client_msg) != OP_MAP_SUCCESS) return MEM_ERROR;

	//初始化缓冲区
	g_client_buf = (buffer *)malloc(sizeof(buffer));
	if (!g_client_buf) return MEM_ERROR;
		
#ifndef TEST
	//创建协程
	int issue_id = uthread_create(g_schedule, issue_client_msg);
	if (issue_id < 0) return FAILURE;
#endif

	g_client_tcp_fd = fd;
	return epollet_add(g_client_tcp_fd, NULL, EPOLLIN | EPOLLET);
}

//创建tcp管理端相关
int create_tcp_manage(uint16_t port)
{
	if (g_manage_tcp_fd != INVALID_SOCKET) return FAILURE;

	int fd = create_tcp_socket(port);
	if (fd < 0) return FAILURE;

	//初始化网络消息映射器
	g_net_manage_msg = (map *)malloc(sizeof(map));
	if (!g_net_manage_msg) return MEM_ERROR;
	if (map_init(g_net_manage_msg) != OP_MAP_SUCCESS) return MEM_ERROR;

	//初始化缓冲区
	g_manage_buf = (buffer *)malloc(sizeof(buffer));
	if (!g_manage_buf) return MEM_ERROR;

	//创建协程
	int issue_id = uthread_create(g_schedule, issue_manage_msg);
	if (issue_id < 0) return FAILURE;

	g_manage_tcp_fd = fd;
	if (0 != epollet_add(g_manage_tcp_fd, NULL, EPOLLIN | EPOLLET))
		return FAILURE;

	return keep_alive();
}

//创建udp相关
int create_udp(uint16_t port)
{
	if (g_udp_fd != INVALID_SOCKET) return FAILURE;

	int fd = create_udp_socket(port);
	if (fd < 0) return FAILURE;

	//初始化网络消息映射器
	g_net_udp_msg = (map *)malloc(sizeof(map));
	if (!g_net_udp_msg) return MEM_ERROR;
	if (map_init(g_net_udp_msg) != OP_MAP_SUCCESS) return MEM_ERROR;

	//初始化缓冲区
	g_udp_buffer = (char *)malloc(UDP_BUFFER_SIZE);
	if (!g_udp_buffer) return MEM_ERROR;

	//设置回调函数
	g_udp_reader = udp_read;

	g_udp_fd = fd;
	return epollet_add(g_udp_fd, NULL, EPOLLIN);
}

//创建服务器
int serv_create()
{
	//构造调度器
	g_schedule  = schedule_create();
	if (!g_schedule) return FAILURE;

	return epollet_create();
}

//添加服务器参数
int serv_ctl(sock_type_e sock_type, short port)
{
	//tcp客户端
	if (sock_type == socktype_client)		
	{
		return create_tcp_client(port);
	}
	//tcp管理端	
	else if (sock_type == socktype_manage)	
	{
		return create_tcp_manage(port);
	}
	//udp端口
	else if (sock_type == socktype_udp)				
	{
		return create_udp(port);
	}
	else
	{
		return PARAM_ERROR;
	}
}

//运行服务器
int serv_run()
{
	//创建协程
	int id = uthread_create(g_schedule, epollet_run);
	if (id < 0) return FAILURE;
	
	id = uthread_create(g_schedule, write_log);
	if (id < 0) return FAILURE;

	//调度器运行
	uthread_run(g_schedule);
}

//注册连接消息函数
int reg_link_event(sock_type_e type, link_hander func)
{
	if (!func) return PARAM_ERROR;

	if (type == socktype_client)
		g_client_link = func;
	else if (type == socktype_manage)
		g_manage_link = func;
	else
		return PARAM_ERROR;

	return SUCCESS;
}

//注册中断消息函数
int reg_shut_event(sock_type_e type, shut_hander func)
{
	if (!func) return PARAM_ERROR;

	if (type == socktype_client)
		g_client_shut = func;
	else if (type == socktype_manage)
		g_manage_shut = func;
	else
		return PARAM_ERROR;

	return SUCCESS;
}

//注册网络消息
int reg_net_msg(sock_type_e sock_type, uint16_t msg, tcpmsg_hander func)
{
	char *key = (char *)malloc(8);
	zero_array(key, 8);
	sprintf(key, "%u", msg);

	map *dst_map = NULL;

	switch (sock_type)
	{
		case socktype_client:
			{
				dst_map = g_net_client_msg;
			}
			break;
		case socktype_manage:
			{
				dst_map = g_net_manage_msg;
			}
			break;
		default:
			return PARAM_ERROR;
	}

	msg_func_item *item = (msg_func_item *)malloc(sizeof(msg_func_item));
	if (!item) return MEM_ERROR;

	item->msg_func = func;
	if (map_put(dst_map, key, strlen(key), item) != OP_MAP_SUCCESS)
		return FAILURE;

	return SUCCESS;
}

//注册UDP消息
int reg_udp_msg(uint16_t msg, udpmsg_hander func)
{
	char *key = (char *)malloc(8);
	zero_array(key, 8);
	sprintf(key, "%u", msg);

	
	msg_func_item *item = (msg_func_item *)malloc(sizeof(msg_func_item));
	if (!item) return MEM_ERROR;

	item->msg_func = func;

	return (map_put(g_net_udp_msg, key, strlen(key), item) == OP_MAP_SUCCESS) ? SUCCESS : FAILURE;
}

//发送数据(tcp)
int tcp_send(uint32_t client_id, uint16_t cmd, char *data, uint32_t len)
{
	//取得对应的客户端
	client_t *cli = get_client(client_id);
	if (!cli) return PARAM_ERROR;

	//申请缓冲区
	int size = sizeof(cmd_head_t);
	if (data) size += len;
	
	int res = buffer_rectify(cli->out, size);
	if (res != SUCCESS) return res;

	//写数据
	cmd_head_t *head = (cmd_head_t *)write_ptr(cli->out);	
	zero(head);
	head->cmd_code = cmd;
	seek_write(cli->out, sizeof(*head));
	if (data)
	{
	   	head->data_size = len;
		buffer_write(cli->out, data, len);
	}

	//发送，如果没有发成功，由后续的epoll写事件发送。
	res = circle_send(cli->fd, read_ptr(cli->out), cli->out->len);
	if (res > 0) buffer_reset(cli->out);

	return res;
}

//发送数据(udp) ip, port必须是大端(网络序)
int udp_send(uint32_t ip, uint16_t port, uint16_t cmd, char *data, uint32_t len)
{
	//参数检查
	if (len > MAX_UDP_LENGTH - sizeof(cmd_head_t))
		return PARAM_ERROR;

	//填写地址信息
	struct sockaddr_in addr;
	zero(&addr);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	addr.sin_port = port;

	//申请缓冲区
	const int size = data ? sizeof(cmd_head_t) : sizeof(cmd_head_t) + len;
	char buf[size];
	zero_array(buf, size);

	//写数据
	cmd_head_t *head = (cmd_head_t *)buf;
	head->cmd_code = cmd;
	if (data)
	{
		head->data_size = len;
		memcpy(buf + sizeof(cmd_head_t), data, len);
	}

	//发送
	return sendto(g_udp_fd, data, len, 0, (struct sockaddr *)&addr, sizeof(addr));
}
