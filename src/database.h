#ifndef _DATABASE_H_
#define _DATABASE_H_

#include "../common/buffer.h"


//数据库队列元素
typedef struct _dbqueue_item
{
	uint16_t			cmd;				//命令码，内部使用，勿需考虑内存对齐。	
	buffer				buf;				//数据缓冲区
}dbqueue_item_t, *pdbqueue_item_t;

//初始化数据库
int init_database();
//注册数据库消息
int reg_db_msg(uint16_t msg, dbmsg_hander func);
//启动数据库
int start_database();
//投递数据库消息
int post_db_msg(uint16_t msg, char *data, uint32_t len);

#endif //_DATABASE_H_
