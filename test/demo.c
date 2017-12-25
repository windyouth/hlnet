#include <stdio.h>
#include <arpa/inet.h>
#include "../hlnet/include/server.h"
#include "../hlnet/include/log.h"
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
//处理登录消息
int deal_login_msg(int client_id, cmd_head_t *head, char *data)
{
	log(loglevel_info, "数据大小: %d, 命令码: %d, 协议版本: %d", 
		head->data_size, head->cmd_code, head->proto_ver);
	char *rsp = "我是服务器，已收到你的登录消息。";
	tcp_send(client_id, MSG_LOGIN, rsp, strlen(rsp));

	return SUCCESS;
}
void main()
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
	reg_net_msg(socktype_client, MSG_LOGIN, deal_login_msg);
	//初始化日志
	SUCCESS != init_log(".log.txt", loglevel_info);
	//运行服务器
	serv_run();
}
