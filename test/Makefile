targets = queue_test test-array test-uthread
compiler = gcc -std=gnu99

define = 
ifneq ($(define), )
define_flag = $(define)
else
define_flag = HELLO_WORLD
endif

all: queue_test test-array test-uthread

queue_test: queue_test.c  ../c-stl/queue.c ../common/algorithm.h
	$(compiler) queue_test.c ../c-stl/queue.c ../common/algorithm.h -o test-queue -g

list_test: test-list.c
	$(compiler) -o $@ $^ -L../bin/lib -lhlnet -g

array_test: test-array.c ../c-stl/array.c
	$(compiler) test-array.c ../c-stl/array.c -o test-array -g

test-uthread: test-uthread.c ../uthread/uthread.c cpu_mem_stat.c ../c-stl/array.c \
				../common/algorithm.h
	$(compiler) -g -o test-uthread test-uthread.c ../uthread/uthread.c cpu_mem_stat.c ../c-stl/array.c \
				../common/algorithm.h

unit_test: unit_test.c
	$(compiler) -o $@ $^ -L../bin/lib -lhlnet -g -D $(define_flag)

clean:
	rm -f $(targets) 
