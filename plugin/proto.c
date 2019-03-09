#include "proto.h"
#include "../c-stl/map.h"
#include "../epollet/client.h"

#define				MAX_CMDDATA_LEN			65000			//数据体的最大长度

static int 				g_tcp_fd_user;		                //监听的套接字ID(用户端)
static int 				g_tcp_fd_manage;		            //监听的套接字ID(管理端)

static map				*g_msg_map_user = NULL;			    //网络消息映射(TCP用户端口)
static map				*g_msg_map_manage = NULL;			//网络消息映射(TCP管理端口)
static map				*g_msg_map_udp = NULL;				//网络消息映射(UDP端口)

//内核命令码(0x00~0x0F)
#define				CMD_KERNEL_HEARTBEAT	0x00				//心跳
#define				CMD_KERNEL_END			0x0F				//最后一个内核命令


//正在读的部分
enum _read_part
{
	READ_PART_HEAD 		= 0,			//正在读包头
	READ_PART_DATA 		= 1				//正在读数据
}read_part;

//引导函数
int tcp_guide(int client_id)
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
            //加入就绪链表
            if (cli->is_ready == NO)
            {
                if (OP_LIST_SUCCESS == list_push_back(g_ready_list, cli))
                    cli->is_ready = YES;
            }

            /* 状态保持HEAD不变，need值不变，迎接下一个数据包 */
        }
    }//end if
    else            //读数据
    {
        //加入就绪链表
        if (cli->is_ready == NO)
        {
            if (OP_LIST_SUCCESS == list_push_back(g_ready_list, cli))
                cli->is_ready = YES;
        }

        cli->status = READ_PART_HEAD;
        cli->need = sizeof(cmd_head_t);
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
        set_need(cli->id, total);
        return NO;
    }
    
    return YES;
} 

//处理TCP消息
void deal_tcpmsg(int client_id)
{   
    //变量定义
    cmd_head_t *head = NULL;
	char *data = NULL;
	msg_func_item *func_item;
	tcpmsg_hander hander;

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

        msp_map = (cli->parent == g_tcp_fd_user) ? g_msg_map_user : g_msg_map_manage;

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
    if (buffer_empty(cli->in))
        set_need(cli->id, sizeof(cmd_head_t));
}

//处理UDP消息
void deal_udpmsg(uint ip, ushort port, char *data, uint len)
{
    assert(data);
    if (!data) return;

    cmd_head_t *head = (cmd_head_t *)data;
	char *data = NULL;

	//检验数据
	if (head->data_size + sizeof(*head) != len)
		return;

    //根据命令码从消息映射容器中取出对应的处理函数并回调
    // ...
}

//发送数据(tcp)
int tcp_send(uint client_id, uint16_t cmd, char *data, uint len)
{
	//取得对应的客户端
	client_t *cli = get_client(client_id);
	if (!cli) return PARAM_ERROR;

	//申请缓冲区
	int size = sizeof(cmd_head_t);
	if (data) size += len;
	
	int res = buffer_rectify(cli->out, size);
	if (res != SUCCESS) return res;

	//写数据
	cmd_head_t *head = (cmd_head_t *)write_ptr(cli->out);	
	zero(head);
	head->cmd_code = cmd;
	seek_write(cli->out, sizeof(*head));
	if (data)
	{
	   	head->data_size = len;
		buffer_write(cli->out, data, len);
	}

	//发送，如果没有发成功，由后续的epoll写事件发送。
	res = circle_send(cli->fd, read_ptr(cli->out), cli->out->len);
	if (res > 0) buffer_reset(cli->out);

	return res;
}

//发送数据(udp) ip, port必须是大端(网络序)
int udp_send(uint ip, uint16_t port, uint16_t cmd, char *data, uint len)
{
	//参数检查
	if (len > MAX_UDP_LENGTH - sizeof(cmd_head_t))
		return PARAM_ERROR;

	//填写地址信息
	struct sockaddr_in addr;
	zero(&addr);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	addr.sin_port = port;

	//申请缓冲区
	const int size = data ? sizeof(cmd_head_t) : sizeof(cmd_head_t) + len;
	char buf[size];
	bzero(buf, size);

	//写数据
	cmd_head_t *head = (cmd_head_t *)buf;
	head->cmd_code = cmd;
	if (data)
	{
		head->data_size = len;
		memcpy(buf + sizeof(cmd_head_t), data, len);
	}

	//发送
	return sendto(g_udp_fd, data, len, 0, (struct sockaddr *)&addr, sizeof(addr));
}

//监听用户端口
int listen_user_port(ushort port)
{
	//初始化网络消息映射器
	g_net_user_msg = (map *)malloc(sizeof(map));
	if (!g_net_user_msg) return MEM_ERROR;
	if (map_init(g_net_user_msg) != OP_MAP_SUCCESS) return MEM_ERROR;
}

//监听UDP端口
int listen_udp_port(ushort port)
{
    //初始化网络消息映射器
	g_net_udp_msg = (map *)malloc(sizeof(map));
	if (!g_net_udp_msg) return MEM_ERROR;
	if (map_init(g_net_udp_msg) != OP_MAP_SUCCESS) return MEM_ERROR;

}

//注册网络消息
int reg_net_msg(sock_type sock_type, uint16_t msg, tcpmsg_hander func)
{
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
	if (map_put(dst_map, msg, item) != OP_MAP_SUCCESS)
		return FAILURE;

	return SUCCESS;
}

int reg_udp_msg(uint16_t msg, udpmsg_hander func)
{
	msg_func_item *item = (msg_func_item *)malloc(sizeof(msg_func_item));
	if (!item) return MEM_ERROR;

	item->msg_func = func;

	return (map_put(g_net_udp_msg, msg, item) == OP_MAP_SUCCESS) ? SUCCESS : FAILURE;

}
