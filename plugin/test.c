#include <stdio.h>
#include "mysql_helper.h"

void main()
{
	MYSQL *mysql = mysql_create("localhost", 3306, "heluan", "heluanhl", "test", 1000);
	if (!mysql)
	{
		puts("mysql_create failure");
		exit(1);
	}

	ulong res = mysql_write(mysql, 
			"call add_user('wangba', '123456', 'wangba666', 'IBM大中华区', '13833338888')");

	printf("res = %u, errmsg: %s \n", res, mysql_error(mysql));
}
