/*
 * description: A circle queue template.
 * author: HeLuan
 * email: heluan123132@163.com
 */

#ifndef _QUEUE_H_
#define _QUEUE_H_

//队列结构体
typedef struct _queue
{
	int 			size;				//已使用大小
	int 			capacity;			//总容积
	int 			head;				//队列头
	int 			tail;				//队列尾
	void 			**table;			//队列数据区
}queue;

#define 		OP_QUEUE_SUCCESS 		(0)
#define 		OP_QUEUE_FAIL 			(-1)
#define 		OP_QUEUE_MEM_ERROR 		(-2)
#define 		OP_QUEUE_PARAM_ERROR	(-3)
#define 		OP_QUEUE_FULL_ERROR		(-4)

#define 		YES						1
#define			NO						0

#define queue_size(queue)	(queue)->size						//取得队列大小
#define queue_empty(queue)	(queue)->size > 0 ? YES : NO		//判断队列是否为空
#define queue_front(queue)	(queue)->table[head]				//返回队列第一个元素
#define queue_back(queue)	(queue)->table[tail]				//返回队列最后一个元素

//遍历队列
#define queue_foreach(queue, item)													\
			for(int i = (queue)->head; 												\
				(item) = (queue)->table[i], i != (queue)->tail; 					\
				i = ++i % (queue)->capacity, (item) = (queue)->table[i])

//初始化队列
int queue_init(queue *queue, uint32_t capacity);
//末尾加入一个元素，队列满后将自动扩展。
//本函数不保证线程安全，多线程请使用push_fixed。
int queue_push(queue *queue, void *item);
//末尾加入一个元素，队列满后将无法继续插入。
int queue_push_fixed(queue* queue, void* item);
//在头部弹出一个元素
void* queue_pop(queue *queue);
//释放队列（浅度）
void queue_free_shallow(queue* queue);
//释放队列（深度）
void queue_free_deep(queue* queue);

#endif //_QUEUE_H_
