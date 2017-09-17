#ifndef _DATABASE_H_
#define _DATABASE_H_

#include "../epollet/buffer.h"


//数据库队列元素
typedef struct _dbqueue_item
{
	uint16_t			cmd;				//命令码	
	buffer				buf;				//数据缓冲区
}dbqueue_item_t, *pdbqueue_item_t;


//启动数据库线程
int start_dbthread();
//注册数据库消息
int reg_db_msg(uint16_t msg, dbmsg_hander func);
//投递数据库消息
int post_db_msg(uint16_t msg, char *data, int len);

#endif //_DATABASE_H_
