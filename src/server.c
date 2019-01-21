#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include "server.h"
#include "alive.h"
#include "log.h"
#include "timer.h"
#include "moment.h"
#include "../common/internal.h"
#include "../c-stl/queue.h"
#include "../c-stl/map.h"
#include "../epollet/epollet.h"
#include "../coroutine/coroutine.h"


#define				UDP_BUFFER_SIZE				(MAX_UDP_LENGTH + 1)	//UDP缓冲区大小


static map				*g_net_user_msg = NULL;			//网络消息映射(TCP用户端口)
static map				*g_net_manage_msg = NULL;			//网络消息映射(TCP管理端口)
static map				*g_net_udp_msg = NULL;				//网络消息映射(UDP端口)

static char				*g_udp_buffer = NULL;				//UDP缓冲区

static struct schedule	*g_schedule = NULL;					//协程调度器

//tcp通信的类型
typedef enum _tcp_type
{
    tcp_type_user = 1,
    tcp_type_manage = 2
}tcp_type;

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

//处理网络消息
static void deal_msg(list *ready_list, list_item *item, map *msg_map)
{
    //变量定义
    cmd_head_t *head = NULL;
	char *data = NULL;
	msg_func_item *func_item;
	tcpmsg_hander hander;
	char cmd[8];

    client_t *cli = (client_t *)item;

    //从输入缓冲区读数据，每次只读一条
	buffer_read(cli->in, head, sizeof(*head));
	buffer_read(cli->in, data, head->data_size);

	//更新活跃时间
	alive(cli->id);

	//如果是内核消息
	if (head->cmd_code <= CMD_KERNEL_END)
	{
		kernel_message(cli->id, head, data);
		return;
	}

	//取出消息处理函数派发消息
	snprintf(cmd, sizeof(cmd), "%u", head->cmd_code);
	func_item = (msg_func_item *)map_get(msg_map, cmd, strlen(cmd));
    //执行消息处理函数
	if (func_item && func_item->msg_func)
	{
		hander = (tcpmsg_hander)func_item->msg_func;
		hander(cli->id, head, data);
	}

    //如果消息读完了，移出就绪链表
    if (cli->in->len <= 0)
    {
        if (NULL != list_erase(ready_list, cli))
        {
            cli->is_ready = NO;
        }
    }
}

//网络消息分发(客户端)
void issue_user_msg(struct schedule *sche, void *arg)
{
    long index;
    list_item *item, *temp;

	for (;;)
	{
        list_foreach(g_user_ready, index, item, temp)
        {
            deal_msg(g_user_ready, item, g_net_user_msg);
        }

		coroutine_yield(sche);
	}//end for
}

//网络消息分发(管理端)
void issue_manage_msg(struct schedule *sche, void *arg)
{
    long index;
    list_item *item, *temp;
	for (;;)
	{
        list_foreach(g_manage_ready, index, item, temp)
        {
            deal_msg(g_manage_ready, item, g_net_manage_msg);
        }

		coroutine_yield(sche);
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
	head = (cmd_head_t *)g_udp_buffer;
	if (head->data_size + sizeof(*head) != size)
		return;

	hander(addr.sin_addr.s_addr, addr.sin_port, (cmd_head_t *)g_udp_buffer, g_udp_buffer + sizeof(*head));
}

//创建tcp客户端相关
static int create_tcp_client(uint16_t port)
{
	if (g_user_tcp_fd != INVALID_SOCKET) return FAILURE;

	if (SUCCESS != create_client_fd(port))
		return FAILURE;

	//初始化网络消息映射器
	g_net_user_msg = (map *)malloc(sizeof(map));
	if (!g_net_user_msg) return MEM_ERROR;
	if (map_init(g_net_user_msg) != OP_MAP_SUCCESS) return MEM_ERROR;

   	//初始化就绪链表
	g_user_ready = list_create();
	if (!g_user_ready) return MEM_ERROR;
		
	//创建协程
	if (-1 == coroutine_new(g_schedule, issue_user_msg, NULL))
		return FAILURE;

	return SUCCESS;
}

//创建tcp管理端相关
static int create_tcp_manage(uint16_t port)
{
	if (g_manage_tcp_fd != INVALID_SOCKET) return FAILURE;

	if (SUCCESS != create_manage_fd(port))
		return FAILURE;

	//初始化网络消息映射器
	g_net_manage_msg = (map *)malloc(sizeof(map));
	if (!g_net_manage_msg) return MEM_ERROR;
	if (map_init(g_net_manage_msg) != OP_MAP_SUCCESS) return MEM_ERROR;

	//初始化就绪链表
	g_manage_ready = list_create();
	if (!g_manage_ready) return MEM_ERROR;

	//创建协程
	if (-1 == coroutine_new(g_schedule, issue_manage_msg, NULL))
		return FAILURE;

	return keep_alive();
}

//创建udp相关
static int create_udp(uint16_t port)
{
	if (g_udp_fd != INVALID_SOCKET) return FAILURE;

	if (SUCCESS != create_udp_fd(port))
		return FAILURE;

	//初始化网络消息映射器
	g_net_udp_msg = (map *)malloc(sizeof(map));
	if (!g_net_udp_msg) return MEM_ERROR;
	if (map_init(g_net_udp_msg) != OP_MAP_SUCCESS) return MEM_ERROR;

	//初始化缓冲区
	g_udp_buffer = (char *)malloc(UDP_BUFFER_SIZE);
	if (!g_udp_buffer) return MEM_ERROR;

	//设置回调函数
	g_udp_reader = udp_read;

	return SUCCESS;
}

//创建服务器
int serv_create()
{
    //初始化缓冲区仓库
    if (SUCCESS != buffer_store_init())
        return FAILURE;

	//构造调度器
	g_schedule  = coroutine_open();
	if (!g_schedule) return FAILURE;

	return epollet_create();
}

//添加服务器参数
int serv_ctl(sock_type_e sock_type, short port)
{
	//tcp客户端
	if (sock_type == socktype_user)		
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
	int id = coroutine_new(g_schedule, epollet_run, NULL);
	if (id < 0) return FAILURE;

	//日志组件
	if (use_log)
	{	
		id = coroutine_new(g_schedule, write_log, NULL);
		if (id < 0) return FAILURE;
	}

	//相对定时器组件
	if (use_timer)
	{	
		id = coroutine_new(g_schedule, check_timers, NULL);
		if (id < 0) return FAILURE;
	}
	
	//绝对定时器组件
	if (use_moment)
	{	
		id = coroutine_new(g_schedule, check_moments, NULL);
		if (id < 0) return FAILURE;
	}

	//调度器运行
	coroutine_run(g_schedule);
}

//设置初次接收数据包的长度
int set_first_length(sock_type_e sock_type, uint length)
{
    if (sock_type == socktype_user)
        g_user_first_length = length;
    else
        g_manage_first_length = length;

    return SUCCESS;
}

//设置下次接收数据包的长度
int set_next_length(uint32_t client_id, uint length)
{
	//取得对应的客户端
	client_t *cli = get_client(client_id);
	if (!cli) return PARAM_ERROR;

    cli->status.need = length;

    return SUCCESS;
}

//注册连接消息函数
int reg_link_event(sock_type_e type, link_hander func)
{
	if (!func) return PARAM_ERROR;

	if (type == socktype_user)
		g_user_link = func;
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

	if (type == socktype_user)
		g_user_shut = func;
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
	bzero(key, 8);
	sprintf(key, "%u", msg);

	map *dst_map = NULL;

	switch (sock_type)
	{
		case socktype_user:
			{
				dst_map = g_net_user_msg;
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
	bzero(key, 8);
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
	bzero(buf, size);

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
