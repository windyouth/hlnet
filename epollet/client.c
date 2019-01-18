#include "client.h"
#include "../c-stl/array.h"


#define			CLI_BUF_ORIGIN_SIZE			256				//客户端结构体自带缓冲区初始大小

static list		*g_user_free = NULL;						//空闲的客户端链表
static array	*g_user_store = NULL;						//存储所有的客户端


//初始化客户端结构体
int client_init(client_t *cli, int size)
{
	//参数检查
	assert(cli && size > 0);
	if (!cli || size <= 0) return PARAM_ERROR;

	zero(cli);
	cli->fd = INVALID_SOCKET;
	cli->status.part = READ_PART_HEAD;
	cli->status.need = sizeof(cmd_head_t);

	//申请并初始化两个缓冲区
	cli->in = (buffer *)malloc(sizeof(buffer));
	if (!cli->in) return MEM_ERROR;
	int res = buffer_init(cli->in, size);
	if (res != SUCCESS) return res;

	cli->out = (buffer *)malloc(sizeof(buffer));
	if (!cli->out) return MEM_ERROR;
	res = buffer_init(cli->out, size);
	if (res != SUCCESS) return res;

	return SUCCESS;
}

//初始化客户端仓库
int client_store_init()
{
	if (g_user_free || g_user_store)
		return REPEAT_ERROR;

	//初始化空闲链表
	g_user_free = list_create();
	if (!g_user_free) return MEM_ERROR;

	//初始化仓库数组
	g_user_store = (array *)malloc(sizeof(array));
	if (!g_user_store) return MEM_ERROR;

	array_init(g_user_store, 2048);

	return SUCCESS;
}

//释放客户端仓库
void client_store_free()
{
	//释放数组
	array_free_deep(g_user_store);
	safe_free(g_user_store);
	//释放空闲链表
	safe_free(g_user_free);
}

//取得一个客户端
client_t *extract_client()
{
	if (list_size(g_user_free) > 0)
		return (client_t *)list_pop_front(g_user_free);
	
	client_t *item = (client_t *)malloc(sizeof(client_t));
	if (!item) return NULL;

	if (client_init(item, CLI_BUF_ORIGIN_SIZE) != SUCCESS)
	{
		client_free(item);

		return NULL;
	}

	item->id = array_size(g_user_store);
	array_push_back(g_user_store, item);
		
	return item;
}

//回收一个客户端
void recycle_client(client_t *cli)
{
	assert(cli);
	if (!cli) return;

	client_reset(cli);
	list_push_back(g_user_free, cli);
}

//根据id查询客户端
client_t *get_client(uint32_t id)
{
	assert(id < array_size(g_user_store));
	if (id >= array_size(g_user_store)) return NULL;

	return (client_t *)array_item(g_user_store, id);
}
