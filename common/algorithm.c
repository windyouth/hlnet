#include <netdb.h>
#include <stdlib.h>
#include "algorithm.h"

//取得本机IP地址，网络序
struct in_addr *get_addr()
{
	char name[64] = { 0 };
	gethostname(name, sizeof(name));
	struct hostent *host = gethostbyname(name);

	return (struct in_addr *)host->h_addr_list[0];
}

//取0~n-1的随机数，且不重复
void get_rand(int a[], int n)
{
	int i, temp, index;

	//0~n-1赋初值
	for (i = 0; i < n; i++)
		a[i] = i;

	srand((unsigned)time(NULL));

	//取出随机下标后，与末尾交换位置
	for (i = n - 1; i > 0; i--)
	{
		index = rand() % i;

		//交换到末尾
		temp = a[i];
		a[i] = a[index];
		a[index] = temp;
	}
}

