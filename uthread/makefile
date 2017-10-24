src = $(wildcard *.c)
obj_name = $(src:%.c=%.o)
obj_dir = ../obj
obj = $(addprefix $(obj_dir)/, $(obj_name))
flag += -fPIC
flag += -std=gnu99

all: $(obj)

# 编绎静态库
#libuthread.a: $(obj)
#	ar rcs $@ $^

$(obj_dir)/%.o: %.c
	gcc -c $< -o $@ $(flag)
