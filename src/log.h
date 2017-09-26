#ifndef _LOG_H_
#define _LOG_H_

//调用接口
#define	log(level, format, ...) do 											\
{																			\
	write_log(level, __FILE__, __func__, __LINE__, format, ##__VA_ARGS__);	\
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
//写日志
int write_log(log_level_e level, const char *file, const char *func, int line, 
			  const char *format, ...);

#endif
