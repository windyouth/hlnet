targets = queue_test test-array test-uthread
compiler = gcc -std=gnu99
all: queue_test test-array test-uthread

queue_test: queue_test.c  ../c-stl/queue.c ../common/algorithm.h
	$(compiler) queue_test.c ../c-stl/queue.c ../common/algorithm.h -o test-queue -g

test-array: test-array.c ../c-stl/array.c
	$(compiler) test-array.c ../c-stl/array.c -o test-array -g

test-uthread: test-uthread.c ../uthread/uthread.c cpu_mem_stat.c ../c-stl/array.c \
				../common/algorithm.h
	$(compiler) -g -o test-uthread test-uthread.c ../uthread/uthread.c cpu_mem_stat.c ../c-stl/array.c \
				../common/algorithm.h

unit_test: unit_test.c
	$(compiler) -o $@ $^ -L../bin/lib -lhlnet -g

buffer_test: buffer_test.c
	$(compiler) -o $@ $< -L../bin/lib -lhlnet -g

clean:
	rm -f $(targets) 
