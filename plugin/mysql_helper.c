#include <assert.h>
#include "mysql_helper.h"
#include "../bin/include/common.h"
#include "../bin/include/algorithm.h"



//关闭对象
void mysql_query_close(mysql_query *query)
{
	if (query)
	{
		mysql_free_result(query->result);
		safe_free(query);
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

	return mysql;
}

//执行写库操作
int mysql_write(MYSQL * mysql, const char *sql)
{
	//参数检查
	assert(mysql && sql);
	if (!mysql || !sql) return PARAM_ERROR;

	if (0 != mysql_real_query(mysql, sql, strlen(sql)))
		return FAILURE;

	return (int)mysql_affected_rows(mysql);
}

//执行读库操作
mysql_query *mysql_read(MYSQL * mysql, const char *sql)
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
	mysql_query *query = malloc(sizeof(mysql_query));	
	if (!query)
	{
		mysql_free_result(my_res);
		return NULL;
	}

	//存储参数
	mysql_data_seek(my_res, 0);
	query->result = my_res;
	query->row = mysql_fetch_row(my_res);
	query->row_count = mysql_num_rows(my_res);
	query->field_count = mysql_num_fields(my_res);

	return query;
}
