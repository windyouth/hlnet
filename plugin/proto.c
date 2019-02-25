#include "proto.h"

#define				MAX_CMDDATA_LEN			65000			//数据体的最大长度

//正在读的部分
enum read_part_e
{
	READ_PART_HEAD 		= 1,			//正在读包头
	READ_PART_DATA 		= 2				//正在读数据
};

//处理TCP消息
int deal_tcpmsg(int client_id, char *data, uint len)
{
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
