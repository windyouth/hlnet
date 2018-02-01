#ifndef _STACK_H_
#define _STACK_H_

#define			STACK_SUCCESS			0					//成功
#define			STACK_FAILURE			-1					//失败
#define			STACK_PARAM_ERROR		-2					//参数错误
#define			STACK_MEM_ERROR			-3					//内存错误

typedef struct stack
{
	int			size;			//总大小
	int			top;			//栈顶索引
	int 		base;			//栈底索引
	void		**table;		//内存区
}_stack;

#define			stack_full(stack)		((stack)->top == (stack)->size)		//是否已满
#define			stack_empty(stack)		((stack)->top == (stack)->base)		//是否已空

//遍历栈
#define stack_foreach(stack, item)										\
			for (int i = (stack)->base; 								\
				 (item) = (stack)->table[i], i != (stack)->top; 		\
				 i++)

//释放栈
#define stack_free(stack) do 				\
{											\
	if (stack->table)						\
		free(stack->table);					\
	free(stack);							\
	stack = NULL;							\
} while(0)

//释放栈(深度)
#define stack_free_deep(stack) do 			\
{											\
	void *item;								\
	stack_foreach(stack, item)				\
	{										\
		if (item) 							\
			free(item);						\
	}										\
											\
	stack_free(stack);						\
} while(0)

//创建栈
_stack *stack_create(int size);
//入栈
int stack_push(_stack *stack, void *item);
//出栈
void *stack_pop(_stack *stack);

#endif //_STACK_H_
