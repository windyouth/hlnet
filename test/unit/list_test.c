#include "../c-stl/list.h"
#include<stdio.h>

typedef struct _student
{
    as_list_item;
    int     id;
    char    name[32];
}student;

void deal_data(list_item *item, void *arg)
{
    student *stu = (student *)item;
    printf("学号: %d, 姓名：%s \n", stu->id, stu->name);
    //list_erase((list *)arg, item);
    //list_pop_front((list *)arg);
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

    student *temp1 = stu = (student *)malloc(sizeof(student));
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

    //_list_foreach(list, deal_data, 0);
    list_item *temp, *item;
    long index;
    list_foreach(list, index, item, temp)
    {
        stu = (student *)item;
        printf("学号: %d, 姓名：%s \n", stu->id, stu->name);
    }
    printf("元素数量：%ld \n", list_size(list));

    puts("--------------------------");
    //list_pop_front(list);
    puts("删除头元素后");
    //_list_foreach(list, deal_data, 0);
    list_item *item_head = list->head;
    list_foreach(list, index, item, temp)
    {
        if (item == item_head)
        {
            list_erase(list, item);
            continue;
        }
        stu = (student *)item;
        printf("学号: %d, 姓名：%s \n", stu->id, stu->name);
    }
    printf("元素数量：%ld \n", list_size(list));

    puts("--------------------------");
    //list_pop_back(list);
    puts("删除尾元素后");
    //_list_foreach(list, deal_data, 0);
    list_foreach(list, index, item, temp)
    {
        if (item == list->tail)
        {
            list_erase(list, item);
            continue;
        }
        stu = (student *)item;
        printf("学号: %d, 姓名：%s \n", stu->id, stu->name);
    }
    printf("元素数量：%ld \n", list_size(list));

    puts("--------------------------");
    //list_erase(list, temp1);
    puts("删除中间元素后");
    //_list_foreach(list, deal_data, 0);
    list_foreach(list, index, item, temp)
    {
        if (item == temp1)
        {
            list_erase(list, item);
            continue;
        }
        stu = (student *)item;
        printf("学号: %d, 姓名：%s \n", stu->id, stu->name);
    }
    printf("元素数量：%ld \n", list_size(list));

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
    
    //list_foreach(left, deal_data, left);
    list_item *temp, *item;
    long index;
    list_foreach(left, index, item, temp)
    {
        stu = (student *)item;
        printf("学号: %d, 姓名：%s \n", stu->id, stu->name);
    }

    printf("左链表元素数量：%ld \n", list_size(left));
    printf("右链表元素数量：%ld \n", list_size(right));

    list_free(left);
    list_free(right);
}

void main(int argc, char **argv)
{
    //press_test();
    normal_test();
}
