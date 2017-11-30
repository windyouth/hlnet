#include <netdb.h>
#include "algorithm.h"

//取得本机IP地址，网络序
struct in_addr *get_addr()
{
	char name[64] = { 0 };
	gethostname(name, sizeof(name));
	struct hostent *host = gethostbyname(name);

	return (struct in_addr *)host->h_addr_list[0];
}
