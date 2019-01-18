#include <sys/time.h>
#include "alive.h"
#include "timer.h"
#include "../epollet/epollet.h"
#include "../c-stl/list.h"

#define				ALIVE_INTERVAL			30			//检查的间隔时间

static list			*g_client_alive = NULL;				//活跃的客户端链表

//检查连接的活跃时间
int check_alive(struct _timer *timer)
{
	uint64_t now = time(0);
    list_item *item = 0;

    list_foreach(g_client_alive, item)
    {
        client_t *cli = (client_t *)item;

        //socket已经被断开
        if (cli->fd == INVALID_SOCKET)
        {
            list_erase(g_client_alive, item);
            recycle_client(cli);

            continue;
        } 

        //socket没有被断开，但活跃时间超时
        //允许的未活跃时间为30~60秒
        if (now - cli->alive_time > ALIVE_INTERVAL)			
        {
            close_socket(cli);
            list_erase(g_client_alive, item);
            recycle_client(cli);

            continue;
        }
    }
}

//启动心跳检测
int keep_alive()
{
	//初始化活跃链表
	g_client_alive = (list *)malloc(sizeof(list));
	if (!g_client_alive) return MEM_ERROR;

	list_init(g_client_alive);
	g_is_keep_alive = YES;

	//开启定时器
	timer_manager();
	add_timer(ALIVE_INTERVAL, -1, check_alive, NULL);
	
	return SUCCESS;
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

