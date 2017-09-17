head = hlnet.h
src = $(wildcard *.c)
obj = $(src:%.c=%.o)
flag += -fPIC
flag += -I../common -I../epollet -I../uthread -I../c-stl

all: $(obj) copy

copy: $(head)
	cp $< ../bin/include

%.o: %.c # %.d
	gcc -c $< -o $@ $(flag)

#%.d: %.c
#	gcc -MM $< $(flag) > $@

clean:
	rm -f *.o
