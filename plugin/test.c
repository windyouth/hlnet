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

	char *sql1 = "call add_user('wangba', '123456', 'wangba666', 'IBM大中华区', '13833338888')";
	char *sql2 = "insert ignore into user(account, password, secret_key, corporation, phone, create_time) values('tony001', '123456', 'tony666', '中国科学院', '13833338888', '2017-12-16 16:07:00')";
	char *sql3 = "select id, account, secret_key, corporation, phone from user";
	char *sql4 = "update user set corporation = '皮包公司甲' where account = 'zhangsan'";
	ulong res = mysql_dml(mysql, sql4);
	printf("res = %u, errmsg: %s \n", res, mysql_error(mysql));
	mysql_set *set = mysql_execute(mysql, sql3);
	if (!set)
	{
		printf("mysql_execute failure, errmsg: %s", mysql_error(mysql));
		mysql_close(mysql);
		exit(1);
	}

	while (!mysql_eof(set))
	{
		//printf("phone: %s \t", mysql_get_string(set, "phone"));
		printf("account: %s \t", mysql_get_string(set, "account"));
		printf("id: %s \t", mysql_get_string(set, "id"));
		printf("corporation: %s \t", mysql_get_string(set, "corporation"));
		printf("secret_key: %s \t", mysql_get_string(set, "secret_key"));
		puts("\n--------------------------------------------------------");
		mysql_next_row(set);
	}

}
