targets = queue_test test-array test-uthread
all: queue_test test-array test-uthread

queue_test: queue_test.c  ../c-stl/queue.c ../common/algorithm.h
	gcc queue_test.c ../c-stl/queue.c ../common/algorithm.h -o test-queue -g

test-array: test-array.c ../c-stl/array.c
	gcc test-array.c ../c-stl/array.c -o test-array -g

test-uthread: test-uthread.c ../uthread/uthread.c cpu_mem_stat.c ../c-stl/array.c \
				../common/algorithm.h
	gcc -g -o test-uthread test-uthread.c ../uthread/uthread.c cpu_mem_stat.c ../c-stl/array.c \
				../common/algorithm.h
clean:
	rm -f $(targets) 
