#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "../common/internal.h"
#include "../common/buffer.h"
#include "../c-stl/list.h"

//重置客户端结构
#define client_reset(cli) do							\
{														\
	cli->status = 0;					                \
	cli->need = 0;		                            	\
	cli->fd = INVALID_SOCKET;  							\
	cli->is_safe = NO;									\
	cli->is_ready = NO;									\
	buffer_reset(cli->in);								\
	buffer_reset(cli->out);								\
}while (0)

//释放客户端结构
#define client_free(cli) do								\
{														\
	buffer_free(cli->in);									\
	buffer_free(cli->out);								\
	safe_free(cli);										\
}while (0)

//客户端信息
typedef struct _client
{
	as_list_item;
	ulong			    alive_time;		//上次活跃时间
	uint			    id;				//套接字对应的内部使用索引号
	int					fd;				//socket文件描述符
	uint			    ip;				//客户端IP地址
	int 				parent;			//所属的父文件描述符
	buffer				*in;			//读缓冲区
	buffer				*out;			//写缓冲区
	ushort	    		need;			//未读完的字节数
    uchar               status;         //当前状态
	uchar				is_safe;		//是否通过安全认证
	uchar				is_ready;		//是否处在就绪链表中
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
client_t *get_client(uint id);

#endif //_CLIENT_H_
