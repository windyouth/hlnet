#include <pthread.h>
#include "database.h"
#include "../c-stl/queue.h"
#include "../c-stl/map.h"
#include "../common/buffer.h"
#include "../coroutine/coroutine.h"


static pthread_t			g_thread = -1;
static struct schedule		*g_schedule = NULL;						//协程调度器

static queue				*g_dbmsg_queue = NULL;
static map					*g_dbmsg_map = NULL;					//数据库消息映射


//数据库消息分发
void issue_db_msg(struct schedule *sche, void *arg)
{
	buffer *msg = NULL;
	int *cmd = NULL;
	msg_func_item *func_item;
	dbmsg_hander hander;
	for (;;)
	{
		//如果为空，让出协程控制权
		if (queue_empty(g_dbmsg_queue))	
		{
			usleep(10);
			//切换协程
			coroutine_yield(sche);
			continue;
		}

		//从队列中弹出一条数据,并读出命令码
		msg = (buffer *)queue_pop(g_dbmsg_queue);
		if (!msg) 
		{
			coroutine_yield(sche);
			continue;
		}
		buffer_read(msg, cmd, sizeof(int));

		//根据命令码查出函数并调用
		func_item = (msg_func_item *)map_get(g_dbmsg_map, cmd, sizeof(int));
		if (func_item && func_item->msg_func)
		{
			hander = (dbmsg_hander)func_item->msg_func;
			hander(read_ptr(msg), msg->len);
		}
		
		//回收利用
		recycle_buffer(msg);
	}
}

//运行函数
void *dbthread_run(void *args)
{
	//创建协程调度器
	g_schedule = coroutine_open();
	if (!g_schedule) return FAILURE;

	//创建协程
	int id = coroutine_new(g_schedule, issue_db_msg, NULL);
	if (id < 0) return FAILURE;
	
	//调度器运行
	coroutine_run(g_schedule);
}

//初始化数据库
int init_database()
{
	//申请内存
	g_dbmsg_queue = (queue *)malloc(sizeof(queue));
	if (!g_dbmsg_queue) return MEM_ERROR;

	//初始化队列
	int res = queue_init(g_dbmsg_queue, 5120);
	if (res != OP_QUEUE_SUCCESS) return FAILURE;

	//初始化数据库消息映射器
	g_dbmsg_map = (map *)malloc(sizeof(map));
	if (!g_dbmsg_map) return MEM_ERROR;
	if (map_init(g_dbmsg_map) != OP_MAP_SUCCESS) return MEM_ERROR;

	return SUCCESS;
}

//启动数据库线程
int start_database()
{
	//创建线程
	if (0 != pthread_create(&g_thread, NULL, dbthread_run, NULL))
	{
		return FAILURE;
	}
	else
	{
		pthread_setname_np(g_thread, "dbthread");
		return SUCCESS;
	}	
}

//注册数据库消息
int reg_db_msg(uint16_t msg, dbmsg_hander func)
{
	int *key = (int *)malloc(sizeof(int));
	*key = msg;

	msg_func_item *item = (msg_func_item *)malloc(sizeof(msg_func_item));
	if (!item) return MEM_ERROR;

	item->msg_func = func;
	if (map_put(g_dbmsg_map, key, sizeof(int), item) != OP_MAP_SUCCESS)
		return FAILURE;

	return SUCCESS;
}

//投递数据库消息
int post_db_msg(uint16_t msg, char *data, uint32_t len)
{
	buffer *buf = extract_buffer();
	if (!buf) return FAILURE;

	int res = buffer_write_int(buf, msg);
	if (res != SUCCESS) return res;

	res = buffer_write(buf, data, len);
	if (res != SUCCESS) return res;

	return queue_push_fixed(g_dbmsg_queue, buf);
}

