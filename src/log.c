#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "log.h"
#include "../common/common.h"
#include "../common/buffer_store.h"
#include "../c-stl/queue.h"

#define				DEFAULT_LOG_LENGTH				256

char				*g_log_file = NULL;				//日志文件路径
log_level_e			g_log_level;					//日志等级

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

	//申请内存
	if (!g_log_file)
	{
		g_log_file = (char *)malloc(PATH_LENGTH);
		if (!g_log_file) return MEM_ERROR;
	}

	//写入
	snprintf(g_log_file, PATH_LENGTH, path);
	g_log_level = level;

	return buffer_store_init();
}

//写日志
int write_log(log_level_e level, const char *file, const char *func, int line, 
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
	snprintf(write_ptr(buf), 32, "%Y-%m-%d %H:%M:%S, ", localtime(&now));
	seek_write(buf, strlen(write_ptr(buf)));

	//写参数
	snprintf(write_ptr(buf), 128, "%s, %s, %d: [%s]", file, func, line, g_level_string[level]);
	seek_write(buf, strlen(write_ptr(buf)));
	
	//写内容
	va_list va;
	va_start(va, format);
	vsnprintf(write_ptr(buf), buffer_surplus(buf) - 2, format, va);
	va_end(va);
	seek_write(buf, strlen(write_ptr(buf)));
	//写结束换行符
	buffer_write(buf, '\n', 1);
}

