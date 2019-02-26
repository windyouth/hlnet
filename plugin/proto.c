#include "proto.h"
#include "../c-stl/map.h"

#define				MAX_CMDDATA_LEN			65000			//数据体的最大长度

int 				g_user_tcp_fd;		    //监听的套接字ID(用户端)
int 				g_manage_tcp_fd;		//监听的套接字ID(管理端)

//正在读的部分
enum read_part_e
{
	READ_PART_HEAD 		= 1,			//正在读包头
	READ_PART_DATA 		= 2				//正在读数据
};

//监听用户端口
int listen_user_port(ushort port)
{
	//初始化网络消息映射器
	g_net_user_msg = (map *)malloc(sizeof(map));
	if (!g_net_user_msg) return MEM_ERROR;
	if (map_init(g_net_user_msg) != OP_MAP_SUCCESS) return MEM_ERROR;
}

//注册网络消息
int reg_net_msg(sock_type sock_type, uint16_t msg, tcpmsg_hander func)
{
	char *key = (char *)malloc(8);
	bzero(key, 8);
	sprintf(key, "%u", msg);

	map *dst_map = NULL;

	switch (sock_type)
	{
		case socktype_user:
			{
				dst_map = g_net_user_msg;
			}
			break;
		case socktype_manage:
			{
				dst_map = g_net_manage_msg;
			}
			break;
		default:
			return PARAM_ERROR;
	}

	msg_func_item *item = (msg_func_item *)malloc(sizeof(msg_func_item));
	if (!item) return MEM_ERROR;

	item->msg_func = func;
	if (map_put(dst_map, key, strlen(key), item) != OP_MAP_SUCCESS)
		return FAILURE;

	return SUCCESS;
}

//处理内核消息
static void kernel_message(int client_id, cmd_head_t *head, char *data)
{
	switch (head->cmd_code)
	{
		case CMD_KERNEL_HEARTBEAT:
		{
			/* 暂时无事可做 */
		}
		break;
		default:
		break;
	}
}

//处理TCP消息
int deal_tcpmsg(int client_id)
{   
    //变量定义
    cmd_head_t *head = NULL;
	char *data = NULL;
	msg_func_item *func_item;
	tcpmsg_hander hander;
	char cmd[8];

    client_t *cli = (client_t *)item;

    //从输入缓冲区读数据，每次只读一条
	buffer_read(cli->in, head, sizeof(*head));
	buffer_read(cli->in, data, head->data_size);
    
    //如果是内核消息
	if (head->cmd_code <= CMD_KERNEL_END)
	{
		kernel_message(cli->id, head, data);
		return;
	}

    //取出消息处理函数派发消息
	snprintf(cmd, sizeof(cmd), "%u", head->cmd_code);
	func_item = (msg_func_item *)map_get(msg_map, cmd, strlen(cmd));
    //执行消息处理函数
	if (func_item && func_item->msg_func)
	{
		hander = (tcpmsg_hander)func_item->msg_func;
		hander(cli->id, head, data);
	}

    //如果读的是数据头
	if (cli->status == READ_PART_HEAD)
	{
		//数据头参数判断
		cmd_head_t *head = read_ptr(cli->in);
        //如果大于规定值，踢掉。
		if (head->data_size > MAX_CMDDATA_LEN) 
		{
			close_socket(cli);
			if (!g_is_keep_alive) 
				recycle_client(cli);

			return FAILURE;
		}

		//如果客户端提示有数据，却只发送一个包头过来，
        //此后将不会得到任何处理，直到被检测链表断开。
		if (head->data_size > 0) 
		{
			cli->status.part = READ_PART_DATA;
			cli->status.need = head->data_size;
		}
		else
		{
            //加入就绪链表
            if (cli->is_ready == NO)
            {
                if (OP_LIST_SUCCESS == list_push_back(ready_list, cli))
                {
                    cli->is_ready = YES;
                }
            }
		}
	}
	else            //如果读的是数据体
	{
        //加入就绪链表
        if (cli->is_ready == NO)
        {
            if (OP_LIST_SUCCESS == list_push_back(ready_list, cli))
            {
                cli->is_ready = YES;
            }
        }
        
		cli->status.part = READ_PART_HEAD;
		cli->status.need = sizeof(cmd_head_t);
	}
}
