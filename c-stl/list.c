#include "list.h"
#include <assert.h>
#include <string.h>

//创建一个链表
list *list_create()
{
    list *list = (struct _list *)malloc(sizeof(struct _list));
    if (!list) return NULL;

    memset(list, 0, sizeof(struct _list));
    list->inited = 1;

    return list;
}

//销毁链表(成员在堆上时)
void list_free(list *list)
{
    assert(list);
    if (!list) return;

    if (list_empty(list)) goto end;

    list_item *temp = list->head;
    list_item *next;
    do
    {
        next = temp->next;
        //释放元素内存
        free((void *)temp);
        temp = next;
    } while(temp != list->head);
    
end:
    free((void *)list);
}

//从该元素前面插入
int list_insert_before(list *list, list_item *pos, list_item *item)
{
    //参数校验
    assert(list && pos && item);
    if (!list || !pos || !item) return OP_LIST_PARAM_ERROR;

    //与前一个元素连接
    item->prev = pos->prev;
    pos->prev->next = item;
    //与pos元素连接
    item->next = pos;
    pos->prev = item;

    //计数加一
    list->size++;

    //如果是从头节点插入，偏移头指针
    if (list->head == pos)
        list->head = item;

    return OP_LIST_SUCCESS;
}

//从该元素后面插入
int list_insert_after(list *list, list_item *pos, list_item *item)
{
    //参数校验
    assert(list && pos && item);
    if (!list || !pos || !item) return OP_LIST_PARAM_ERROR;

    //与后一个元素连接
    item->next = pos->next;
    pos->next->prev = item;
    //与pos元素连接
    item->prev = pos;
    pos->next = item;

    //计数加一
    list->size++;

    //如果是从尾节点插入，偏移尾指针
    if (list->tail == pos)
        list->tail = item;

    return OP_LIST_SUCCESS;
}

//插入第一个元素
#define list_insert_first(list, item) do    \
{                                           \
    list->head = item;                      \
    list->tail = item;                      \
    list->size = 1;                         \
    /*单元素时，前向和后向指针均指向自己*/  \
    item->prev = item;                      \
    item->next = item;                      \
} while(0)

//从链表头部插入
int list_push_front(list *list, list_item *item)
{
    if (list_empty(list))
    {
        list_insert_first(list, item);
        return OP_LIST_SUCCESS;
    }

    return list_insert_before(list, list->head, item);
}

//从链表尾部插入
int list_push_back(list *list, list_item *item)
{
    if (list_empty(list))
    {
        list_insert_first(list, item);
        return OP_LIST_SUCCESS;
    }

    return list_insert_after(list, list->tail, item);
}

//删除一个元素
list_item *list_erase(list *list, list_item *item)
{
    //参数校验
    assert(list && item);
    if (!list || !item) return OP_LIST_PARAM_ERROR;

    //如果只有一个元素
    if (list_only_one(list))
    {
        list->head = 0;
        list->tail = 0;

        goto end;
    }

    //删除元素
    item->prev->next = item->next;
    item->next->prev = item->prev;
    
    //头指针判断
    if (list->head == item)
    {
        list->head = item->next;
    }
    //尾指针判断
    else if (list->tail == item)
    {
        list->tail = item->prev;
    }

end:
    //计数减一
    list->size--;
    //指针置零
    item->prev = 0;
    item->next = 0;
    //返回删除的元素
    return item;
}

//遍历链表
void _list_foreach(list *list, deal_func deal, void *arg)
{
    //参数校验
    assert(list && deal);
    if (!list || !deal) return;

    //空链表，直接返回
    if (list_empty(list)) return;

    //遍历各元素
    list_item *item = list->head;
    list_item *next;
    do
    {
        next = item->next;
        //执行操作
        deal(item, arg);
        item = next;
    } 
    while(item != list->head);
}
