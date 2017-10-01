#ifndef _STORE_H_
#define _STORE_H_

#include <stdint.h>
#include "../c-stl/list.h"

//创建仓库
list *create_store();
//取得内存块
void *extract_chunk(list *store, uint32_t need);
//回收内存块
void recycle_chunk(list *store, void *block);
//关闭仓库
void destroy_store(list *store);

#endif
