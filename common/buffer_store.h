#ifndef _BUFFER_STORE_H_
#define _BUFFER_STORE_H_

#include "buffer.h"

//初始化缓冲区仓库
int buffer_store_init();
//释放缓冲区仓库
void buffer_store_free();

//取得一个缓冲区
buffer *extract_buffer();
//回收一个缓冲区
void recycle_buffer(buffer *buf);

#endif //_BUFFER_STORE_H_
