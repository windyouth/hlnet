compiler = gcc -std=gnu99
head = $(wildcard *.h)
src = $(wildcard *.c)
obj_name = $(src:%.c=%.o)
obj_dir = ../obj
obj = $(addprefix $(obj_dir)/, $(obj_name))
debug_flag = 
flag += -fPIC

all: $(obj) copy

copy: $(head)
	cp $^ ../hlnet/include

$(obj_dir)/%.o: %.c
	$(compiler) -c $< -o $@ $(flag) $(debug_flag)

