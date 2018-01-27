#include <stdio.h>
#include <stdlib.h>
#include "../hlnet/include/heap.h"

void main()
{
	heap *heap = (struct _heap *)malloc(sizeof(struct _heap));
	if (!heap) puts("malloc(sizeof(struct _heap)) failure");

	heap_item item1;
	heap_item item2;
	heap_item item3;
	heap_item item4;
	heap_item item5;
	heap_item item6;
	heap_item item7;
	heap_item item8;

	item1.key = 7;
	item2.key = 5;
	item3.key = 2;
	item4.key = 1;
	item5.key = 8;
	item6.key = 3;
	item7.key = 6;
	item8.key = 4;

	heap_init(heap, 16);
	heap_item *temp;

	heap_push(heap, &item1);
	puts("-----------after heap_push(heap, &item1)-------------");
	heap_foreach(heap, temp)
		printf("heap_item.key: %u \n", temp->key);

	heap_push(heap, &item2);
	puts("-----------after heap_push(heap, &item2)-------------");
	heap_foreach(heap, temp)
		printf("heap_item.key: %u \n", temp->key);

	heap_push(heap, &item3);
	puts("-----------after heap_push(heap, &item3)-------------");
	heap_foreach(heap, temp)
		printf("heap_item.key: %u \n", temp->key);

	heap_push(heap, &item4);
	puts("-----------after heap_push(heap, &item4)-------------");
	heap_foreach(heap, temp)
		printf("heap_item.key: %u \n", temp->key);

	heap_push(heap, &item5);
	puts("-----------after heap_push(heap, &item5)-------------");
	heap_foreach(heap, temp)
		printf("heap_item.key: %u \n", temp->key);

	heap_push(heap, &item6);
	puts("-----------after heap_push(heap, &item6)-------------");
	heap_foreach(heap, temp)
		printf("heap_item.key: %u \n", temp->key);

	heap_push(heap, &item7);
	puts("-----------after heap_push(heap, &item7)-------------");
	heap_foreach(heap, temp)
		printf("heap_item.key: %u \n", temp->key);

	heap_push(heap, &item8);
	puts("-----------after heap_push(heap, &item8)-------------");
	heap_foreach(heap, temp)
		printf("heap_item.key: %u \n", temp->key);
}
