#include <sys/time.h>
#include <signal.h>
#include "keep_alive.h"
#include "../epollet/epollet.h"
#include "../c-stl/list.h"

#define			ALIVE_INTERVAL			3				//检查的间隔时间

list				*g_client_alive = NULL;				//活跃的客户端链表

//检查连接的活跃时间
void check_alive(int num)
{
#ifdef TEST
	puts("check_alive...");
#endif

	list_item *item = NULL;
	client_t *cli = NULL;
	uint64_t now = time(0);
	
	//遍历链表
	list_foreach(g_client_alive, item)
	{
		//参数检查与转换
		if (!item) continue;
		cli = (client_t *)item;
#ifdef TEST
		printf("进入链表检查，client_id: %d\n", cli->id);
#endif

		//socket已经被断开
		if (cli->fd == INVALID_SOCKET)
		{
			list_remove(g_client_alive, item);
			recycle_client(cli);
#ifdef TEST
		printf("cli->fd == INVALID_SOCKET断开，client_id: %d\n", cli->id);
#endif
			continue;
		} 

		//socket没有被断开，但活跃时间超时
		//允许的未活跃时间为30~60秒
		if (now - cli->alive_time > ALIVE_INTERVAL)			
		{
			close_socket(cli);
			list_remove(g_client_alive, item);
			recycle_client(cli);
#ifdef TEST
		printf("超时被断开，client_id: %d \n", cli->id);
#endif
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
	signal(SIGALRM, check_alive);

	struct itimerval tick;
	memset(&tick, 0, sizeof(tick));
	tick.it_value.tv_sec = ALIVE_INTERVAL;
	tick.it_interval.tv_sec = ALIVE_INTERVAL;

	int res = setitimer(ITIMER_REAL, &tick, NULL);
	if (res) return FAILURE;

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

