#include <time.h>
#include "keep_alive.h"
#include "../epollet/epollet.h"
#include "../c-stl/list.h"

list				*g_client_alive = NULL;				//活跃的客户端链表

//启动心跳检测
int keep_alive()
{
	//初始化活跃链表
	g_client_alive = (list *)malloc(sizeof(list));
	if (!g_client_alive) return MEM_ERROR;

	list_init(g_client_alive);
	g_is_keep_alive = YES;

	//开启定时器
}

//添加到心跳检测池
void add_alive(int client_id)
{
	client_t *cli = get_client(client_id);
	if (!cli) return;

	list_push_back(g_client_alive, cli);
}

//更新活跃时间
void alive(int client_id)
{
	client_t *cli = get_client(client_id);
	if (!cli) return;

	cli->alive_time = time(0);
}

//设为安全连接
int safe(int client_id)
{
	client_t *cli = get_client(client_id);
	if (!cli) return FAILURE;

	cli->is_safe = YES;

	return SUCCESS;
}

//查询是否是安全连接
int is_safe(int client_id)
{
	client_t *cli = get_client(client_id);
	if (!cli) return FAILURE;

	return cli->is_safe;
}

