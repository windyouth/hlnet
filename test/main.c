#include <stdio.h>
#include <assert.h>
#include <arpa/inet.h>
#include <mysql/mysql.h>
#include "../hlnet/include/server.h"
#include "../hlnet/include/log.h"
#include "../plugin/mysql_helper.h"
#include "../hlnet/include/timer.h"
#include "../hlnet/include/moment.h"
#include "../hlnet/include/alive.h"
#include "../epollet/client.h"
#include "define.h"

MYSQL				*g_mysql = NULL;			//mysql连接

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

	if (SUCCESS != serv_ctl(socktype_user, 3366))
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

	if (SUCCESS != reg_link_event(socktype_user, my_link_hander))
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

	if (SUCCESS != reg_net_msg(socktype_user, MSG_LOGIN, my_tcpmsg_hander))
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
int on_netmsg_login(int client_id, cmd_head_t *head, char *data)
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

//网络消息：注册
int on_netmsg_reg(int client_id, cmd_head_t *head, char *data)
{
	assert(data && head && head->data_size == sizeof(reg_info));
	if (!data || !head || head->data_size != sizeof(reg_info))
	{	
		puts("收到注册消息，但参数不合法。");
		return PARAM_ERROR;
	}

	reg_info *reg = (reg_info *)data;

	/*
	puts("收到注册数据如下:");
	printf("命令码：0x%X \n", head->cmd_code);
	printf("协议版本号：%d \n", head->proto_ver);
	*/

	db_reg_info info;
	info.client_id = client_id;
	memcpy(&info.info, data, head->data_size);
	post_db_msg(DB_REGISTER, &info, sizeof(info));

	return SUCCESS;
}

//网络消息：登录
int on_dbmsg_login(char *data, uint32_t len)
{
	return SUCCESS;
}

//处理数据库注册消息
int on_dbmsg_reg(char *data, uint32_t len)
{
	assert(data && len >= sizeof(db_reg_info));
	if (!data || len < sizeof(db_reg_info)) return PARAM_ERROR;

	db_reg_info *reginfo = (db_reg_info *)data;
	char sql[256] = { 0 };
	snprintf(sql, 256, "call add_user('%s', '%s', '%s', '%s', '%s')", 
			reginfo->info.account, reginfo->info.password, reginfo->info.secret_key, 
			reginfo->info.corporation, reginfo->info.phone);


	char rsp[128];
	mysql_set *set = mysql_execute(g_mysql, sql);
	if (!set)
	{
		printf("mysql_execute failure, errmsg: %s \n", mysql_error(g_mysql));

		return FAILURE;
	}

	//从结果集中读取数据
	while (!mysql_eof(set))
	{
		snprintf(rsp, 128, "%s, user_id: %d", mysql_get_string(set, "message"), 
				mysql_get_int(set, "user_id"));
		//puts(rsp);
		mysql_next_row(set);
	}

	//关闭结果集
	mysql_set_close(g_mysql, set);

	//发送消息
	tcp_send(reginfo->client_id, MSG_REGISTER, rsp, strlen(rsp));
    //log(loglevel_error, rsp);

	return SUCCESS;
}

//定时器触发事件
int timer_func(struct _timer *timer)
{
    static int count = 0;
    printf("第%d次执行定时器：%s 当前clock时间：%d\n", 
            ++count, (char *)timer->data, clock());
}

//定时器触发事件
int moment_func(struct _moment *moment)
{
    static int count = 0;
    printf("第%d次执行定时器：%s 当前clock时间：%d\n", 
            ++count, (char *)moment->data, clock());
}

client_t *g_client1;
client_t *g_client2;
client_t *g_client3;
client_t *g_client4;
client_t *g_client5;

//心跳模块测试
void alive_test()
{
	if (SUCCESS != keep_alive())
	{
		puts("初始化心跳模块失败");
		return;
	}

	if (SUCCESS != client_store_init())
	{
		puts("初始化客户端仓库失败");
		//return;
	}

	g_client1 = extract_client();
	g_client2 = extract_client();
	g_client3 = extract_client();
	g_client4 = extract_client();
	g_client5 = extract_client();

	g_client1->fd = 1001;
	g_client2->fd = 1002;
	g_client3->fd = 1003;
	g_client4->fd = 1004;
	g_client5->fd = 1005;

	add_alive(g_client1->id);
	add_alive(g_client2->id);
	add_alive(g_client3->id);
	add_alive(g_client4->id);
	add_alive(g_client5->id);

	alive(g_client1->id);
	alive(g_client2->id);
	alive(g_client3->id);
	alive(g_client4->id);
	alive(g_client5->id);
	
	safe(g_client1->id);
	safe(g_client3->id);
	safe(g_client5->id);

    close_socket(g_client4);

	printf("g_client1 safe: %d \n", is_safe(g_client1->id));
	printf("g_client2 safe: %d \n", is_safe(g_client2->id));
	printf("g_client3 safe: %d \n", is_safe(g_client3->id));
	printf("g_client4 safe: %d \n", is_safe(g_client4->id));
	printf("g_client5 safe: %d \n", is_safe(g_client5->id));

}

int main()
{
	//创建服务器
	if (SUCCESS != serv_create())
		puts("serv_create failure");
	//监听端口
	if (SUCCESS != serv_ctl(socktype_user, PORT_CLIENT))
		puts("serv_ctl failure");

	//注册连接函数
	if (SUCCESS != reg_link_event(socktype_user, my_link_hander))
		puts("reg_link_event failure");
	//注册断开函数
	if (SUCCESS != reg_shut_event(socktype_user, my_shut_hander))
		puts("reg_shut_event failure");

	//注册网络消息
	if (SUCCESS != reg_net_msg(socktype_user, MSG_LOGIN, on_netmsg_login))
		puts("reg_net_msg failure");
	if (SUCCESS != reg_net_msg(socktype_user, MSG_REGISTER, on_netmsg_reg))
		puts("reg_net_msg failure");

	//初始化数据库
	init_database();
	//注册数据库消息
	if (SUCCESS != reg_db_msg(DB_LOGIN, on_dbmsg_login))
		puts("reg_db_msg DB_LOGIN failure");
	if (SUCCESS != reg_db_msg(DB_REGISTER, on_dbmsg_reg))
		puts("reg_net_msg DB_REGISTER failure");
	//启动数据库线程
	start_database();

	//初始化日志
	if (SUCCESS != init_log("log.txt", loglevel_error))
		puts("init_log failure");

	//初始化数据库
	g_mysql = mysql_create("localhost", 3306, "heluan", "heluanhl", "test", 1000);
	if (!g_mysql) puts("mysql_create failure");

    //初始化定时器
    char *msg = "没有共产党就没有新中国。";
    /*
    timer_manager();
    add_timer(1, 100, timer_func, msg);
    */
    /*
    moment_manager();
    add_moment(time(NULL) + 5, moment_func, msg);
    */

    //alive_test();

    set_first_length(socktype_user, sizeof(cmd_head_t));

	//运行服务器
	puts("serv_run...");
	puts("---------------------------------------");
	serv_run();

	return 0;
}
