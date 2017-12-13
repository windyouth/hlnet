#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <sys/time.h>
#include <signal.h>
#include "../bin/include/log.h"
#include "../common/common.h"
#include "../common/store.h"
#include "../common/buffer.h"
#include "../epollet/client.h"
#include "../src/database.h"
#include "../src/keep_alive.h"

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
	char *star = "迈克尔·乔丹";
	log(loglevel_error, "篮球之神是：%s，他的场均得分是：%d分。", star, 30);
	log(loglevel_warning, "%s拿下了%d个得分王！", star, 10);
	log(loglevel_debug, "%s拿下了%d个得分王！", star, 10);
	log(loglevel_info, "%s拿下了%d个得分王！", star, 10);
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
	str1 = "醉里挑灯看剑！";
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
	src = "冷风如刀！";
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

#define			DB_LOGIN		0x000A			//登录消息号
#define			DB_REGISTER		0xFFFF			//注册消息号

//登录结构体
typedef struct 
{
	char		account[32];
	char		password[32];
}login_info;

//注册结构体
typedef struct
{
	char		account[32];
	char		password[32];
	char		name[8];
	uchar		sex;
	char 		addr[64];
	char		remark[64];
}reg_info;

//处理登录数据库消息
int deal_login_msg(char *data, uint32_t len)
{
	assert(data && len == sizeof(login_info));
	if (!data || len != sizeof(login_info)) return PARAM_ERROR;

	login_info *login = (login_info *)data;

	puts("收到登录数据如下:");
	printf("账号：%s \n", login->account);
	printf("密码：%s \n", login->password);

	return SUCCESS;
}

//处理注册数据库消息
int deal_reg_msg(char *data, uint32_t len)
{
	assert(data && len == sizeof(reg_info));
	if (!data || len != sizeof(reg_info)) return PARAM_ERROR;

	reg_info *reg = (reg_info *)data;

	puts("收到注册数据如下:");
	printf("账号：%s \n", reg->account);
	printf("密码：%s \n", reg->password);
	printf("姓名：%s \n", reg->name);
	printf("性别：%s \n", reg->sex == 1 ? "男" : "女");
	printf("地址：%s \n", reg->addr);
	printf("备注：%s \n", reg->remark);

	return SUCCESS;
}

/* 数据库线程测试 */
void dbtest()
{
	//初始化数据库
	init_database();
	//注册数据库消息
	reg_db_msg(DB_LOGIN, deal_login_msg);
	reg_db_msg(DB_REGISTER, deal_reg_msg);
	//启动数据库线程
	start_database();

	//构造测试数据
	login_info *login_buf = (login_info *)malloc(sizeof(login_info));
	reg_info *reg_buf = (reg_info *)malloc(sizeof(reg_info));
	if (!login_buf || !reg_buf)
	{	
		puts("malloc failed");
		return;
	}

	bzero(login_buf, sizeof(login_info));
	bzero(reg_buf, sizeof(reg_info));

	snprintf(login_buf->account, 32, "fuck001");
	snprintf(login_buf->password, 32, "abc123");

	snprintf(reg_buf->account, 32, "fuck002");
	snprintf(reg_buf->password, 32, "abc456");
	snprintf(reg_buf->name, 8, "门罗");
	reg_buf->sex = 1;
	snprintf(reg_buf->addr, 64, "美国佐治亚州红脖子俱乐部");
	snprintf(reg_buf->remark, 64, "hi everybody, I am from American.");

	post_db_msg(DB_LOGIN, (char *)login_buf, sizeof(*login_buf));
	post_db_msg(DB_REGISTER, (char *)reg_buf, sizeof(*reg_buf));
	post_db_msg(DB_LOGIN, (char *)login_buf, sizeof(*login_buf));
	post_db_msg(DB_REGISTER, (char *)reg_buf, sizeof(*reg_buf));
	post_db_msg(DB_LOGIN, (char *)login_buf, sizeof(*login_buf));
	post_db_msg(DB_REGISTER, (char *)reg_buf, sizeof(*reg_buf));
	post_db_msg(DB_LOGIN, (char *)login_buf, sizeof(*login_buf));
	post_db_msg(DB_REGISTER, (char *)reg_buf, sizeof(*reg_buf));
	post_db_msg(DB_LOGIN, (char *)login_buf, sizeof(*login_buf));
	post_db_msg(DB_REGISTER, (char *)reg_buf, sizeof(*reg_buf));

	while (1)
		usleep(10);
}

client_t *g_client1;
client_t *g_client2;
client_t *g_client3;
client_t *g_client4;
client_t *g_client5;

//心跳模块测试
void alive_test()
{
	if (SUCCESS != keep_alive())
	{
		puts("初始化心跳模块失败");
		return;
	}

	if (SUCCESS != client_store_init())
	{
		puts("初始化客户端仓库失败");
		return;
	}

	g_client1 = extract_client();
	g_client2 = extract_client();
	g_client3 = extract_client();
	g_client4 = extract_client();
	g_client5 = extract_client();

	g_client1->fd = 1001;
	g_client2->fd = 1002;
	g_client3->fd = 1003;
	g_client4->fd = 1004;
	g_client5->fd = 1005;

	add_alive(g_client1->id);
	add_alive(g_client2->id);
	add_alive(g_client3->id);
	add_alive(g_client4->id);
	add_alive(g_client5->id);

	alive(g_client1->id);
	alive(g_client2->id);
	alive(g_client3->id);
	alive(g_client4->id);
	alive(g_client5->id);
	
	safe(g_client1->id);
	safe(g_client3->id);
	safe(g_client5->id);

	printf("g_client1 safe: %d \n", is_safe(g_client1->id));
	printf("g_client2 safe: %d \n", is_safe(g_client2->id));
	printf("g_client3 safe: %d \n", is_safe(g_client3->id));
	printf("g_client4 safe: %d \n", is_safe(g_client4->id));
	printf("g_client5 safe: %d \n", is_safe(g_client5->id));

	while (1)
		usleep(10);
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
	//dbtest();
	//pthread_test();
	alive_test();

	return 0;
}
