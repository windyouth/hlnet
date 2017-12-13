#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "define.h"
#include "../bin/include/algorithm.h"

void show_addr()
{
	char local[128] = { 0 };
	struct hostent *hent;	
	gethostname(local, sizeof(local));
	printf("local: %s \n", local);
	hent = gethostbyname(local);
	printf("hent->h_name: %s \n", hent->h_name);

	for (int i = 0; hent->h_addr_list[i]; ++i)
	{
		printf("%s\t", inet_ntoa(*(struct in_addr *)hent->h_addr_list[i]));
	}
	printf("\n");
}

void main()
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == fd)
	{
		puts("socket create failed");
		exit(1);
	}

	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT_CLIENT);
	//addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//addr.sin_addr = *(get_addr());
	char *ip = "192.168.1.10";
	inet_aton(ip, &(addr.sin_addr));

	if (FAILURE == connect(fd, (struct sockaddr *)&addr, sizeof(addr)))
	{
		puts("connect server failed");
		exit(1);
	}
	puts("connect success");

	int len = sizeof(cmd_head_t) + sizeof(login_info);
	char buf[len];
	bzero(buf, len);

	cmd_head_t *head = (cmd_head_t *)buf;
	head->data_size = sizeof(login_info);
	head->cmd_code = MSG_LOGIN;
	head->proto_ver = 5;

	login_info *login = (login_info *)(head + 1);
	bzero(login, sizeof(login));
	snprintf(login->account, 32, "fuck001");
	snprintf(login->password, 32, "abc123");

	int res;
	if ((res = send(fd, buf, len, 0)) < 0)
	{
		puts("send data failed");
		exit(1);
	}
	printf("发送成功，发送字节数：%d \n", res);

	res = recv(fd, buf, len, 0);
	printf("the bytes of data from server：%d \n", res);
	if (res > 0)
	{
		buf[res] = 0;
		head = (cmd_head_t *)buf;
		char *data = (char *)(head + 1);
		printf("the message from server：\n");
		printf("命令码：0x00%X \n", head->cmd_code);
		printf("协议版本号：%d \n", head->proto_ver);
		printf("数据长度：%d \n", head->data_size);
		printf("数据内容：%s \n", data);
	}

	usleep(1000);
	//pause();
	close(fd);
}
