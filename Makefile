compiler = gcc -std=gnu99
target = hlnet/lib/libhlnet.so
obj = $(wildcard obj/*.o)

#如果不加入调试信息，按O2级别优化代码。
debug_flag = -O2

flag += -lpthread
# -fPIC 告诉编绎器产生与位置无关代码，为共享库.so所必须。 
flag += -fPIC -shared
#加标志宏
define = _HLNET_

common_dir = ./common
epollet_dir = ./epollet
coroutine_dir = ./coroutine
cstl_dir = ./c-stl
main_dir = ./src
proto_dir = ./proto

lib_dir = ./lib
obj_dir = ./obj
bin_dir = ./hlnet

#创建目录
#如果不想像clean一样写成目标依赖项的形式，就必须这样写。
ifneq ($(MAKECMDGOALS), clean)
$(shell mkdir -p $(obj_dir)) 
$(shell mkdir -p $(bin_dir))
$(shell mkdir -p $(bin_dir)/include)
$(shell mkdir -p $(bin_dir)/lib)
endif

.PHONY: main common cstl coroutine epollet proto

all: $(target)

$(target): main common cstl coroutine epollet proto
	$(compiler) -o $@ $(obj) $(flag) $(debug_flag)

main:
	make -C $(main_dir) debug_flag=$(debug_flag) define_flag=$(define)

common:
	make -C $(common_dir) debug_flag=$(debug_flag) define_flag=$(define)

coroutine:
	make -C $(coroutine_dir) debug_flag=$(debug_flag)

cstl:
	make -C $(cstl_dir) debug_flag=$(debug_flag)

epollet:
	make -C $(epollet_dir) debug_flag=$(debug_flag)

proto:
	make -C $(proto_dir) debug_flag=$(debug_flag) define_flag=$(define)

clean: 
	#rm -f lib/* obj/* bin/include/* bin/lib/*
	#make -C $(epollet_dir) clean
	rm -rf $(obj_dir) $(bin_dir)

debug:
	@make debug_flag=-g
