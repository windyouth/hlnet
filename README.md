hlnet基于微服务理念设计，是一个高性能、接口友好、扩展性强、结构轻巧的网络通信库，用C语言编写。<br>
（友情提示：如要试用请下载v1.0稳定版，目前最新版本部分功能尚在开发中）。<br>

它具备的功能：<br>
可同时监听一个或多个TCP和UDP端口，既支持自定义协议，也支持公有协议（如HTTP）<br>
拥有异步的日志和数据库队列功能。<br>
设计有相对时间定时器和绝对时间定时器。<br>

它的特点：<br>
1.采用epoll的边缘触发(ET)通信方式，这是目前linux系统上效率最高的网络通信模式。<br>
2.采用协程技术，全程实现无锁化编程。<br>
3.藕合度低，扩展性强。一些模块可以很轻松的拆分出来用于其他项目。<br>
4.完成度高，用户使用时几乎不需要关注任何与系统API打交道的细节，可以把所有注意力都放在业务逻辑上。<br>
5.内部数据零拷贝，性能损耗小。<br>
6.使用了内存池，能有效减少重复的内存申请和释放，减少运行过程中内存碎片的产生。<br>
7.采用小顶堆管理定时器，相比传统的线性数据结构，定时器的运行效率有几何级数的提高。<br>
8.采用组件化的设计方式，只有启动相应模块时，其对应的代码才会运行。如果不启动某模块，
  其相应的数据结构不会产生，其代码不会运行，故不会有额外的时间和空间上的损耗。<br>
9.采用生产者-消费者模式，模块间协作非常轻量级。<br>
10.提供的API结构清晰、简单好用。<br>
11.结构轻巧，除去测试部分，包括空行和注释，只有四千多行代码。<br>

性能指标：<br>
经过初步的性能测试，单核的机器，2万5的客户端并发量，QPS可以达到5万。<br>

注意事项：<br>
1.应用层定义消息命令码(cmd_head_t:cmd_code)请以0x10开头，0x00~0x0F被设定为内核命令码。<br>
2.如果使用了数据库线程(start_database)，其与网络通信是使用不同的线程，如果有线程共享数据，要注意访问冲突的问题。<br>

------------------------------使用方法----------------------------------------<br>

执行make之后，会生成一个hlnet的文件夹，将此文件夹拷贝到你的工程目录中即可使用。<br>
hlnet/include中为相应的头文件，hlnet/lib中为生成的动态库文件。<br>

main()函数使用示例如下：<br>

#include "../hlnet/include/server.h" <br>
#include "../hlnet/include/log.h" <br>

void main()<br>
{<br>
	//创建服务器<br>
	serv_create();<br>
	//监听端口<br>
	serv_ctl(socktype_client, PORT_CLIENT);<br>
	//注册连接函数<br>
	reg_link_event(socktype_client, my_link_hander);<br>
	//注册断开函数<br>
	reg_shut_event(socktype_client, my_shut_hander);<br>
	//注册网络消息<br>
	reg_net_msg(socktype_client, MSG_LOGIN, deal_login_msg);<br>
	//初始化日志<br>
	SUCCESS != init_log(".log.txt", loglevel_info);<br>
	//运行服务器<br>
	serv_run();<br>
}<br>

更详细的测试用例请查看test文件夹中的：demo.c、main.c和client.c文件。<br>

------------------------------------------------------------------------------<br>

项目中引用了开源库c-collection和coroutine的代码，并做了一些自己的补充。<br>
在此感谢c-collection的作者ZhangShiming和coroutine的作者云风(cloudwu)大神。<br>

作者：何峦<br>
QQ: 22664834<br>
Email: heluan123132@163.com<br>
