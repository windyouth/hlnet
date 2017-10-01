#include "buffer_store.h"
#include "../c-stl/list.h"


#define			DEFAULT_BUFFER_SIZE				256

list			*g_buffer_store = NULL;			//缓冲区仓库


//初始化缓冲区仓库
int buffer_store_init()
{
	if (g_buffer_store) return SUCCESS;

	//初始化空闲链表
	g_buffer_store = (list *)malloc(sizeof(list));
	if (!g_buffer_store) return MEM_ERROR;

	list_init(g_buffer_store);

	return SUCCESS;
}

//释放缓冲区仓库
void buffer_store_free()
{
	//释放空闲链表
	list_free_deep(g_buffer_store);
	safe_free(g_buffer_store);
}

//取得一个缓冲区
buffer *extract_buffer()
{
	if (list_size(g_buffer_store) > 0)
		return (buffer *)list_remove_first(g_buffer_store);
	
	buffer *item = (buffer *)malloc(sizeof(buffer));
	if (!item) return NULL;

	if (buffer_init(item, DEFAULT_BUFFER_SIZE) != SUCCESS)
	{
		buffer_free(item);

		return NULL;
	}

	return item;
}

//回收一个缓冲区
void recycle_buffer(buffer *buf)
{
	assert(!buf);
	if (!buf) return;

	buffer_reset(buf);
	list_push_back(g_buffer_store, buf);
}
