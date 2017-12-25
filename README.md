hlnet是一个高性能、接口友好、扩展性强、结构轻巧的网络通信库，用C语言编写。

它具备的功能：
可同时监听一个用户端口、一个管理端口，和一个UDP端口，也可只监听其中一个或多个。
拥有异步日志功能和异步的数据库队列。

它的特点：
1.用协程代替多线程，全程实现无锁化编程，大幅提高效率。
2.采用epoll的边缘触发(ET)通信方式，这是目前linux系统上效率最高的网络通信模式。
3.采用组件化的设计方式，只有启动相应模块时，其对应的代码才会运行。如果不启动某模块，
  其相应的数据结构不会产生，其代码不会运行，故不会有额外的时间和空间上的损耗。
4.结构轻巧，除去测试部分，包括空行和注释，只有四千多行代码。
5.内置内存回收器，使用过的内存块可放置在回收器中，下次直接取出使用即可，不需要重复申请。
6.数据在接收消息过程中只拷贝一次，在发送消息过程中零拷贝。
7.整个库只使用两个线程，一个网络线程，一个数据库线程。如果不启用数据库功能，则只使用一个网络线程。这样有助于您根据服务器CPU的核数精确配置进程数量，从而尽可能减少运行时CPU调度产生的系统开销。
8.采用生产者-消费者模式，模块间协作非常轻量级。

注意事项：
1.应用层定义消息命令码(cmd_head_t:cmd_code)请以0x10开头，0x00~0x0F被设定为内核命令码。
2.如果使用了数据库线程(start_database)，其与网络通信是使用不同的线程，如果有线程共享数据，要注意访问冲突的问题。

------------------------------使用示例----------------------------------------

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

------------------------------------------------------------------------------

项目中引用了开源库c-collection和coroutine的代码，并做了一些自己的补充。
在此感谢c-collection的作者ZhangShiming和coroutine的作者云风(cloudwu)大神。

作者：何峦
QQ: 22664834
Email: heluan123132@163.com
