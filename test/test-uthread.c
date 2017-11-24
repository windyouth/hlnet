#include "../uthread/uthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "cpu_mem_stat.h"

/*
void show_cpu_mem_stat()
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
*/

void func1(void * arg)
{
    puts("1");
    puts("11");
	//show_cpu_mem_stat();
    puts("111");
    puts("1111");
}

void func2(void * arg)
{
	char *str = "22";
    puts(str);
    puts(str);
    uthread_yield((schedule_t *)arg);
    puts(str);
	//show_cpu_mem_stat();
    puts(str);
}

void func3(void *arg)
{
	char *str = "3333";
    puts(str);
    puts(str);
    uthread_yield((schedule_t *)arg);
    puts(str);
	//show_cpu_mem_stat();
    puts(str);

}

void func4(void *arg)
{
	for (int i = 1; i > 0; ++i)
	{
		//uthread_sleep((schedule_t*)arg, 1000);
		printf("i = %d\n", i);
		//show_cpu_mem_stat();
		usleep(1);

    	uthread_yield((schedule_t *)arg);
	}
}

void context_test()
{
    char stack[1024*128];
	int a = 128;
    ucontext_t uc1, ucmain;

    getcontext(&uc1);
    uc1.uc_stack.ss_sp = stack;
    uc1.uc_stack.ss_size = 1024*128;
    uc1.uc_stack.ss_flags = 0;
    uc1.uc_link = &ucmain;
        
    makecontext(&uc1,(void (*)(void))func1,0);

    swapcontext(&ucmain,&uc1);
    puts("main");
	printf("a = %d\n", a);
}

void schedule_test()
{
	schedule_t *sche = schedule_create();
    
    int id2 = uthread_create(sche, func2);
    int id3 = uthread_create(sche, func3);
	int id4 = uthread_create(sche, func4);

    uthread_run(sche);

    puts("main over");
}

int main()
{

    //context_test();
    puts("----------------");
    schedule_test();

    return 0;
}
