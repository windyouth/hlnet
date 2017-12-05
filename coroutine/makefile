compiler = gcc -std=gnu99
src = $(wildcard *.c)
obj_name = $(src:%.c=%.o)
obj_dir = ../obj
obj = $(addprefix $(obj_dir)/, $(obj_name))
flag += -fPIC
debug_flag = 

all: $(obj)

# 编绎静态库
#libuthread.a: $(obj)
#	ar rcs $@ $^

$(obj_dir)/%.o: %.c
	$(compiler) -c $< -o $@ $(flag) $(debug_flag)
