target = ../lib/libuthread.a
src = $(wildcard *.c)
obj_name = $(src:%.c=%.o)
obj_dir = ../obj
obj = $(addprefix $(obj_dir)/, $(obj_name))
flag += -fPIC

all: $(target)

$(target): $(obj)
	ar rcs $@ $^

$(obj_dir)/%.o: %.c
	gcc -c $< -o $@ $(flag)

clean:
	rm -f $(target)
