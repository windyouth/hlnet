#ifndef _STORE_H_
#define _STORE_H_

#include <sys/types.h>
#include "../c-stl/list.h"
#include "../common/algorithm.h"

//仓库结构体
typedef struct _store
{
	uint		chunk_size;			//内存块大小
	list		*chunk_list;		//内存块链表
}store_t, *pstore;

//销毁仓库
#define destroy_store(store) do 						\
{														\
	list_free_deep(store->chunk_list);					\
	safe_free(store->chunk_list);						\
	safe_free(store);									\
} while(0)

//回收内存块
#define recycle_chunk(store, chunk) do 												\
{																					\
	list_push_back(store->chunk_list, (void *)((ulong)chunk - sizeof(list_item)));	\
} while(0)

//创建仓库
store_t *create_store(uint chunk_size);
//取得内存块
void *extract_chunk(store_t *store);

#endif
