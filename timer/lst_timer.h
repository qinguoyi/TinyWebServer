#ifndef LST_TIMER
#define LST_TIMER

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/uio.h>

#include <time.h>
#include "../log/log.h"

/*
 * 前向声明：两个元素之前互相调用，但是因为声明顺序的关系，
   导致一个元素找不到另一个元素的声明，所以要提前声明
 */
class util_timer;

/*
 * 用户数据结构
 * address 客户端socket地址
   sockfd 客户端对应的socket文件描述符
   timer 定时器
 */
struct client_data
{
	sockaddr_in address;
	int sockfd;
	util_timer *timer;
};

/*
 * 定时器类
 * expire 任务的超时时间，这里设定为绝对时间，即Unix时间，以s为单位
   user_data 指向用户数据
   prev 指向前一个定时器
   next 指向下一个定时器
   cb_func 定时器回调函数(要执行的函数)
 */
class util_timer
{
public:
	util_timer() : prev(NULL), next(NULL) {}

public:
	time_t expire;

	void (*cb_func)(client_data *);
	client_data *user_data;
	util_timer *prev;
	util_timer *next;
};

/*
 * 定时器链表
   一个按照过期时间升序，双向链表，且带有头尾指针
 * head 头指针指向第一个定时器
   tail 尾指针指向最后一个定时器
   private:add_timer 一个重载的辅助函数，被public的add_timer和adjust_timer函数调用，
       该函数用于将timer添加到节点lst_head之后的部分链表中
 * add_timer 将定时器添加到链表中
   adjust_timer 修改链表中的某个定时器，只支持过期时间延长
   del_timer 将定时器从链表中删除
   tick SIGALRM信号被触发，即有任务到期，执行此函数处理链表上所有到期的任务
 */
class sort_timer_lst
{
public:
	sort_timer_lst();
	~sort_timer_lst();

	void add_timer(util_timer *timer);
	void adjust_timer(util_timer *timer);
	void del_timer(util_timer *timer);
	void tick();

private:
	void add_timer(util_timer *timer, util_timer *lst_head);

	util_timer *head;
	util_timer *tail;
};

/*
 * 封装客户类
 * u_pipefd 通信的管道
   m_timer_lst 定时器链表
   u_epollfd epoll事件表的文件描述符
   m_TIMESLOT 设定一个定时时间触发SIGALRM信号
 * init 初始化
   setnonblocking 对文件描述符设置非阻塞
       因为epoll采用ET边沿触发，如果是阻塞的，socket的读写操作将会因为没有后续的事件而一直处于阻塞状态
   addfd 将事件添加到epoll时间表中，根据判断TRIGMode==1决定是否开启ET工作模式
   sig_handler 信号处理函数
   addsig 设置信号对应的信号处理函数
   timer_handler 定时处理超时的任务，并通过alarm重新定时以不断触发SIGALRM信号
   show_error 向客户端发送错误信息，并关闭该连接
 */
class Utils
{
public:
	Utils() {}
	~Utils() {}

	void init(int timeslot);

	//对文件描述符设置非阻塞
	int setnonblocking(int fd);

	//将内核事件表注册读事件，ET模式，选择开启EPOLLONESHOT
	void addfd(int epollfd, int fd, bool one_shot, int TRIGMode);

	//信号处理函数
	static void sig_handler(int sig);

	//设置信号函数
	void addsig(int sig, void(handler)(int), bool restart = true);

	//定时处理任务，重新定时以不断触发SIGALRM信号
	void timer_handler();

	void show_error(int connfd, const char *info);

public:
	static int *u_pipefd;
	sort_timer_lst m_timer_lst;
	static int u_epollfd;
	int m_TIMESLOT;
};

void cb_func(client_data *user_data);

#endif
