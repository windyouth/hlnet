target = bin/lib/hlnet.so

flag += -Llib
flag += -lmain -lcommon -lcstl -luthread -lepollet
# -fPIC 告诉编绎器产生与位置无关代码，为共享库.so所必须。 
flag += -fPIC -shared

common_dir = ./common
epollet_dir = ./epollet
uthread_dir = ./uthread
cstl_dir = ./c-stl
main_dir = ./src

lib_dir = ./lib
obj_dir = ./obj
bin_dir = ./bin

#创建目录
#如果不想像clean一样写成目标依赖项的形式，就必须这样写。
ifneq ($(MAKECMDGOALS), clean)
$(shell mkdir -p $(lib_dir))
$(shell mkdir -p $(obj_dir)) 
$(shell mkdir -p $(bin_dir))
$(shell mkdir -p $(bin_dir)/include)
$(shell mkdir -p $(bin_dir)/lib)
endif

.PHONY: main common cstl uthread epollet

all: $(target)

$(target): main common cstl uthread epollet
	gcc -o $@ $(flag)

main:
	make -C $(main_dir)

common:
	make -C $(common_dir)

uthread:
	make -C $(uthread_dir)

cstl:
	make -C $(cstl_dir)

epollet:
	make -C $(epollet_dir)

clean: 
	rm -f lib/* obj/* bin/include/* bin/lib/*
	make -C $(main_dir) clean
	make -C $(common_dir) clean
	make -C $(uthread_dir) clean
	make -C $(cstl_dir) clean
	make -C $(epollet_dir) clean
	rm -rf $(lib_dir) $(obj_dir) $(bin_dir)
