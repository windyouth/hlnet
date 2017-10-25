head = $(wildcard *.h)
src = $(wildcard *.c)
obj_name = $(src:%.c=%.o)
obj_dir = ../obj
obj = $(addprefix $(obj_dir)/, $(obj_name))
debug_flag = 
flag += -fPIC
flag += -std=gnu99

all: $(obj) copy

copy: $(head)
	cp $^ ../bin/include

$(obj_dir)/%.o: %.c
	gcc -c $< -o $@ $(flag) $(debug_flag)

