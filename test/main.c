#include <stdio.h>
#include <assert.h>
#include <arpa/inet.h>
#include "../src/server.h"
#include "../src/log.h"
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

int api_test()
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

	return 0;
}

//处理登录数据库消息
int deal_login_msg(int client_id, cmd_head_t *head, char *data)
{
	assert(data && head && head->data_size == sizeof(login_info));
	if (!data || !head || head->data_size != sizeof(login_info)) 
	{	
		puts("收到登录消息，但参数不合法。");
		return PARAM_ERROR;
	}
	login_info *login = (login_info *)data;

	puts("收到登录数据如下:");
	printf("命令码：%X \n", head->cmd_code);
	printf("协议版本号：%d \n", head->proto_ver);
	printf("账号：%s \n", login->account);
	printf("密码：%s \n", login->password);

	return SUCCESS;
}

//处理注册数据库消息
int deal_reg_msg(int client_id, cmd_head_t *head, char *data)
{
	assert(data && head && head->data_size == sizeof(reg_info));
	if (!data || !head || head->data_size != sizeof(login_info))
	{	
		puts("收到注册消息，但参数不合法。");
		return PARAM_ERROR;
	}

	reg_info *reg = (reg_info *)data;

	puts("收到注册数据如下:");
	printf("命令码：%X \n", head->cmd_code);
	printf("协议版本号：%d \n", head->proto_ver);
	printf("账号：%s \n", reg->account);
	printf("密码：%s \n", reg->password);
	printf("姓名：%s \n", reg->name);
	printf("性别：%s \n", reg->sex == 1 ? "男" : "女");
	printf("地址：%s \n", reg->addr);
	printf("备注：%s \n", reg->remark);

	return SUCCESS;
}

int main()
{
	//创建服务器
	serv_create();
	//监听端口
	serv_ctl(socktype_client, PORT_CLIENT);
	//注册连接函数
	reg_link_event(socktype_client, my_link_hander);
	//注册断开函数
	reg_shut_event(socktype_client, my_shut_hander);
	//注册网络消息
	reg_net_msg(socktype_client, DB_LOGIN, deal_login_msg);
	reg_net_msg(socktype_client, DB_REGISTER, deal_reg_msg);
	//初始化日志
	init_log(".log.txt", loglevel_error);
	//运行服务器
	serv_run();

	return 0;
}
