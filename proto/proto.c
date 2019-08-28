#include "proto.h"
#include "../c-stl/map.h"
#include "../epollet/client.h"

#define				MAX_CMDDATA_LEN			65000			//数据体的最大长度

//内核命令码(0x00~0x0F)
#define				CMD_KERNEL_HEARTBEAT	0x00			//心跳
#define				CMD_KERNEL_END			0x0F			//最后一个内核命令

static int 				g_tcp_fd_user = INVALID_SOCKET;	    //监听的套接字ID(用户端)
static int 				g_tcp_fd_manage = INVALID_SOCKET;   //监听的套接字ID(管理端)
static int 		   		g_udp_fd = INVALID_SOCKET;			//监听的套接字ID(UDP)

static map				*g_msg_map_user = NULL;			    //网络消息映射(TCP用户端口)
static map				*g_msg_map_manage = NULL;			//网络消息映射(TCP管理端口)
static map				*g_msg_map_udp = NULL;				//网络消息映射(UDP端口)

//正在读的部分
enum _read_part
{
	READ_PART_HEAD 		= 0,			//正在读包头
	READ_PART_DATA 		= 1				//正在读数据
}read_part;

//引导函数
static int tcp_guide(int client_id)
{
    //取得客户端结构
    client_t *cli = get_client(client_id);
    if (!cli) return FAILURE;

    //未读完，直接返回
    if (cli->read < cli->need)
        return SUCCESS;

    //如果在读包头
    if (cli->status == READ_PART_HEAD)
    {
        //大于规定值，直接踢掉
        cmd_head_t *head = seek_ptr(cli->in);
        if (head->data_size > MAX_CMDDATA_LEN)
        {
            close_socket(cli);
            return FAILURE;
        }

        //如果客户端提示有数据，却只发一个包头过来，
        //此后将不会得到任何处理，直到被检测链表断开。
        if (head->data_size > 0)
        {
            cli->status = READ_PART_DATA;
            cli->need = head->data_size;
        }
        //只有一个包头，内含命令码的情况
        else
        {
            /* 状态保持HEAD不变，need值不变，迎接下一个数据包 */
            cli->is_ok = YES;
        }
    }//end if
    else            //读数据
    {
        cli->status = READ_PART_HEAD;
        cli->need = sizeof(cmd_head_t);
        cli->is_ok = YES;
    }

    //已读部分置零
    cli->read = 0;
    //seek值跟上write值
    cli->in->seek = cli->in->write;
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

//检验数据长度合法性
static int verify(client_t *cli)
{
    //检验参数
    assert(cli);
    if (!cli) return NO;

    cmd_head_t *head = 0;

    //检验数据长度是否满足数据头
    if (buffer_length(cli->in) < sizeof(cmd_head_t))
        return NO;

    //如果大于规定值，踢掉。
	if (head->data_size > MAX_CMDDATA_LEN) 
	{
		close_socket(cli);
		return NO;
	}

    //检验数据体
	//如果客户端提示有数据，却只发送一个包头过来，
    //此后将不会得到任何处理，直到被检测链表断开。
    head = read_ptr(cli->in);
    int total = head->data_size + sizeof(*head);
    if (total > buffer_length(cli->in))
    {
        cli->need = total;
        return NO;
    }
    
    return YES;
} 

//处理TCP消息
void deal_tcp_msg(int client_id)
{   
    //变量定义
    cmd_head_t *head = NULL;
	char *data = NULL;
	msg_func_item *func_item;
	tcpmsg_hander hander;
    map *msg_map = NULL;

	//取得对应的客户端
	client_t *cli = get_client(client_id);
	if (!cli) return;

    //循环读取消息处理
    while (verify(cli) == YES)
    {
        //从输入缓冲区读数据，每次只读一条
        buffer_read(cli->in, head, sizeof(*head));
        buffer_read(cli->in, data, head->data_size);

        //如果是内核消息
        if (head->cmd_code <= CMD_KERNEL_END)
        {
            kernel_message(cli->id, head, data);
            return;
        }

        msg_map = (cli->parent == g_tcp_fd_user) ? g_msg_map_user : g_msg_map_manage;

        //取出消息处理函数派发消息
        func_item = (msg_func_item *)map_get(msg_map, head->cmd_code);
        //执行消息处理函数
        if (func_item && func_item->msg_func)
        {
            hander = (tcpmsg_hander)func_item->msg_func;
            hander(cli->id, head, data);
        }
    } //end while

    //处理完了，重新设置需要长度
    //if (buffer_empty(cli->in))
    if (cli->in->len <= 0)
        cli->need = sizeof(cmd_head_t);
}

//处理UDP消息
void deal_udp_msg(uint ip, ushort port, char *data, uint len)
{
    assert(data);
    if (!data) return;

    cmd_head_t *head = (cmd_head_t *)data;
    udpmsg_hander hander;

	//检验数据
	if (head->data_size + sizeof(*head) != len)
		return;

    //根据命令码从消息映射容器中取出对应的处理函数并回调
    msg_func_item *func_item = map_get(g_msg_map_udp, head->cmd_code);
    if (func_item && func_item->msg_func)
    {
        hander = (udpmsg_hander)func_item->msg_func;
        hander(ip, port, head, data);
    }
}

//发送数据(tcp)
int send_tcp_data(uint client_id, char *data, uint len)
{
    return send_tcp(client_id, data, len);
}

//发送数据(udp) ip, port必须是大端(网络序)
int send_udp_data(uint ip, ushort port, char *data, uint len)
{
    return send_udp(g_udp_fd, ip, port, data, len);
}

//注册TCP消息
int reg_tcp_msg(sock_type sock_type, ushort msg, tcpmsg_hander func)
{
	map *dst_map = NULL;

	switch (sock_type)
	{
		case sock_type_user:
			{
				dst_map = g_msg_map_user;
			}
			break;
		case sock_type_manage:
			{
				dst_map = g_msg_map_manage;
			}
			break;
		default:
			return PARAM_ERROR;
	}

	msg_func_item *item = (msg_func_item *)malloc(sizeof(msg_func_item));
	if (!item) return MEM_ERROR;

	item->msg_func = func;
	if (map_put(dst_map, msg, item) != OP_MAP_SUCCESS)
		return FAILURE;

	return SUCCESS;
}

//注册UDP消息
int reg_udp_msg(ushort msg, udpmsg_hander func)
{
	msg_func_item *item = (msg_func_item *)malloc(sizeof(msg_func_item));
	if (!item) return MEM_ERROR;

	item->msg_func = func;

	return (map_put(g_msg_map_udp, msg, item) == OP_MAP_SUCCESS) ? SUCCESS : FAILURE;
}

//协议部分初始化
int proto_init()
{
    if (!g_msg_map_user)
    {
        g_msg_map_user = (map *)malloc(sizeof(map));
        if (!g_msg_map_user) return MEM_ERROR;
        if (map_init(g_msg_map_user) != OP_MAP_SUCCESS) return MEM_ERROR;
    }
    
    if (!g_msg_map_manage)
    {
        g_msg_map_manage = (map *)malloc(sizeof(map));
        if (!g_msg_map_manage) return MEM_ERROR;
        if (map_init(g_msg_map_manage) != OP_MAP_SUCCESS) return MEM_ERROR;
    }
    
    if (!g_msg_map_udp)
    {
        g_msg_map_udp = (map *)malloc(sizeof(map));
        if (!g_msg_map_udp) return MEM_ERROR;
        if (map_init(g_msg_map_udp) != OP_MAP_SUCCESS) return MEM_ERROR;
    }

    return serv_init();
}

//监听端口
int listen_port(sock_type type, ushort port)
{
    int fd;
    switch (type)
    {
        case sock_type_user:
            fd = g_tcp_fd_user = listen_tcp(port, tcp_guide, deal_tcp_msg);
            break;
        case sock_type_manage:
            fd = g_tcp_fd_manage = listen_tcp(port, tcp_guide, deal_tcp_msg);
            break;
        case sock_type_udp:
            fd = g_udp_fd = listen_udp(port, deal_udp_msg);
            break;
        default:
            break;
    }

    return (fd == INVALID_SOCKET) ? FAILURE : SUCCESS;
}
