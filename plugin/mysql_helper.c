#include <assert.h>
#include "mysql_helper.h"
#include "../bin/include/common.h"
#include "../bin/include/algorithm.h"

//读取字符串
const char *mysql_get_string(mysql_set *set, const char *field)
{
	//参数检查
	assert(set && field);
	if (!set || !field) return NULL;

	//遍历，查找对应值
	MYSQL_FIELD *pfield;
	int i = set->index;
	do
	{
		mysql_field_seek(set->result, i);
		pfield = mysql_fetch_field(set->result);
		if (pfield && (0 == strcmp(pfield->name, field)))
		{
			set->index = i;
			return set->row[i];
		}

		i = ++i % set->field_count;
	}while(i != set->index);

	return NULL;
}

//关闭对象
void mysql_set_close(mysql_set *set)
{
	if (set)
	{
		mysql_free_result(set->result);
		safe_free(set);
	}
}

//创建一个mysql连接并连接服务器
MYSQL *mysql_create(const char *host, int port, const char *user, const char *pwd, 
				    const char *db, int timeout)
{
	//初始化一个实例
	MYSQL *mysql = mysql_init(NULL);
	if (!mysql) return NULL;

	//设置重连
	int arg = 1;
	if (0 != mysql_options(mysql, MYSQL_OPT_RECONNECT, &arg)) 
	{
		mysql_close(mysql);
		return NULL;
	}

	//设置超时时间
	mysql->options.read_timeout = timeout;
	mysql->options.write_timeout = timeout;
	mysql->options.connect_timeout = timeout;

	//连接服务器
	if (NULL == mysql_real_connect(mysql, host, user, pwd, db, port, NULL, 0))
	{
		mysql_close(mysql);
		return NULL;
	}

	//设置字符集
	char sql[32];
	snprintf(sql, 32, "set names utf8");
	if (mysql_real_query(mysql, sql, strlen(sql)))
	{
		mysql_close(mysql);
		return NULL;
	}

	return mysql;
}

//执行写库操作
ulong mysql_dml(MYSQL * mysql, const char *sql)
{
	//参数检查
	assert(mysql && sql);
	if (!mysql || !sql) return PARAM_ERROR;

	if (0 != mysql_real_query(mysql, sql, strlen(sql)))
		return FAILURE;

	return mysql_affected_rows(mysql);
}

//执行读库操作
mysql_set *mysql_execute(MYSQL * mysql, const char *sql)
{
	//参数检查
	assert(mysql && sql);
	if (!mysql || !sql) return PARAM_ERROR;

	//执行查询
	if (0 != mysql_real_query(mysql, sql, strlen(sql)))
		return NULL;

	//取出结果集
	MYSQL_RES *my_res = mysql_store_result(mysql);
	if (!my_res) return NULL;

	//申请内存
	mysql_set *set = malloc(sizeof(mysql_set));	
	if (!set)
	{
		mysql_free_result(my_res);
		return NULL;
	}

	//存储参数
	mysql_data_seek(my_res, 0);
	set->result = my_res;
	set->row = mysql_fetch_row(my_res);
	set->field_count = mysql_num_fields(my_res);
	set->index = 0;

	return set;
}
