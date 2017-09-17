#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "cpu_mem_stat.h"

#define skip_token(p)	 while (*p != ' ' && *p != '\0') ++p

//获取cpu状态
int get_cpu_stat(cpu_stat *stat)
{
	int file = 0;
	char buf[256] = { 0 };
	char *p;

	file = open("/proc/stat", O_RDONLY);
	int len = read(file, buf, sizeof(buf) - 1);
	close(file);
	buf[len] = 0;

	memset(stat, 0, sizeof(*stat));

	p = buf;
	skip_token(p);

	int cpu_user = strtoul(p, &p, 10);
	int cpu_nice = strtoul(p, &p, 10);
	int cpu_sys = strtoul(p, &p, 10);
	int cpu_idle = strtoul(p, &p, 10);
	stat->usage = cpu_user + cpu_nice + cpu_sys;
	stat->all = cpu_user + cpu_nice + cpu_sys + cpu_idle;

	return 0;
}

//获取内存状态
int get_mem_stat(mem_stat *stat)
{
	int file = 0;
	char buf[256] = { 0 };
	char *p;

	file = open("/proc/meminfo", O_RDONLY);
	int len = read(file, buf, sizeof(buf) - 1);
	close(file);
	buf[len] = 0;

	memset(stat, 0, sizeof(*stat));
	
	p = buf;
	skip_token(p);
	stat->total = strtoul(p, &p, 10);
	
	p = strchr(p, '\n');
	skip_token(p);
	stat->free = strtoul(p, &p, 10);

	return 0;
}


