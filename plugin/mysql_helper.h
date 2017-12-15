#ifndef _MYSQL_HELPER_H_ 
#define _MYSQL_HELPER_H_

#include <mysql/mysql.h>
#include <stdint.h>

//查询结果集
typedef struct _mysql_query
{
	MYSQL_RES			*result;
	MYSQL_FIELD			*field;
	MYSQL_ROW			row;
	uint64_t			field_count;
	uint64_t			row_count;
}mysql_query;



//关闭对象
void mysql_query_close(mysql_query *query);

//创建一个mysql连接并连接服务器
MYSQL *mysql_create(const char *host, int port, const char *user, const char *pwd, 
	    			const char *db, int timeout);
//执行写库操作
int mysql_write(MYSQL * mysql, const char *sql);
//执行读库操作
mysql_query *mysql_read(MYSQL * mysql, const char *sql);

#endif //_MYSQL_HELPER_H_
