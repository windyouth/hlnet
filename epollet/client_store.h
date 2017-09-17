#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "common.h"
#include "buffer.h"
#include "../c-stl/list.h"

//重置客户端结构
#define client_reset(cli) do							\
{														\
	cli->status.part = READ_PART_HEAD;					\
	cli->status.need = sizeof(packet_head_t);			\
	buffer_reset(cli->in);								\
	buffer_reset(cli->out);								\
}while (0)

//释放客户端结构
#define client_free(cli) do								\
{														\
	safe_free(cli->in);									\
	safe_free(cli->out);								\
	safe_free(cli);										\
}while (0)

enum read_part_e
{
	READ_PART_HEAD 		= 1,			//正在读包头
	READ_PART_DATA 		= 2				//正在读数据
};

//读状态
typedef struct _read_status
{
	uint16_t			need;			//未读完的字节数
	uint8_t				part;			//正在读的部分，参见枚举：read_status_e
	uint8_t				reserve;		//保留字段 
}read_status_t, *pread_status;

//客户端信息
typedef struct _client
{
	as_list_item;
	uint32_t			id;				//套接字对应的内部使用索引号
	int					fd;				//socket文件描述符
	uint32_t			ip;				//客户端IP地址
	int 				parent;			//所属的父文件描述符
	read_status_t		status;			//读状态
	buffer				*in;			//读缓冲区
	buffer				*out;			//写缓冲区
}client_t, *pclient;

//初始化客户端结构体
int client_init(client_t *cli, int size);

//初始化客户端仓库
int client_store_init();
//释放客户端仓库
void client_store_free();

//取得一个客户端
client_t *extract_client();
//回收一个客户端
void recycle_client(client_t *cli);

//根据id查询客户端
client_t *get_client(uint32_t id);

#endif //_CLIENT_H_
