target = bin/lib/hlnet.so

flag += -Llib 
flag += -lmain -lcstl -luthread -lepollet
# -fPIC 告诉编绎器产生与位置无关代码，为共享库.so所必须。 
flag += -fPIC -shared

epollet_dir = ./epollet
uthread_dir = ./uthread
cstl_dir = ./c-stl
main_dir = ./src

.PHONY: main cstl uthread epollet

all: $(target)

$(target): main cstl uthread epollet
	gcc -o $@ $(flag)

main:
	make -C $(main_dir)

uthread:
	make -C $(uthread_dir)

cstl:
	make -C $(cstl_dir)

epollet:
	make -C $(epollet_dir)

clean: 
	rm -f lib/* obj/* bin/include/* bin/lib/*
	make -C $(main_dir) clean
	make -C $(uthread_dir) clean
	make -C $(cstl_dir) clean
	make -C $(epollet_dir) clean
