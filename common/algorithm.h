#ifndef _ALGORITHM_H_	
#define _ALGORITHM_H_

#include <string.h>

#define			max(x, y)					(x > y ? x : y)
#define 		min(x, y)					(x < y ? x : y)

#define			zero(ptr)					memset(ptr, 0, sizeof(*ptr))
#define			zero_array(ptr, len)		memset(ptr, 0, len)

//安全释放指针
#define	safe_free(ptr) do						\
{												\
	if (ptr)									\
	{											\
		free(ptr);								\
		ptr = 0;								\
	}											\
}while(0)

#endif //_ALGORITHM_H_
