#include <stdio.h>
#include <pthread.h>
#include "../bin/include/log.h"
#include "../common/common.h"
#include "../common/store.h"
#include "../common/buffer.h"
#include "../epollet/client.h"
#include "../src/database.h"

#define hello(str) do 		\
{							\
	puts(str);				\
} while(0)

void log_test()
{
	int res = init_log("./log.txt", loglevel_info);
	if (res != SUCCESS)
	{
		printf("init_log failure, errno: %d\n", res);
		return;
	}
	char *girl = "姚子淑";
	log(loglevel_error, "我的宝贝是：%s，她的年龄是：%d岁。", girl, 27);
	log(loglevel_warning, "我爱%s%d年！", girl, 10000);
	log(loglevel_debug, "我爱%s%d年！", girl, 10000);
	log(loglevel_info, "我爱%s%d年！", girl, 10000);
}

void store_test()
{
	store_t *store = create_store(32);
	if (!store)
		puts("create store failure");

	void *chunk1 = extract_chunk(store);
	void *chunk2 = extract_chunk(store);
	void *chunk3 = extract_chunk(store);
	void *chunk4 = extract_chunk(store);
	recycle_chunk(store, chunk1);
	recycle_chunk(store, chunk2);
	recycle_chunk(store, chunk3);
	recycle_chunk(store, chunk4);
	void *chunk5 = extract_chunk(store);
	void *chunk6 = extract_chunk(store);
	void *chunk7 = extract_chunk(store);
	void *chunk8 = extract_chunk(store);

	printf("chunk1: %p\n", chunk1);
	printf("chunk2: %p\n", chunk2);
	printf("chunk2: %p\n", chunk3);
	printf("chunk4: %p\n", chunk4);
	printf("chunk5: %p\n", chunk5);
	printf("chunk6: %p\n", chunk6);
	printf("chunk7: %p\n", chunk7);
	printf("chunk8: %p\n", chunk8);
}

void buffer_test()
{
	//取得一个缓冲区
	buffer *buf = extract_buffer();
	if (!buf) return;

	//申请大小
	int res = buffer_rectify(buf, 256);
	if (res != SUCCESS) return;

	buf->write = 244;
	buf->read = 0;
	buf->len = 244;
	buf->end = 244;

	//写数据
	/*
	snprintf(write_ptr(buf), 128, "hello world");
	int len = strlen(write_ptr(buf));
	//定位指针
	seek_write(buf, len);
	*/
	char *src = "hello world";
	int len = strlen(src);
	buffer_write(buf, src, len);

	char *str1;
	buf->read += 244;
	buf->len -= 244;
	buffer_read(buf, str1, len);
	puts(str1);

	buffer_write_int(buf, 5201314);
	str1 = "姚子淑我想你！";
	len = strlen(str1);
	buffer_write(buf, str1, len);

	int *code;
	buffer_read(buf, code, sizeof(int));
	printf("code: %d\n", *code);
	char *str2;
	buffer_read(buf, str2, len);
	puts(str2);

	//回收缓冲区
	recycle_buffer(buf);
}

void rectify_test()
{
	//取得一个缓冲区
	buffer *buf = extract_buffer();
	if (!buf) return;

	buf->read = 0;
	buf->write = 245;
	buf->end = 245;
	buf->len = 245;

	printf("read: %d, write: %d, end: %d, len: %d, size: %d\n", 
			buf->read, buf->write, buf->end, buf->len, buf->size);

	buffer_rectify(buf, 12);
	puts("------------buffer_rectify(buf, 12)-----------");
	printf("read: %d, write: %d, end: %d, len: %d, size: %d\n", 
			buf->read, buf->write, buf->end, buf->len, buf->size);

	buf->read = 200;
	buf->write = 180;
	buf->end = 250;
	buf->len = 230;

	printf("read: %d, write: %d, end: %d, len: %d, size: %d\n", 
			buf->read, buf->write, buf->end, buf->len, buf->size);

	buffer_rectify(buf, 16);
	puts("------------buffer_rectify(buf, 16)-----------");
	printf("read: %d, write: %d, end: %d, len: %d, size: %d\n", 
			buf->read, buf->write, buf->end, buf->len, buf->size);

	buffer_rectify(buf, 20);
	puts("------------buffer_rectify(buf, 20)-----------");
	printf("read: %d, write: %d, end: %d, len: %d, size: %d\n", 
			buf->read, buf->write, buf->end, buf->len, buf->size);

#ifdef RECTIFY22
	buffer_rectify(buf, 22);
	puts("------------buffer_rectify(buf, 22)-----------");
	printf("read: %d, write: %d, end: %d, len: %d, size: %d\n", 
			buf->read, buf->write, buf->end, buf->len, buf->size);
#endif

#ifdef RECTIFY100
	buffer_rectify(buf, 100);
	puts("------------buffer_rectify(buf, 100)-----------");
	printf("read: %d, write: %d, end: %d, len: %d, size: %d\n", 
			buf->read, buf->write, buf->end, buf->len, buf->size);
#endif

	puts("-----------------------------------------------------");

	buf->read = 57;
	buf->write = 200;
	buf->end = 200;
	buf->len = 143;
	
	printf("read: %d, write: %d, end: %d, len: %d, size: %d\n", 
			buf->read, buf->write, buf->end, buf->len, buf->size);

	buffer_rectify(buf, 56);
	puts("------------buffer_rectify(buf, 56)-----------");
	printf("read: %d, write: %d, end: %d, len: %d, size: %d\n", 
			buf->read, buf->write, buf->end, buf->len, buf->size);

	buffer_rectify(buf, 57);
	puts("------------buffer_rectify(buf, 57)-----------");
	printf("read: %d, write: %d, end: %d, len: %d, size: %d\n", 
			buf->read, buf->write, buf->end, buf->len, buf->size);

	buf->read = 55;
	buf->write = 200;
	buf->len = 145;
	puts("------------buf->read = 55-----------");
	printf("read: %d, write: %d, end: %d, len: %d, size: %d\n", 
			buf->read, buf->write, buf->end, buf->len, buf->size);

	buffer_rectify(buf, 57);
	puts("------------buffer_rectify(buf, 57)-----------");
	printf("read: %d, write: %d, end: %d, len: %d, size: %d\n", 
			buf->read, buf->write, buf->end, buf->len, buf->size);
}

//写测试
void buffer_write_test()
{
	//取得一个缓冲区
	buffer *buf = extract_buffer();
	if (!buf) return;

	buf->write = 244;
	buf->read = 0;
	buf->len = 244;
	buf->end = 244;

	//连续写
	char *src = "hello world";
	int len = strlen(src);
	buffer_write(buf, src, len);

	buffer_write_int(buf, 5201314);
	src = "姚子淑我想你！";
	len = strlen(src);
	buffer_write(buf, src, len);

	//连续读
	char *str1;
	buf->read += 244;
	buf->len -= 244;
	buffer_read(buf, str1, 11);
	puts(str1);
	
	int *code;
	buffer_read(buf, code, sizeof(int));
	printf("code: %d\n", *code);
	char *str2;
	buffer_read(buf, str2, len);
	puts(str2);

	//回收缓冲区
	recycle_buffer(buf);
}

void buffer_read_test()
{
	//取得一个缓冲区
	buffer *buf = extract_buffer();
	if (!buf) return;

	buf->write = 244;
	buf->read = 244;
	buf->end = 244;
	buf->len = 0;

	char *src = "hello world";
	int len1 = strlen(src);
	buffer_write(buf, src, len1);

	puts("------------buffer_write(hello world)-----------");
	printf("read: %d, write: %d, end: %d, len: %d, size: %d\n", 
			buf->read, buf->write, buf->end, buf->len, buf->size);

	src = "I love yaozishu forever";
	int len2 = strlen(src);
	buffer_write(buf, src, len2);

	puts("------------buffer_write(I love yaozishu forever)-----------");
	printf("read: %d, write: %d, end: %d, len: %d, size: %d\n", 
			buf->read, buf->write, buf->end, buf->len, buf->size);

	char *dst;
	buffer_read(buf, dst, len1);
	puts(dst);

	puts("------------buffer_read(buf, dst, len)-----------");
	printf("read: %d, write: %d, end: %d, len: %d, size: %d\n", 
			buf->read, buf->write, buf->end, buf->len, buf->size);

	buffer_read(buf, dst, len2);
	puts(dst);

	puts("------------buffer_read(buf, dst, len)-----------");
	printf("read: %d, write: %d, end: %d, len: %d, size: %d\n", 
			buf->read, buf->write, buf->end, buf->len, buf->size);

	buf->write = 248;
	buf->read = 248;
	buf->end = 255;
	buf->len = 1;
	
	char *src3 = "yao dan";
	int len3 = strlen(src3);
	buffer_write(buf, src3, len3);

	puts("------------buffer_write(yao dan)-----------");
	printf("read: %d, write: %d, end: %d, len: %d, size: %d\n", 
			buf->read, buf->write, buf->end, buf->len, buf->size);

	buffer_read(buf, dst, len3);
	puts(dst);

	puts("------------buffer_read(yao dan)-----------");
	printf("read: %d, write: %d, end: %d, len: %d, size: %d\n", 
			buf->read, buf->write, buf->end, buf->len, buf->size);
}

void client_test()
{
	int res = client_store_init();
	if (res != SUCCESS) return;

	client_t *cli1 = extract_client();
	client_t *cli2 = extract_client();
	client_t *cli3 = extract_client();
	client_t *cli4 = extract_client();
	recycle_client(cli1);
	recycle_client(cli2);
	recycle_client(cli3);
	recycle_client(cli4);
	client_t *cli5 = extract_client();
	client_t *cli6 = extract_client();
	client_t *cli7 = extract_client();
	client_t *cli8 = extract_client();

	printf("cli1: %p, id: %d \n", cli1, cli1->id);
	printf("cli2: %p, id: %d \n", cli2, cli2->id);
	printf("cli3: %p, id: %d \n", cli3, cli3->id);
	printf("cli4: %p, id: %d \n", cli4, cli4->id);
	printf("cli5: %p, id: %d \n", cli5, cli5->id);
	printf("cli6: %p, id: %d \n", cli6, cli6->id);
	printf("cli7: %p, id: %d \n", cli7, cli7->id);
	printf("cli8: %p, id: %d \n", cli8, cli8->id);

	recycle_client(cli3);
	cli7 = extract_client();
	recycle_client(cli4);
	cli8 = extract_client();

	printf("get_client(cli7->id) = %p, cli7->id = %d \n", 
			get_client(cli7->id), cli7->id);
	printf("get_client(cli8->id) = %p, cli8->id = %d \n", 
			get_client(cli8->id), cli8->id);
}

/* 数据库线程测试 */
void dbtest()
{
	log_test();
	start_database();

	sleep(5);
}

void *pthread_run(void *args)
{
	puts("线程运行");
}

pthread_t				g_thread_test = -1;
void pthread_test()
{
	int res; // = pthread_create(&g_thread_test, NULL, pthread_run, NULL);
	if (res != 0) 
		puts("创建线程失败");
	else
	{
		sleep(2);
		printf("主线程ID：%d\n", (unsigned)pthread_self());
	}
}

int main()
{
	//log_test();
	//store_test();
	//buffer_test();
	//rectify_test();
	//buffer_write_test();
	//buffer_read_test();
	//client_test();
	dbtest();
	//pthread_test();

	return 0;
}
