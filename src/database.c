#include "database.h"
#include "../c-stl/queue.h"
#include "../c-stl/map.h"
#include "pthread.h"
#include "../common/buffer_store.h"

pthread_t				g_thread = -1;

queue					*g_dbmsg_queue = NULL;
map						*g_dbmsg_map = NULL;					//数据库消息映射

//运行函数
void *dbthread_run(void *args)
{
	for (;;)
	{

	}

	return NULL;
}

//启动数据库线程
int start_dbthread()
{
	//申请内存
	g_dbmsg_queue = (queue *)malloc(sizeof(queue));
	if (!g_dbmsg_queue) return MEM_ERROR;

	//初始化队列
	zero(g_dbmsg_queue);
	int res = queue_init(g_dbmsg_queue, 5120);
	if (res != OP_QUEUE_SUCCESS) return FAILURE;

	//初始化数据库消息映射器
	g_dbmsg_map = (map *)malloc(sizeof(map));
	if (!g_dbmsg_map) return MEM_ERROR;
	if (map_init(g_dbmsg_map) != OP_MAP_SUCCESS) return MEM_ERROR;

	//初始化缓冲区仓库
	res = buffer_store_init();
	if (res != SUCCESS) return res;

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

