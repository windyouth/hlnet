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

//链表头
typedef struct _list_item
{
    struct _list_item   *prev;      //前向指针
    struct _list_item   *next;      //后向指针
}list_item;

//链表
typedef struct _list
{
    size_t          size;           //数量
    list_item       *head;          //头指针
    list_item       *tail;          //尾指针
    unsigned char   inited;         //是否初始化
}list;

//创建一个链表
list *list_create();
//销毁链表
#define list_free(list)     free(list)
//销毁链表(带元素释放)
void list_free_deep(list *list);
//从该元素前面插入
int list_insert_before(list *list, list_item *pos, list_item *item);
//从该元素后面插入
int list_insert_after(list *list, list_item *pos, list_item *item);
//从链表头部插入
int list_push_front(list *list, void *item);
//从链表尾部插入
int list_push_back(list *list, void *item);
//删除一个元素
list_item *list_erase(list *list, list_item *item);
//删除第一个元素
list_item *list_pop_front(list *list);
//删除最后一个元素
list_item *list_pop_back(list *list);

#endif
