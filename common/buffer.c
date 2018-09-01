#include <assert.h>
#include "buffer.h"
#include "store.h"
#include "../c-stl/list.h"

#define					BUFFER_ORIGINAL_SIZE				256

static store_t			*g_buffer_store = NULL;				//缓冲区仓库

//初始化缓冲区
int buffer_init(buffer *buf, uint32_t size)
{
	//检查参数
	assert(buf && size);
	if (!buf || !size) return PARAM_ERROR;

	zero(buf);
	buf->buf = (char *)malloc(size);
	if (!buf->buf) return MEM_ERROR;

	buf->size = size;

	return SUCCESS;
}

//重新申请内存
static int buffer_realloc(buffer *buf, uint32_t new_size)
{
	if (!buf || new_size == 0) return PARAM_ERROR;

	buf->buf = (char *)realloc(buf->buf, new_size);
	if (!buf->buf) return MEM_ERROR;

	//如果是中空结构，read以下的数据全部下移到底部。
	if (buf->len > 0 && buf->read >= buf->write)
	{
		int i = buf->end, index = new_size;
		while (i > buf->read) 
			buf->buf[--index] = buf->buf[--i];

		buf->read = index;
		buf->end = new_size;
	}

	//重置参数
	buf->size = new_size;
    //尾部空缺被消除
    buf->gap = 0;

	return SUCCESS;
}

//检查并调整缓冲区,使之具备need大小的连续空闲空间
int buffer_rectify(buffer *buf, uint32_t need)
{
	//检查参数
	if (!buf || need == 0) return PARAM_ERROR;

	uint32_t new_size = buf->size + max(buf->size / 2, need * 10);

	//剩余空间不足,则重新申请大小
	if (buffer_surplus(buf) < need ||
		buf->write < buf->read && buf->write + need > buf->read)
	{
		return buffer_realloc(buf, new_size);	
	}
	
	//允许刚好写满
	if (buf->write >= buf->read && buf->write + need > buf->size)
	{
		if (buf->len == 0)
		{
			//如果为空，直接归零重写。
			buffer_reset(buf);
			return SUCCESS;
		}
		else if (buf->read >= need)
		{
			//写指针移到开头
			buf->write = 0;
			//记录尾部空缺
			buf->gap = buf->size - buf->end;
			
			return SUCCESS;
		}
		else
		{
			return buffer_realloc(buf, new_size);	
		}
	}

	return SUCCESS;
}

//往缓冲区写
int buffer_write(buffer *buf, char* src, uint32_t len)
{
	assert(buf && src);
	if (!buf || !src) return PARAM_ERROR;

	int res = buffer_rectify(buf, len + 1);
	if (res != SUCCESS) return res;

	memcpy(write_ptr(buf), src, len);
	seek_write(buf, len);

	//写结束符
	*write_ptr(buf) = 0;

	return SUCCESS;
}

//往缓冲区写数字
int buffer_write_int(buffer *buf, int num)
{
	assert(buf);
	if (!buf) return PARAM_ERROR;

	int res = buffer_rectify(buf, sizeof(int) + 1);
	if (res != SUCCESS) return res;

	int *int_ptr = (int *)write_ptr(buf);
	*int_ptr = num;

	seek_write(buf, sizeof(int));

	//写结束符，但不偏移写指针。
	//这样结束符在读的时候起作用，但写的时候不产生影响。
	*write_ptr(buf) = 0;

	return SUCCESS;
}


//初始化缓冲区仓库
int buffer_store_init()
{
    if (!g_buffer_store)
	{
		g_buffer_store = create_store(sizeof(buffer));
		if (!g_buffer_store) return FAILURE;
	}

    return SUCCESS;
}

//释放缓冲区仓库
void buffer_store_free()
{
    if (g_buffer_store)
        destroy_store(g_buffer_store);
}

//取得一个缓冲区
buffer *extract_buffer()
{
	if (!g_buffer_store)
        return NULL;
	
	buffer *buf = (buffer *)extract_chunk(g_buffer_store);
	if (!buf) return NULL;

	if (buffer_init(buf, BUFFER_ORIGINAL_SIZE) != SUCCESS)
	{
		recycle_chunk(g_buffer_store, buf);
		return NULL;
	}

	return buf;
}

//回收一个缓冲区
void recycle_buffer(buffer *buf)
{
	assert(buf);
	if (!buf) return;

	buffer_reset(buf);
	recycle_chunk(g_buffer_store, buf);
}

