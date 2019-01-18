/*
 * 说明：双向循环链表
 * 作者：何峦
 * 日期：2019-01-17
 */

#ifndef _LIST_H_
#define _LIST_H_

//错误码定义
#define 		OP_LIST_SUCCESS 		(0)
#define 		OP_LIST_FAILURE 		(-1)
#define 		OP_LIST_MEM_ERROR 		(-2)
#define 		OP_LIST_PARAM_ERROR	    (-3)

//作为链表头，应用结构体头部包含。
#define     as_list_item    list_item __list_item       
#define list_size(list)	    (list)->size				//取得链表大小
#define list_empty(list)	((list)->size == 0)	        //判断链表是否为空
#define list_only_one(list) ((list)->size == 1)         //是否只有一个元素
#define list_front(list)	(list)->head			    //返回链表第一个元素
#define list_back(list)	    (list)->tail			    //返回链表最后一个元素

//遍历链表
#define list_foreach(list, item)                                               \
    for (int i = list_size(list), item = (list)->head, list_item *next = NULL;  \
         item != NULL && i >= 0 && (next = item->next);                         \
         i--, item = next)

//链表头
typedef struct _list_item
{
    struct _list_item   *prev;      //前向指针
    struct _list_item   *next;      //后向指针
}list_item;

//链表
typedef struct _list
{
    long            size;           //数量
    list_item       *head;          //头指针
    list_item       *tail;          //尾指针
    unsigned char   inited;         //是否初始化
}list;

//创建一个链表
list *list_create();
//销毁链表(带元素释放)
void list_free(list *list);
//销毁链表
#define list_free_shalow(list)     free(list)

//从该元素前面插入
int list_insert_before(list *list, list_item *pos, list_item *item);
//从该元素后面插入
int list_insert_after(list *list, list_item *pos, list_item *item);
//从链表头部插入
int list_push_front(list *list, list_item *item);
//从链表尾部插入
int list_push_back(list *list, list_item *item);

//删除一个元素
list_item *list_erase(list *list, list_item *item);
//删除第一个元素
#define list_pop_front(list)    list_erase(list, list->head)
//删除最后一个元素
#define list_pop_back(list)     list_erase(list, list->tail)

//遍历链表
typedef void (* deal_func)(list_item *item, void *arg);
void _list_foreach(list *list, deal_func deal, void *arg);

#endif
