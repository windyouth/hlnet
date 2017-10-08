#ifndef _STORE_H_
#define _STORE_H_

#include <sys/types.h>
#include "../c-stl/list.h"
#include "../common/algorithm.h"

//销毁仓库
#define destroy_store(store) do 						\
{														\
	list_free_deep(store);								\
	safe_free(store);									\
} while(0)

//回收内存块
#define recycle_chunk(store, chunk) do 					\
{														\
	list_push_back(store, chunk - sizeof(list_item));	\
} while(0)

//创建仓库
list *create_store();
//取得内存块
void *extract_chunk(list *store, uint need);

#endif
