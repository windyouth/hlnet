#include "database.h"
#include "../c-stl/queue.h"
#include "pthread.h"

thread_t				g_thread = -1;

queue					*g_dbqueue = NULL;
map						*g_dbmsg_map = NULL;					//数据库消息映射

//运行函数
int dbthread_run(void *args)
{
	return SUCCESS;
}

//启动数据库线程
int start_dbthread()
{
	//申请内存
	g_dbqueue = (queue *)malloc(sizeof(queue));
	if (!g_dbqueue) return MEM_ERROR;

	zero(g_dbqueue);
	int res = queue_init(g_dbqueue, 5120);
	if (res != QP_QUEUE_SUCCESS) return FAILURE;

	//创建线程
	res = pthread_create(g_thread, NULL, dbthread_run, 0);
	if (res != 0)
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
	char *key = (char *)malloc(8);
	zero_array(key, 8);
	sprintf(key, "%u", msg);

	if (map_put(g_dbmsg_map, key, strlen(key), func) != OP_MAP_SUCCESS)
		return FAILURE;

	return SUCCESS;
}

//投递数据库消息
int post_db_msg(uint16_t msg, char *data, int len)
{
}

