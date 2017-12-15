#ifndef _MYSQL_HELPER_H_ 
#define _MYSQL_HELPER_H_

#include <mysql/mysql.h>
#include <sys/types.h>

//查询结果集
typedef struct _mysql_result
{
	MYSQL_RES			*result;
	MYSQL_ROW			row;
	uint				field_count;
	uint 				index;					//当前读到的索引值
}mysql_result;

//是否结束
#define mysql_eof(query)				(query->row == NULL)
//下一行
#define mysql_next_row(query)			(query->row = mysql_fetch_row(query->result))

//读取其他形式的值
#define mysql_get_float(query, filed) 	(atof(mysql_get_string(query, field)))
#define mysql_get_int(query, filed) 	(atoi(mysql_get_string(query, field)))
#define mysql_get_int64(query, filed) 	(atol(mysql_get_string(query, field)))

//写数据库操作
#define mysql_write(mysql, sql)			mysql_real_query(mysql, sql, strlen(sql))

//读取字符串
const char *mysql_get_string(mysql_result *query, const char *field);
//关闭结果集对象
void mysql_result_close(mysql_result *query);

//创建一个mysql连接并连接服务器
MYSQL *mysql_create(const char *host, int port, const char *user, const char *pwd, 
	    			const char *db, int timeout);
//执行写库操作
//int mysql_write(MYSQL * mysql, const char *sql);
//执行读库操作
mysql_result *mysql_read(MYSQL * mysql, const char *sql);

#endif //_MYSQL_HELPER_H_
