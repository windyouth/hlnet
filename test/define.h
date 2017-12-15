#ifndef _DEFINE_H_
#define _DEFINE_H_

#include "../bin/include/common.h"

#define 		PORT_CLIENT		3366
#define 		PORT_MANAGE		4455
#define 		PORT_UDP		5566

#define			DB_LOGIN		0x000A			//登录消息号
#define			DB_REGISTER		0xFFFF			//注册消息号

#define			MSG_LOGIN		0x00AB			//登录消息号
#define			MSG_REGISTER	0xFFFF			//注册消息号

//登录结构体
typedef struct 
{
	char		account[32];
	char		password[32];
}login_info;

//注册结构体
typedef struct
{
	char		account[32];
	char		password[32];
	char 		secret_key[32];
	char 		corporation[32];
	char 		phone[32];
}reg_info;

//数据库注册
typedef struct
{
	int			client_id;
	reg_info	info;
}db_reg_info;

#endif //_DEFINE_H_
