target = ../lib/libmain.a
head = $(wildcard *.h)
src = $(wildcard *.c)
obj_name = $(src:%.c=%.o)
obj_dir = ../obj
obj = $(addprefix $(obj_dir)/, $(obj_name))
flag += -fPIC
flag += -std=gnu99
flag += -I../common -I../epollet -I../uthread -I../c-stl

all: $(target) copy

copy: $(head)
	cp $^ ../bin/include

$(target): $(obj)
	ar rcs $@ $^

#%.o: %.c # %.d
$(obj_dir)/%.o: %.c
	gcc -c $< -o $@ $(flag)

#%.d: %.c
#	gcc -MM $< $(flag) > $@

clean:
	rm -f $(target)
