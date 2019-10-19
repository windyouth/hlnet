#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdint.h>
#include "common.h"
#include "algorithm.h"

/*
 * description: 一个循环队列缓冲区。
 * 				里面所写的每段数据在内存上都是连续的，
 * 				写和读应该步调一致，如果写两段，读一段，不保证执行正确。
 * author: 何峦
 * email: heluan123132@163.com
 */


//从缓冲区获取读指针
#define read_ptr(buffer) 	((buffer)->buf + (buffer)->read)
//从缓冲区获取写指针
#define write_ptr(buffer) 	((buffer)->buf + (buffer)->write)
//从缓冲区获取定位指针
#define seek_ptr(buffer) 	((buffer)->buf + (buffer)->seek)

//缓冲区数据已占用长度
#define buffer_length(buffer)   ((buffer)->len)
//缓冲区剩余大小
#define buffer_surplus(buffer) 	((buffer)->size - (buffer)->len - (buffer)->gap)
//缓冲区是否为空
#define buffer_empty(bufer)    ((buffer)->len <= 0)

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
		(buf)->gap = 0;			                	\
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
	dst = read_ptr(buf);							\
	seek_read(buf, len);							\
}while (0)

//重置缓冲区
#define buffer_reset(buf) do 						\
{													\
	(buf)->read = 0;								\
	(buf)->write = 0;								\
	(buf)->seek = 0;								\
	(buf)->len = 0;									\
	(buf)->gap = 0;									\
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
	uint    	read;			//读索引
	uint    	write;			//写索引
    uint        seek;           //定位索引
	int		    len;			//数据已占用的总长度，不含尾部空长度
	int		    gap;			//尾部空长度
	uint	    size;			//缓冲区大小
	uint	    end;			//尾索引，内容的最末位置，最大值刚好为size
	char		*buf;			//起始地址指针
    char        init;           //是否初始化
}buffer, *pbuffer;


//初始化缓冲区
int buffer_init(buffer *buf, uint size);
//检查并调整缓冲区,使之具备need大小的连续空闲空间
int buffer_rectify(buffer *buf, uint need);
//往缓冲区写
int buffer_write(buffer *buf, char *src, uint len);
//往缓冲区写数字
int buffer_write_int(buffer *buf, int num);


//初始化缓冲区仓库
int buffer_store_init();
//释放缓冲区仓库
void buffer_store_free();

//取得一个缓冲区
buffer *extract_buffer();
//回收一个缓冲区
void recycle_buffer(buffer *buf);

#endif //_BUFFER_H_

