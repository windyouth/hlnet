#ifndef _ALGORITHM_H_	
#define _ALGORITHM_H_

#include <string.h>

#define			max(x, y)					(x > y ? x : y)
#define 		min(x, y)					(x < y ? x : y)

#define			zero(ptr)					memset((ptr), 0, sizeof(*(ptr)))
#define			mem_zero(ptr, len)		    memset(ptr, 0, len)

#define			array_size(array)			(sizeof(array) / sizeof(array[0]))

//安全释放指针
#define	safe_free(ptr) do						\
{												\
	if (ptr)									\
	{											\
		free(ptr);								\
		ptr = 0;								\
	}											\
}while(0)

//取得本机IP地址，网络序
struct in_addr *get_addr();

#endif //_ALGORITHM_H_
