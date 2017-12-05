compiler = gcc -std=gnu99
head = $(wildcard *.h)
src = $(wildcard *.c)
obj_name = $(src:%.c=%.o)
obj_dir = ../obj
obj = $(addprefix $(obj_dir)/, $(obj_name))
debug_flag = 
flag += -fPIC
#附加包含目录
flag += -I../common -I../c-stl

all: $(obj)

$(obj_dir)/%.o: %.c
	$(compiler) -c $< -o $@ $(flag) $(debug_flag)
