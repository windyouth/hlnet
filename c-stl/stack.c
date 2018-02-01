#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

//创建栈
_stack *stack_create(int size)
{
	//参数检查
	assert(size > 0);
	if (size <= 0) return NULL;

	//内存申请
	_stack *stack = (_stack *)malloc(sizeof(_stack));
	if (!stack) return NULL;
	stack->table = (void **)malloc(size * sizeof(void *));
	if (!stack)
	{
		free(stack);
		return NULL;
	}

	//赋初值
	stack->size = size;
	stack->base = stack->top = 0;

	return NULL;
}

//入栈
int stack_push(_stack *stack, void *item)
{
	//参数检查
	assert(stack && item);
	if (!stack || !item) return STACK_PARAM_ERROR;
	
	//如果满了，扩展内存。
	if (stack_full(stack))
	{
		int new_size = stack->size + stack->size / 2;
		void **new_table = (void **)realloc(stack->table, new_size * sizeof(void *));
		if (!new_table) return STACK_MEM_ERROR;

		stack->table = new_table;
		stack->size = new_size;
	}

	stack->table[stack->top++] = item;

	return STACK_SUCCESS;
}

//出栈
void *stack_pop(_stack *stack)
{
	//参数检查
	assert(stack);
	if (!stack) return NULL;

	if (stack_empty(stack)) return NULL;

	return stack->table[stack->top--];
}
