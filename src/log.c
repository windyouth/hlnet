#include <assert.h>
#include <stdio.h>
#include "log.h"
#include "../common/common.h"
#include "../common/buffer_store.h"

char				*g_log_file = NULL;				//日志文件路径
log_level_e			g_log_level;					//日志等级

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
int log(log_level_e level, const char *format, ...)
{
	if (level > g_log_level) return FAILURE;

	buffer *buf = extract_buffer();
	if (!buf) return FAILURE;
}

