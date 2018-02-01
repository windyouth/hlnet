#ifndef _STACK_H_
#define _STACK_H_

typedef struct stack
{
	int			size;			//总大小
	int 		base;			//栈底索引
	int			top;			//栈顶索引
	void		**table;		//内存区
}_stack;

//创建栈
_stack *stack_create(int size);
//释放栈
void stack_free(_stack *stack);
//入栈
int stack_push(_stack *stack);
//出栈
void *stack_pop(_stack *stack);

#endif //_STACK_H_
