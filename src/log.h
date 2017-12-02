#ifndef _LOG_H_
#define _LOG_H_

#include "../coroutine/coroutine.h"

//调用接口
#define	log(level, format, ...) do 											\
{																			\
	add_log(level, __FILE__, __func__, __LINE__, format, ##__VA_ARGS__);	\
} while(0)																	

//日志等级
typedef enum _log_level
{
	loglevel_error = 0,			//错误
	loglevel_warning = 1,		//警告
	loglevel_debug = 2,			//调试
	loglevel_info = 3			//信息
}log_level_e;

//初始化日志
int init_log(char *path, log_level_e level);
//添加日志
int add_log(log_level_e level, const char *file, const char *func, int line, 
			  const char *format, ...);
//写日志
void write_log(struct schedule *sche, void *arg);
//释放日志
void free_log();

extern char				*g_log_path;				//日志文件路径

#endif
