#include <assert.h>
#include "store.h"
#include "../common/common.h"
#include "../common/algorithm.h"

//创建仓库
store_t *create_store(uint chunk_size)
{
	store_t *store = (store_t *)malloc(sizeof(store_t));
	if (!store) return NULL;

	store->chunk_size = chunk_size;
	store->chunk_list = (list *)malloc(sizeof(list));
	if (!store->chunk_list) 
	{
		safe_free(store);

		return NULL;
	}

	list_init(store->chunk_list);

	return store;
}

//取得内存块
void *extract_chunk(store_t *store)
{
	//参数检查
	assert(store);
	if (!store) return NULL;

	//如果有现成的，直接取出。
	if (list_size(store->chunk_list) > 0) 
	{
		list_item *item = list_remove_first(store->chunk_list); 
		if (!item) return NULL;

		return (void *)((ulong)item + sizeof(list_item));
	}

	//如果没有，新申请一个。
    uint len = sizeof(list_item) + store->chunk_size;
	void *chunk = malloc(len);
	if (!chunk) return NULL;
    //内存块用零值初始化
    mem_zero(chunk, len);

	//list_item这个头部外面不要
	return (void *)((ulong)chunk + sizeof(list_item));
}

