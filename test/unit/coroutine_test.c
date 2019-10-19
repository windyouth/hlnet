#include "../coroutine/coroutine.h"
#include <unistd.h>
#include <stdio.h>

struct args {
	int n;
};

static void
foo(struct schedule * S, void *ud) 
{
	struct args * arg = ud;
	int start = arg->n;
	int i;
	for (i=0;i>=0;i++) 
	{
		printf("coroutine %d : %d\n",coroutine_running(S) , start + i);
		sleep(1);
		coroutine_yield(S);
	}
}

static void
test(struct schedule *S) 
{
	struct args arg1 = { 0 };
	struct args arg2 = { 100 };

	int co1 = coroutine_new(S, foo, &arg1);
	int co2 = coroutine_new(S, foo, &arg2);
	printf("main start\n");
	while (coroutine_status(S,co1) && coroutine_status(S,co2)) 
	{
		coroutine_resume(S,co1);
		coroutine_resume(S,co2);
	} 
	printf("main end\n");
}

int main() 
{
	struct schedule * S = coroutine_open();

	struct args arg1 = { 0 };
	struct args arg2 = { 100 };
	int co1 = coroutine_new(S, foo, &arg1);
	int co2 = coroutine_new(S, foo, &arg2);

	printf("main start\n");
	coroutine_run(S);
	printf("main end\n");

	coroutine_close(S);
	
	return 0;
}

