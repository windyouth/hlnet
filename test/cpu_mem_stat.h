#include <stdint.h>

//cpu状态
typedef struct _cpu_stat
{
	uint32_t usage;		//已用时间
	uint32_t all;		//总的时间
}cpu_stat;

//内存状态
typedef struct _mem_stat
{
	uint32_t total;		//总大小
	uint32_t free;		//空闲大小
}mem_stat;

//获取cpu状态
int get_cpu_stat(cpu_stat *stat);
//获取内存状态
int get_mem_stat(mem_stat *stat);

