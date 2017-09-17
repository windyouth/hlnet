#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "queue.h"

#define			QUEUE_INIT_CAPACITY			5120			//队列初始容积

//初始化队列
int queue_init(queue *queue, uint32_t capacity)
{
	//参数检查
	assert(queue);
	if (!queue) return OP_QUEUE_PARAM_ERROR;

	//申请内存
	int len = capacity * sizeof(void **);
	queue->table = (void **)malloc(len);
	assert(queue->table != NULL);
	if (!queue->table) return OP_QUEUE_MEM_ERROR;

	//初始化值
	queue->size = 0;
	queue->capacity = capacity;
	queue->head = 0;
	queue->tail = 0;

	memset(queue->table, 0, len);

	return OP_QUEUE_SUCCESS;
}

//在末尾加入一个元素
int queue_push(queue *queue, void *item)
{
	//参数检查
	assert(queue != NULL && item != NULL);
	if (NULL == queue || NULL == item)
	{
		return OP_QUEUE_PARAM_ERROR;
	}

	//如果满了
	if (queue->head == (queue->tail + 1) % queue->capacity)
	{
		int new_capacity = queue->capacity + queue->capacity / 2;
		void** newtable = (void**)realloc(queue->table, new_capacity * sizeof(void**));
		if (newtable == NULL)
		{
			return OP_QUEUE_MEM_ERROR;
		}
		else
		{
			queue->table = newtable;

			//head以下的数据全部后移到底部
			if (queue->head > queue->tail)
			{
				int i = queue->capacity, index = new_capacity;
				while (i > queue->head)
				{
					queue->table[--index] = queue->table[--i];
				}
				queue->head = index;
			}

			queue->capacity = new_capacity;
		}//end else
	}//end if

	queue->table[queue->tail] = (void*)item;
	queue->tail = (queue->tail + 1) % queue->capacity;
	queue->size++;

	return OP_QUEUE_SUCCESS;
}

//末尾加入一个元素，队列满后将无法继续插入。
int queue_push_fixed(queue* queue, void* item)
{
	//参数检查
	assert(queue != NULL && item != NULL);
	if (NULL == queue || NULL == item)
	{
		return OP_QUEUE_PARAM_ERROR;
	}

	//如果满了
	if (queue->head == (queue->tail + 1) % queue->capacity)
	{
		return OP_QUEUE_FULL_ERROR;
	}

	queue->table[queue->tail] = (void*)item;
	queue->tail = (queue->tail + 1) % queue->capacity;
	queue->size++;

	return OP_QUEUE_SUCCESS;
}

//在头部弹出一个元素
void* queue_pop(queue *queue)
{
	//参数检查
	assert(queue != NULL);
	if (NULL == queue)
	{
		return NULL;
	}

	void* item = queue->table[queue->head];
	queue->table[queue->head] = NULL;
	queue->head = (queue->head + 1) % queue->capacity;
	queue->size--;

	return item;
}

//释放队列（浅度）
void queue_free_shallow(queue* queue)
{
	assert(queue != NULL);
	free(queue->table);
	memset(queue, 0, sizeof(*queue));
}

//释放队列（深度）
void queue_free_deep(queue* queue)
{
	assert(queue != NULL);
	
	for (int i = queue->head; i != queue->tail; i = (i + 1) % queue->capacity)
	{
		if (queue->table[i])
		{
			free(queue->table[i]);
		}
	}

	free(queue->table);
	memset(queue, 0, sizeof(*queue));
}

