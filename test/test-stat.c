#include <stdio.h>
#include "cpu_mem_stat.h"

void main()
{
	cpu_stat stat_c;
	memset(&stat_c, 0, sizeof(stat_c));

	mem_stat stat_m;
	memset(&stat_m, 0, sizeof(stat_m));

	get_cpu_stat(&stat_c);
	get_mem_stat(&stat_m);
	
	printf("cpu usage: %u, cpu all: %u, cpu use_rate: %.2f\% \n", stat_c.usage, stat_c.all, (float)stat_c.usage / stat_c.all * 100);
	printf("mem total: %u, mem free: %u, mem use_rate: %.2f\% \n", stat_m.total, stat_m.free, (float)(stat_m.total - stat_m.free) / stat_m.total * 100 );
}
