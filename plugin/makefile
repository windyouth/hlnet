compiler = gcc -std=gnu99

test: test.c mysql_helper.c mysql_helper.h
	$(compiler) -o $@ $^ -g `mysql_config --cflags --libs`
