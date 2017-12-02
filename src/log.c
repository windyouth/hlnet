#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "stdarg.h"
#include "log.h"
#include "../common/common.h"
#include "../common/buffer.h"
#include "../c-stl/queue.h"
#include "../coroutine/coroutine.h"

#define				DEFAULT_LOG_LENGTH				256

char				*g_log_path = NULL;				//日志文件路径
FILE				*g_log_file = NULL;				//日志文件结构
log_level_e			g_log_level = loglevel_error;	//日志等级
queue				*g_log_queue = NULL;			//日志队列

//日志等级字符串
char g_level_string[4][8] = 
{
	"Error", 
	"Warning", 
	"Debug", 
	"Info"
};

//初始化日志
int init_log(char *path, log_level_e level)
{
	//参数检查
	assert(path);
	if (!path) return PARAM_ERROR;
	if (g_log_path) return REPEAT_ERROR;

	//申请内存
	g_log_path = (char *)malloc(PATH_LENGTH);
	if (!g_log_path) return MEM_ERROR;

	//写入
	snprintf(g_log_path, PATH_LENGTH, path);
	g_log_level = level;

	g_log_file = fopen(g_log_path, "a");
	if (!g_log_file) return IO_ERROR; 
	
	//初始化日志队列
	g_log_queue = (queue *)malloc(sizeof(queue));
	if (!g_log_queue) return MEM_ERROR;
	int res = queue_init(g_log_queue, 256);
	if (res != OP_QUEUE_SUCCESS) return FAILURE;

	return SUCCESS;
}

//添加日志
int add_log(log_level_e level, const char *file, const char *func, int line, 
			  const char *format, ...)
{
	//参数检查
	if (level > g_log_level) return FAILURE;

	//内存准备
	buffer *buf = extract_buffer();
	if (!buf) return FAILURE;

	int res = buffer_rectify(buf, DEFAULT_LOG_LENGTH);
	if (res != SUCCESS) return res;
	zero_array(write_ptr(buf), DEFAULT_LOG_LENGTH);

	//写日期
	time_t now = time(0);
	strftime(write_ptr(buf), 32, "%Y-%m-%d %H:%M:%S ", localtime(&now));
	res = strlen(write_ptr(buf));
	seek_write(buf, res);

	//写参数
	snprintf(write_ptr(buf), 128, "{%s %s() %d}[%s]: ", file, func, line, g_level_string[level]);
	res = strlen(write_ptr(buf));
	seek_write(buf, res);
	
	//写内容
	va_list va;
	va_start(va, format);
	vsnprintf(write_ptr(buf), buffer_surplus(buf) - 2, format, va);
	va_end(va);
	res = strlen(write_ptr(buf));
	seek_write(buf, res);
	//写结束换行符
	buffer_write(buf, "\n", 1);

	queue_push(g_log_queue, buf);
}

//写日志
void write_log(void *arg)
{
	buffer *item = NULL;
	for (;;)
	{
#ifdef TEST
			puts("执行write_log()");
#endif
		//如果为空，让出协程控制权
		if (queue_empty(g_log_queue))	
		{
			usleep(10);	/* 事关整个线程的休眠 */
			uthread_yield((schedule_t *)arg);
			continue;
		}

		//从队列中弹出一条数据
		item = (buffer *)queue_pop(g_log_queue);
		if (!item) 
		{
			uthread_yield((schedule_t *)arg);
			continue;
		}

		if (g_log_file)
			fwrite(read_ptr(item), 1, item->len, g_log_file);

		//回收利用
		recycle_buffer(item);
	}
}

//释放日志
void free_log()
{
	fclose(g_log_file);
}
