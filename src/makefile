head = $(wildcard *.h)
src = $(wildcard *.c)
obj_name = $(src:%.c=%.o)
obj_dir = ../obj
obj = $(addprefix $(obj_dir)/, $(obj_name))
debug_flag = 
flag += -fPIC
flag += -std=gnu99
flag += -I../common -I../epollet -I../uthread -I../c-stl

all: $(obj) copy

copy: $(head)
	cp $^ ../bin/include

#%.o: %.c # %.d
$(obj_dir)/%.o: %.c
	gcc -c $< -o $@ $(flag) $(debug_flag)

#%.d: %.c
#	gcc -MM $< $(flag) > $@
