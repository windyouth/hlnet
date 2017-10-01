#ifndef _KEEP_ALIVE_H_
#define _KEEP_ALIVE_H_

//添加到心跳检测池
void keep_alive(int client_id);
//更新活跃时间
void alive(int client_id);
//设为安全连接
int safe(int client_id);
//查询是否是安全连接
int is_safe(int client_id);
//断开客户端连接
int shut_client(int client_id);

#endif //_KEEP_ALIVE_H_
