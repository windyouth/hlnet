compiler = gcc -std=gnu99
head = $(wildcard *.h)
src = $(wildcard *.c)
obj_name = $(src:%.c=%.o)
obj_dir = ../obj
obj = $(addprefix $(obj_dir)/, $(obj_name))
debug_flag = 
flag += -fPIC
flag += -I../common -I../epollet -I../uthread -I../c-stl

all: $(obj) copy

copy: $(head)
	cp $^ ../hlnet/include

#%.o: %.c # %.d
$(obj_dir)/%.o: %.c
	$(compiler) -c $< -o $@ $(flag) $(debug_flag)

#%.d: %.c
#	gcc -MM $< $(flag) > $@
