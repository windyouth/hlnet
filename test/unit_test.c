#include <stdio.h>
#include "../bin/include/log.h"
#include "../common/common.h"
#include "../common/store.h"

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
	void *chunk5 = extract_chunk(store);
	void *chunk6 = extract_chunk(store);

	printf("chunk1: %p\n", chunk1);
	printf("chunk2: %p\n", chunk2);
	printf("chunk2: %p\n", chunk3);
	printf("chunk4: %p\n", chunk4);
	printf("chunk5: %p\n", chunk5);
	printf("chunk6: %p\n", chunk6);
}

int main()
{
	//log_test();
	store_test();

	return 0;
}
