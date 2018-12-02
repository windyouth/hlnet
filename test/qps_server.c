#include <stdio.h>
#include <assert.h>
#include <arpa/inet.h>
#include "../hlnet/include/server.h"
#include "define.h"

//连接事件
int my_link_hander(int client_id, uint32_t ip)
{
	struct in_addr addr;
	addr.s_addr = ip;
	printf("客户端连接，id: %d, ip: %s \n", client_id, inet_ntoa(addr));

	return 0;
}

//关闭事件
int my_shut_hander(int client_id)
{
	printf("客户端断开，id: %d \n", client_id);

	return 0;
}

//处理登录数据库消息
int on_netmsg_qps(int client_id, cmd_head_t *head, char *data)
{
	assert(data && head && head->data_size == sizeof(reg_info));
	if (!data || !head || head->data_size != sizeof(reg_info))
	{	
		puts("收到注册消息，但参数不合法。");
		return PARAM_ERROR;
	}

	reg_info *reg = (reg_info *)data;

	db_reg_info info;
	info.client_id = client_id;
	memcpy(&info.info, data, head->data_size);

	//发送消息
	tcp_send(client_id, MSG_QPS, &info, sizeof(info));

    return 0;
}

void main()
{
    //创建服务器
	if (SUCCESS != serv_create())
		puts("serv_create failure");
	//监听端口
	if (SUCCESS != serv_ctl(socktype_client, PORT_CLIENT))
		puts("serv_ctl failure");

	//注册连接函数
	if (SUCCESS != reg_link_event(socktype_client, my_link_hander))
		puts("reg_link_event failure");
	//注册断开函数
	if (SUCCESS != reg_shut_event(socktype_client, my_shut_hander))
		puts("reg_shut_event failure");

	//注册网络消息
	if (SUCCESS != reg_net_msg(socktype_client, MSG_QPS, on_netmsg_qps))
		puts("reg_net_msg failure");

    set_first_length(socktype_client, sizeof(cmd_head_t));

	//运行服务器
	puts("pressure test, come on ......");
	puts("---------------------------------------");
	serv_run();
}
