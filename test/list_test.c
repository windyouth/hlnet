#include "../c-stl/list.h"
#include<stdio.h>

/*
typedef struct sutd
{
    as_list_item;
    int id;
    char *name;
}stud_t;

void old_test()
{
    list list1;
    int ret = 0;
    list_init(&list1);

    stud_t stud1;
    stud1.id = 0;
    stud1.name = "0-stu";

    stud_t stud2;
    stud2.id = 1;
    stud2.name = "1-stu";

    stud_t stud3;
    stud3.id = 2;
    stud3.name = "2-stu";

    stud_t stud4;
    stud4.id = 3;
    stud4.name = "3-stu";

    ret = list_push_back(&list1, &stud1);
    if(ret == OP_LIST_FAILURE) return -1;

    ret = list_push_back(&list1, &stud2);
    if(ret == OP_LIST_FAILURE) return -1;

    ret = list_push_back(&list1, &stud3);
    if(ret == OP_LIST_FAILURE) return -1;

    //ret = list_push_back(&list, stud4);
    //if(ret == OP_LIST_FAILUREURE) return -1;  

    list_insert_before(&list1, 2, &stud4);

    list_item *temp = NULL;
    list_foreach(&list1, temp)
    {
		stud_t *p = (stud_t*)temp;
        printf("id=%d, name=%s\n",p->id, p->name);
    }

    list_free_shalow(&list1);
}
*/

typedef struct _student
{
    as_list_item;
    int     id;
    char    name[32];
}student;

void deal_data(list_item *item)
{
    student *stu = (student *)item;
    printf("学号: %d, 姓名：%s \n", stu->id, stu->name);
}

void normal_test()
{
    list *list = list_create();
    if (!list)
    {
        puts("list_create failure");
        return;
    }

    student *stu = (student *)malloc(sizeof(student));
    stu->id = 1;
    snprintf(stu->name, 32, "张三");
    list_push_back(list, stu);

    stu = (student *)malloc(sizeof(student));
    stu->id = 2;
    snprintf(stu->name, 32, "李四");
    list_push_back(list, stu);

    student *temp = stu = (student *)malloc(sizeof(student));
    stu->id = 3;
    snprintf(stu->name, 32, "王五");
    list_push_front(list, stu);

    stu = (student *)malloc(sizeof(student));
    stu->id = 4;
    snprintf(stu->name, 32, "孙六");
    list_push_front(list, stu);

    stu = (student *)malloc(sizeof(student));
    stu->id = 5;
    snprintf(stu->name, 32, "赵七");
    list_push_front(list, stu);

    printf("元素数量：%ld \n", list_size(list));
    list_foreach(list, deal_data);

    puts("--------------------------");
    list_pop_front(list);
    puts("删除头元素后");
    printf("元素数量：%ld \n", list_size(list));
    list_foreach(list, deal_data);

    puts("--------------------------");
    list_pop_back(list);
    puts("删除尾元素后");
    printf("元素数量：%ld \n", list_size(list));
    list_foreach(list, deal_data);

    puts("--------------------------");
    list_erase(list, temp);
    puts("删除中间元素后");
    printf("元素数量：%ld \n", list_size(list));
    list_foreach(list, deal_data);

    list_free(list);
}

void press_test()
{
    list *left = list_create();
    list *right = list_create();
    if (!left || !right)
    {
        puts("list_create failure");
        return;
    }

    student *stu = 0;
    for (int i = 0; i < 25000; i++)
    {
        stu = (student *)malloc(sizeof(student));
        stu->id = i + 1;
        snprintf(stu->name, 32, "李四");
        list_push_back(left, stu);
    }

    int count = list_size(left);
    for (int i = 0; i < count; i++)
    {
        list_push_front(right, list_pop_front(left));
    }

    count = list_size(right);
    for (int i = 0; i < count; i++)
    {
        list_push_back(left, list_pop_back(right));
    }
    
    list_foreach(left, deal_data);
    printf("左链表元素数量：%ld \n", list_size(left));
    printf("右链表元素数量：%ld \n", list_size(right));

    list_free(left);
    list_free(right);
}

void main(int argc, char **argv)
{
    press_test();
}
