#include <stdio.h>
#include "../src/server.h"

//连接事件
int my_link_hander(int client_id, uint32_t ip)
{
	return 0;
}
//关闭事件
int my_shut_hander(int client_id)
{
	return 0;
}

//TCP消息函数
int my_tcpmsg_hander(int client_id, cmd_head_t *head, char *data)
{
	return 0;
}

//UDP消息函数，ip和port是大端(网络序)
int my_udpmsg_hander(uint32_t ip, uint16_t port, cmd_head_t *head, char *data)
{
	return 0;
}

int main()
{
	if (SUCCESS != serv_create())
	{
		puts("serv_create failed");
		return -1;
	}
	puts("serv_create success");

	if (SUCCESS != serv_ctl(socktype_client, 3366))
	{
		puts("serv_ctl(socktype_client, 3366) failed");
		return -1;
	}
	puts("serv_ctl(socktype_client, 3366) success");

	if (SUCCESS != serv_ctl(socktype_manage, 4455))
	{
		puts("serv_ctl(socktype_manage, 4455) failed");
		return -1;
	}
	puts("serv_ctl(socktype_manage, 4455) success");

	if (SUCCESS != serv_ctl(socktype_udp, 5566))
	{
		puts("serv_ctl(socktype_udp, 5566) failed");
		return -1;
	}
	puts("serv_ctl(socktype_udp, 5566) success");

	if (SUCCESS != reg_link_event(socktype_client, my_link_hander))
	{
		puts("reg_link_event failed");
		return -1;
	}
	puts("reg_link_event success");

	if (SUCCESS != reg_shut_event(socktype_manage, my_shut_hander))
	{
		puts("reg_shut_event failed");
		return -1;
	}
	puts("reg_shut_event success");

	if (SUCCESS != reg_net_msg(socktype_client, 0x00A0, my_tcpmsg_hander))
	{
		puts("reg_net_msg failed");
		return -1;
	}
	puts("reg_net_msg success");

	if (SUCCESS != reg_udp_msg(0xAABB, my_udpmsg_hander))
	{
		puts("reg_udp_msg failed");
		return -1;
	}
	puts("reg_udp_msg success");

	while (1)
		usleep(10);

	return 0;
}
