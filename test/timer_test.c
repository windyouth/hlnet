#include <stdio.h>
#include <stdlib.h>
#include "../hlnet/include/heap.h"
#include "../hlnet/include/stack.h"

typedef struct student
{
	as_heap_item;
	char		name[16];
}_student;

void heap_test_data(heap *heap)
{
	heap_item *item1 = (heap_item *)malloc(sizeof(heap_item));
	heap_item *item2 = (heap_item *)malloc(sizeof(heap_item));
	heap_item *item3 = (heap_item *)malloc(sizeof(heap_item));
	heap_item *item4 = (heap_item *)malloc(sizeof(heap_item));
	heap_item *item5 = (heap_item *)malloc(sizeof(heap_item));
	heap_item *item6 = (heap_item *)malloc(sizeof(heap_item));
	heap_item *item7 = (heap_item *)malloc(sizeof(heap_item));
	heap_item *item8 = (heap_item *)malloc(sizeof(heap_item));
	heap_item *item9 = (heap_item *)malloc(sizeof(heap_item));
	heap_item *item10 = (heap_item *)malloc(sizeof(heap_item));
	heap_item *item11 = (heap_item *)malloc(sizeof(heap_item));
	heap_item *item12 = (heap_item *)malloc(sizeof(heap_item));
	heap_item *item13 = (heap_item *)malloc(sizeof(heap_item));
	heap_item *item14 = (heap_item *)malloc(sizeof(heap_item));
	heap_item *item15 = (heap_item *)malloc(sizeof(heap_item));

	item1->key = 7;
	item2->key = 6;
	item3->key = 12;
	item4->key = 1;
	item5->key = 14;
	item6->key = 3;
	item7->key = 10;
	item8->key = 8;
	item9->key = 5;
	item10->key = 2;
	item11->key = 11;
	item12->key = 4;
	item13->key = 13;
	item14->key = 9;
	item15->key = 6;

	heap_push(heap, item1);
	heap_push(heap, item2);
	heap_push(heap, item3);
	heap_push(heap, item4);
	heap_push(heap, item5);
	heap_push(heap, item6);
	heap_push(heap, item7);
	heap_push(heap, item8);
	heap_push(heap, item9);
	heap_push(heap, item10);
	heap_push(heap, item11);
	heap_push(heap, item12);
	heap_push(heap, item13);
	heap_push(heap, item14);
	heap_push(heap, item15);
}

//堆测试
void heap_test()
{
	heap *heap = (struct heap *)malloc(sizeof(heap));
	if (!heap)
	{	
		puts("malloc(sizeof(struct _heap)) failure");
		return;
	}

	heap_init(heap, 2);

	heap_test_data(heap);

	heap_item *temp;

	heap_foreach(heap, temp)
		printf("heap_item.key: %u \n", temp->key);

	puts("---------------------------------------------------------");

	for (int i = 1; i <= 15; ++i)
	{
		temp = heap_pop(heap);
		printf("heap_item.key: %u \n", temp->key);
	}
}

//栈测试用例
void stack_test()
{
	_stack *stack = stack_create(2);
	_student stu1;
	_student stu2;
	_student stu3;
	_student stu4;
	_student stu5;
	_student stu6;
	_student stu7;
	_student stu8;
	snprintf(stu1.name, 16, "张三");
	snprintf(stu2.name, 16, "李四");
	snprintf(stu3.name, 16, "王五");
	snprintf(stu4.name, 16, "孙六");
	snprintf(stu5.name, 16, "赵七");
	snprintf(stu6.name, 16, "王八");
	snprintf(stu7.name, 16, "关晓彤");
	snprintf(stu8.name, 16, "赵丽颖");
	stu1.key = 18;
	stu2.key = 17;
	stu3.key = 16;
	stu4.key = 15;
	stu5.key = 14;
	stu6.key = 13;
	stu7.key = 12;
	stu8.key = 11;

	stack_push(stack, &stu1);
	stack_push(stack, &stu2);
	stack_push(stack, &stu3);
	stack_push(stack, &stu4);
	stack_push(stack, &stu5);
	stack_push(stack, &stu6);
	stack_push(stack, &stu7);
	stack_push(stack, &stu8);

	_student *temp;
	stack_foreach(stack, temp)
	{
		printf("姓名：%s	年龄：%d \n", temp->name, temp->key);
	}

	puts("------------------------------");

	while (!stack_empty(stack))
	{
		temp = (_student *)stack_pop(stack);
		printf("姓名：%s	年龄：%d \n", temp->name, temp->key);
	}
}

void main()
{
	//heap_test();
	stack_test();
}
