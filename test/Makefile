targets = queue_test test-array test-uthread
compiler = gcc -std=gnu99

define = 
ifneq ($(define), )
define_flag = $(define)
else
define_flag = HELLO_WORLD
endif

all: queue_test array_test uthread_test

queue_test: queue_test.c  ../c-stl/queue.c ../common/algorithm.h
	$(compiler) queue_test.c ../c-stl/queue.c ../common/algorithm.h -o test-queue -g

list_test: list_test.c
	$(compiler) -o $@ $^ -L../bin/lib -lhlnet -g

array_test: array_test.c ../c-stl/array.c
	$(compiler) test-array.c ../c-stl/array.c -o test-array -g

uthread_test: uthread_test.c ../uthread/uthread.c ../c-stl/array.c \
				../common/algorithm.h
	$(compiler) -g -o $@ $^

unit_test: unit_test.c
	$(compiler) -o $@ $^ -L../bin/lib -lhlnet -g -D $(define_flag)

main: main.c
	$(compiler) -o $@ $^ -L../bin/lib -lhlnet -g -D $(define_flag)

client: client.c
	$(compiler) -o $@ $^ -g -D $(define_flag)

clean:
	rm -f $(targets) 
