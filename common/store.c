#include <assert.h>
#include "store.h"
#include "../common/common.h"

//创建仓库
list *create_store()
{
	list *store = (list *)malloc(sizeof(list));
	if (store) list_init(store);

	return store;
}

//取得内存块
void *extract_chunk(list *store, uint need)
{
	//参数检查
	assert(store);
	if (!store) return PARAM_ERROR;

	if (list_size(store) > 0) 
		return list_remove_first(store);

	return malloc(sizeof(list_item) + need);
}

