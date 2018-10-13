compiler = gcc -std=gnu99
head = coroutine.h
src = $(wildcard *.c)
obj_name = $(src:%.c=%.o)
obj_dir = ../obj
obj = $(addprefix $(obj_dir)/, $(obj_name))
flag += -fPIC
debug_flag = 

all: $(obj) copy

copy: $(head)
	cp $^ ../hlnet/include

# 编绎静态库
#libuthread.a: $(obj)
#	ar rcs $@ $^

$(obj_dir)/%.o: %.c %.h
	$(compiler) -c $< -o $@ $(flag) $(debug_flag)
