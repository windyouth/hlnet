#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdint.h>
#include "common.h"
#include "algorithm.h"

/*
 * description: 一个循环队列缓冲区，里面所写的每段数据在内存上都是连续的。
 * author: 何峦
 * email: heluan123132@163.com
 */


//从缓冲区获取读指针
#define read_ptr(buffer) 	((buffer)->buf + (buffer)->read)
//从缓冲区获取写指针
#define write_ptr(buffer) 	((buffer)->buf + (buffer)->write)

//缓冲区剩余大小
#define buffer_surplus(buffer) 	((buffer)->size - (buffer)->len)
//缓冲区尾部空缺
#define buffer_gap(buffer)		((buffer)->size - (buffer)->end)

//偏移读指针
#define seek_read(buf, off) do						\
{													\
	(buf)->read += (off);							\
	(buf)->len -= (off);							\
	/* 												\
	 * 读和写如果长度没有前后一致，					\
	 * 此处不保证正确执行。 						\
	 */    											\
	if ((buf)->read >= (buf)->end && 				\
		(buf)->read != (buf)->write) 				\
	{												\
		(buf)->read = 0;							\
		(buf)->len -= buffer_gap(buf);				\
		(buf)->end = (buf)->write;					\
	}												\
}while (0)

//偏移写指针
#define seek_write(buf, off) do						\
{													\
	(buf)->len += (off);							\
	(buf)->write += (off);							\
	(buf)->end = max((buf)->write, (buf)->end);		\
	if ((buf)->write == (buf)->size)				\
		(buf)->write = 0;							\
}while (0)

//从缓冲区读数据
#define buffer_read(buf, dst, len) do 				\
{													\
	if ((buf)->read + (len) > (buf)->size)			\
		seek_read(buf, (buf)->size - (buf)->read);	\
	dst = read_ptr(buf);							\
	seek_read(buf, len);							\
}while (0)

//重置缓冲区
#define buffer_reset(buf) do 						\
{													\
	(buf)->read = 0;								\
	(buf)->write = 0;								\
	(buf)->len = 0;									\
	(buf)->end = 0;									\
}while (0)

//释放缓冲区
#define buffer_free(buffer) do						\
{													\
	safe_free((buffer)->buf);						\
	safe_free(buffer);								\
}while (0)

//缓冲区结构体
typedef struct _buffer
{
	uint32_t	read;			//读索引
	uint32_t	write;			//写索引
	int32_t		len;			//数据已占用的总长度，包含尾部可能出现的空长度
	uint32_t	size;			//缓冲区大小
	uint32_t	end;			//尾索引，内容的最末位置，最大值刚好为size
	char		*buf;			//起始地址指针
}buffer, *pbuffer;


//初始化缓冲区
int buffer_init(buffer *buf, uint32_t size);
//检查并调整缓冲区,使之具备need大小的连续空闲空间
int buffer_rectify(buffer *buf, uint32_t need);
//往缓冲区写
int buffer_write(buffer *buf, char *src, uint32_t len);
//往缓冲区写数字
int buffer_write_int(buffer *buf, int num);

//取得一个缓冲区
buffer *extract_buffer();
//回收一个缓冲区
void recycle_buffer(buffer *buf);

#endif //_BUFFER_H_

