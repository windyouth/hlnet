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
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//addr.sin_addr = *(get_addr());
	//char *ip = "0.0.0.0";
	//inet_aton(ip, &(addr.sin_addr));

	if (FAILURE == connect(fd, (struct sockaddr *)&addr, sizeof(addr)))
	{
		puts("connect server failed");
		exit(1);
	}
	puts("connect success");

	login_info login;
	bzero(&login, sizeof(login));
	snprintf(login.account, 32, "fuck001");
	snprintf(login.password, 32, "abc123");

	int len;
	if ((len = send(fd, &login, sizeof(login), 0)) < 0)
	{
		puts("send data failed");
		exit(1);
	}
	printf("发送成功，发送字节数：%d \n", len);

	//sleep(3600);
	close(fd);
}
