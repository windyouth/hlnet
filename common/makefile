target = ../lib/libcommon.a
head = algorithm.h common.h
src = $(wildcard *.c)
obj_name = $(src:%.c=%.o)
obj_dir = ../obj
obj = $(addprefix $(obj_dir)/, $(obj_name))
flag += -fPIC
flag += -std=gnu99

all: $(target) copy

copy: $(head)
	cp $^ ../bin/include

$(target): $(obj)
	ar rcs $@ $^

$(obj_dir)/%.o: %.c
	gcc -c $< -o $@ $(flag)

clean:
	rm -f $(target)
