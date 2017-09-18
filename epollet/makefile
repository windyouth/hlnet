target = ../lib/libepollet.a
head = $(wildcard *.h)
src = $(wildcard *.c)
obj_name = $(src:%.c=%.o)
obj_dir = ../obj
obj = $(addprefix $(obj_dir)/, $(obj_name))
flag += -fPIC
flag += -std=gnu99
#附加包含目录
flag += -I../common -I../c-stl

all: $(target)

$(target): $(obj)
	ar rcs $@ $^

$(obj_dir)/%.o: %.c
	gcc -c $< -o $@ $(flag)

clean:
	rm -f $(target)
