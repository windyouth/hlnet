targets = queue_test test-array coroutine_test
compiler = gcc -std=gnu99

define = 
ifneq ($(define), )
define_flag = $(define)
else
define_flag = HELLO_WORLD
endif

all: queue_test array_test coroutine_test

queue_test: queue_test.c  ../c-stl/queue.c ../common/algorithm.h
	$(compiler) queue_test.c ../c-stl/queue.c ../common/algorithm.h -o test-queue -g

list_test: list_test.c
	$(compiler) -o $@ $^ -L../bin/lib -lhlnet -g

array_test: array_test.c ../c-stl/array.c
	$(compiler) test-array.c ../c-stl/array.c -o test-array -g

coroutine_test: coroutine_test.c ../coroutine/coroutine.c 
	$(compiler) -g -o $@ $^ -Wall

unit_test: unit_test.c
	$(compiler) -o $@ $^ -L../bin/lib -lhlnet -g -D $(define_flag)

main: main.c define.h
	$(compiler) -o $@ $^ -L../bin/lib -lhlnet -g -D $(define_flag)

client: client.c ../common/algorithm.c define.h
	$(compiler) -o $@ $^ -g -D $(define_flag)

epollet_test: epollet_test.c
	$(compiler) -o $@ $< -g -D $(define_flag)

myepoll: myepoll.c
	$(compiler) -o $@ $< -g -D $(define_flag)

clean:
	rm -f $(targets) 
