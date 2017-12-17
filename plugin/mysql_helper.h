#ifndef _MYSQL_HELPER_H_ 
#define _MYSQL_HELPER_H_

#include <mysql/mysql.h>
#include <sys/types.h>

//查询结果集
typedef struct _mysql_set
{
	MYSQL_RES			*result;
	MYSQL_ROW			row;
	uint				field_count;
	uint 				index;					//当前读到的索引值
}mysql_set;

//是否结束
#define mysql_eof(set)					(set->row == NULL)
//下一行
#define mysql_next_row(set)	do						\
{													\
	set->row = mysql_fetch_row(set->result);		\
	set->index = 0;									\
} while(0)

//读取其他形式的值
#define mysql_get_float(set, field) 	(atof(mysql_get_string(set, field)))
#define mysql_get_int(set, field) 		(atoi(mysql_get_string(set, field)))
#define mysql_get_int64(set, field) 	(atol(mysql_get_string(set, field)))

//读取字符串
const char *mysql_get_string(mysql_set *set, const char *field);
//关闭结果集对象
void mysql_set_close(mysql_set *set);

//创建一个mysql连接并连接服务器
MYSQL *mysql_create(const char *host, int port, const char *user, const char *pwd, 
	    			const char *db, int timeout);
//执行写库操作,没有查询动作
ulong mysql_dml(MYSQL * mysql, const char *sql);
//执行有查询动作的sql语句。
mysql_set *mysql_execute(MYSQL * mysql, const char *sql);

#endif //_MYSQL_HELPER_H_
