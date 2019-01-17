#include "list.h"
#include <assert.h>
#include <string.h>

//创建一个链表
list *list_create()
{
    list *list = (list *)malloc(sizeof(struct _list));
    if (!list) return NULL;

    memset(list, 0, sizeof(struct _list));
    list->inited = 1;

    return list;
}

//销毁链表(成员在堆上时)
void list_free_deep(list *list)
{
    assert(list);
    if (!list) return;

    list_item *temp = list->head;
    list_item *next;
    do
    {
        next = temp->next;
        free((void *)temp);
        temp = next;
    } while(temp != list->head);
    
    free((void *)list);
}

//从该元素前面插入
int list_insert_before(list *list, list_item *pos, list_item *item)
{
    return OP_LIST_SUCCESS;
}

//从该元素后面插入
int list_insert_after(list *list, list_item *pos, list_item *item)
{
    return OP_LIST_SUCCESS;
}

//从链表头部插入
int list_push_front(list *list, void *item)
{
    return OP_LIST_SUCCESS;
}

//从链表尾部插入
int list_push_back(list *list, void *item)
{
    return OP_LIST_SUCCESS;
}

//删除一个元素
list_item *list_erase(list *list, list_item *item)
{
    return 0;
}

//删除第一个元素
list_item *list_pop_front(list *list)
{
    return 0;
}

//删除最后一个元素
list_item *list_pop_back(list *list)
{
    return 0;
}


