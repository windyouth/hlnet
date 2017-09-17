#include <stdio.h>
#include <stdlib.h>
#include "../common/algorithm.h"
#include "../c-stl/queue.h"

struct student
{
	int id;
	char* name;
	int score;
};

int test_queue()
{
	queue students;
	zero(&students);
	queue_init(&students);

	struct student stu0;
	zero(&stu0);
	stu0.id = 1000;
	stu0.name = "张三";
	stu0.score = 99;

	struct student stu1;
	zero(&stu1);
	stu1.id = 1001;
	stu1.name = "李四";
	stu1.score = 95;
	
	struct student stu2;
	zero(&stu2);
	stu2.id = 1002;
	stu2.name = "王五";
	stu2.score = 88;

	for (int i = 0; i < 7; ++i)
	{
		queue_push(&students, &stu0);
	}
	struct student* stu_tmp1 = (struct student*)queue_pop(&students);
	struct student* stu_tmp2 = (struct student*)queue_pop(&students);
	for (int i = 0; i < 3; ++i)
	{
		queue_push(&students, &stu1);
	}

	void* item = NULL;
	struct student* stu = NULL;
	queue* stu_ptr = &students;
	queue_foreach(&students, item)
	{	
		stu = (struct student*)item;
		if (stu)
			printf("student info: id=%d, name=%s, score=%d\n", stu->id, stu->name, stu->score);
	}
	puts("---foreach end---");
	printf("student info: id=%d, name=%s, score=%d\n", stu_tmp1->id, stu_tmp1->name, stu_tmp1->score);
	printf("student info: id=%d, name=%s, score=%d\n", stu_tmp2->id, stu_tmp2->name, stu_tmp2->score);
	printf("head: %d, tail: %d, size: %d, capacity: %d\n", students.head, students.tail,
			students.size, students.capacity);
}

void main()
{
	test_queue();
}
