#include <stdio.h>
#include "../bin/include/log.h"
#include "../common/common.h"
#include "../common/store.h"
#include "../common/buffer.h"

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
	
	write_log(NULL);
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

	//写数据
	snprintf(write_ptr(buf), 128, "hello world");
	int len = strlen(write_ptr(buf));
	//定位指针
	seek_write(buf, len);

	char *str1;
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

int main()
{
	//log_test();
	//store_test();
	buffer_test();

	return 0;
}
