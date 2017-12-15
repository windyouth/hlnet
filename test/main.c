#include <stdio.h>
#include <assert.h>
#include <arpa/inet.h>
#include <mysql/mysql.h>
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

	if (SUCCESS != reg_net_msg(socktype_client, MSG_LOGIN, my_tcpmsg_hander))
	{
		puts("reg_net_msg failed");
		return -1;
	}
	puts("reg_net_msg success");

	if (SUCCESS != reg_udp_msg(MSG_LOGIN, my_udpmsg_hander))
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

	char *rsp = "我是服务器，已收到你的登录消息。";
	tcp_send(client_id, MSG_LOGIN, rsp, strlen(rsp));

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
	
	return SUCCESS;
}

MYSQL		*g_link = NULL;
MYSQL_RES	*g_result = NULL;
MYSQL_ROW	g_row;

//初始化数据库
int init_mysql()
{
	g_link = mysql_init(NULL);
	if (!g_link)
	{
		puts("mysql_init failure");
		return FAILURE;
	}

	if (!mysql_real_connect(g_link, "localhost", "heluan", "heluanhl", 
							"test", 3306, 0, 0))
	{
		puts("mysql_connect failure");
		mysql_close(g_link);
		return FAILURE;
	}

	/*
	if (mysql_real_query(g_link, "set names utf8"))
	{
		mysql_close(g_link);
		return FAILURE;
	}*/

	return SUCCESS;
}

//处理数据库注册消息
int deal_dbmsg_reg(char *data, uint32_t len)
{
	assert(data && len >= sizeof(db_reg_info));
	if (!data || len < sizeof(db_reg_info)) return PARAM_ERROR;

	db_reg_info *reginfo = (db_reg_info *)data;
	char sql[256] = { 0 };
	snprintf(sql, 256, "call add_user('%s', '%s', '%s', '%s', '%s')", 
			reginfo->info.account, reginfo->info.password, reginfo->info.secret_key, 
			reginfo->info.corporation, reginfo->info.phone);

	if (mysql_real_query(g_link, sql, strlen(sql)))
	{
		puts("mysql_real_query failure");
		return FAILURE;
	}

	g_result = mysql_store_result(g_link);
	if (NULL == g_result)
	{
		log(loglevel_error, "mysql_store_result failure, g_result == NULL");
		return FAILURE;
	}

	puts("注册成功");
	while (g_row = mysql_fetch_row(g_result))
	{
		//printf("user_id: %d");
	}

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
	if (SUCCESS != reg_shut_event(socktype_client, my_shut_hander))
		puts("reg_shut_event failure");

	//注册网络消息
	if (SUCCESS != reg_net_msg(socktype_client, MSG_LOGIN, deal_login_msg))
		puts("reg_net_msg failure");

	if (SUCCESS != reg_net_msg(socktype_client, MSG_REGISTER, deal_reg_msg))
		puts("reg_net_msg failure");

	//初始化日志
	if (SUCCESS != init_log(".log.txt", loglevel_error))
		puts("init_log failure");

	//运行服务器
	puts("serv_run...");
	puts("---------------------------------------");
	serv_run();

	return 0;
}
