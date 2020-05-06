#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <sys/epoll.h>

#include "./lock/locker.h"
#include "./threadpool/threadpool.h"
#include "./timer/lst_timer.h"
#include "./http/http_conn.h"
#include "./log/log.h"
#include "./CGImysql/sql_connection_pool.h"

const int MAX_FD = 65536;           //最大文件描述符
const int MAX_EVENT_NUMBER = 10000; //最大事件数
const int TIMESLOT = 5;             //最小超时单位

//定时器相关参数
static int pipefd[2];
sort_timer_lst timer_lst;
static int epollfd = 0;
int *Utils::m_pipefd = pipefd;
int Utils::m_epollfd = epollfd;

int main(int argc, char *argv[])
{
    //端口号,默认9006
    int port = 9006;

    //数据库校验方式，默认同步
    int SQLVerify = 0;

    //日志写入方式，默认同步
    int LOGWrite = 0;

    //触发模式，默认LT
    int TRIGMode = 0;

    //优雅关闭链接，默认不使用
    int OPT_LINGER = 0;

    int opt;
    const char *str = "p:s:l:t:o";
    while ((opt = getopt(argc, argv, str)) != -1)
    {
        switch (opt)
        {
        case 'p':
        {
            port = atoi(optarg);
            break;
        }
        case 's':
        {
            SQLVerify = atoi(optarg);
            break;
        }
        case 'l':
        {
            LOGWrite = atoi(optarg);
            break;
        }
        case 't':
        {
            TRIGMode = atoi(optarg);
            break;
        }
        case 'o':
        {
            OPT_LINGER = atoi(optarg);
            break;
        }
        default:
            break;
        }
    }

    //生成日志
    if (1 == LOGWrite)
        Log::get_instance()->init("./ServerLog", 2000, 800000, 800);
    else
        Log::get_instance()->init("./ServerLog", 2000, 800000, 0);

    //创建数据库连接池
    connection_pool *connPool = connection_pool::GetInstance();
    connPool->init("localhost", "root", "root", "qgydb", 3306, 8);

    //创建线程池
    threadpool<http_conn> *pool = NULL;
    try
    {
        pool = new threadpool<http_conn>(connPool);
    }
    catch (...)
    {
        return 1;
    }

    http_conn *users = new http_conn[MAX_FD];
    assert(users);

    //初始化数据库读取表
    map<string, string> users_passwd;

    if (0 == SQLVerify)
        users->initmysql_result(connPool, users_passwd);
    else if (1 == SQLVerify)
        users->initresultFile(connPool, users_passwd);

    //root文件夹路径
    char server_path[200];
    getcwd(server_path, 200);
    char root[6] = "/root";
    char *m_root = (char *)malloc(strlen(server_path) + strlen(root) + 1);
    strcpy(m_root, server_path);
    strcat(m_root, root);

    //网络编程基础步骤
    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);

    //优雅关闭连接
    if (0 == OPT_LINGER)
    {
        struct linger tmp = {0, 1};
        setsockopt(listenfd, SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));
    }
    else if (1 == OPT_LINGER)
    {
        struct linger tmp = {1, 1};
        setsockopt(listenfd, SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));
    }

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    int flag = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    ret = bind(listenfd, (struct sockaddr *)&address, sizeof(address));
    assert(ret >= 0);
    ret = listen(listenfd, 5);
    assert(ret >= 0);

    //工具类,信号和描述符基础操作
    Utils utils;
    utils.init(timer_lst, TIMESLOT);

    //epoll创建内核事件表
    epoll_event events[MAX_EVENT_NUMBER];
    epollfd = epoll_create(5);
    assert(epollfd != -1);

    utils.addfd(epollfd, listenfd, false, TRIGMode);
    http_conn::m_epollfd = epollfd;

    //定时器相关
    ret = socketpair(PF_UNIX, SOCK_STREAM, 0, pipefd);
    assert(ret != -1);
    utils.setnonblocking(pipefd[1]);
    utils.addfd(epollfd, pipefd[0], false, TRIGMode);

    utils.addsig(SIGPIPE, SIG_IGN);
    utils.addsig(SIGALRM, utils.sig_handler, false);
    utils.addsig(SIGTERM, utils.sig_handler, false);

    bool stop_server = false;
    bool timeout = false;
    alarm(TIMESLOT);
    client_data *users_timer = new client_data[MAX_FD];

    //主循环
    while (!stop_server)
    {
        int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if (number < 0 && errno != EINTR)
        {
            LOG_ERROR("%s", "epoll failure");
            break;
        }

        for (int i = 0; i < number; i++)
        {
            int sockfd = events[i].data.fd;

            //处理新到的客户连接
            if (sockfd == listenfd)
            {
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof(client_address);
                if (0 == TRIGMode)
                {
                    int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addrlength);
                    if (connfd < 0)
                    {
                        LOG_ERROR("%s:errno is:%d", "accept error", errno);
                        continue;
                    }
                    if (http_conn::m_user_count >= MAX_FD)
                    {
                        utils.show_error(connfd, "Internal server busy");
                        LOG_ERROR("%s", "Internal server busy");
                        continue;
                    }
                    users[connfd].init(connfd, client_address, m_root, users_passwd, SQLVerify, TRIGMode);

                    //初始化client_data数据
                    //创建定时器，设置回调函数和超时时间，绑定用户数据，将定时器添加到链表中
                    users_timer[connfd].address = client_address;
                    users_timer[connfd].sockfd = connfd;
                    util_timer *timer = new util_timer;
                    timer->user_data = &users_timer[connfd];
                    timer->cb_func = cb_func;
                    time_t cur = time(NULL);
                    timer->expire = cur + 3 * TIMESLOT;
                    users_timer[connfd].timer = timer;
                    timer_lst.add_timer(timer);
                }
                else
                {
                    while (1)
                    {
                        int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addrlength);
                        if (connfd < 0)
                        {
                            LOG_ERROR("%s:errno is:%d", "accept error", errno);
                            break;
                        }
                        if (http_conn::m_user_count >= MAX_FD)
                        {
                            utils.show_error(connfd, "Internal server busy");
                            LOG_ERROR("%s", "Internal server busy");
                            break;
                        }
                        users[connfd].init(connfd, client_address, m_root, users_passwd, SQLVerify, TRIGMode);

                        //初始化client_data数据
                        //创建定时器，设置回调函数和超时时间，绑定用户数据，将定时器添加到链表中
                        users_timer[connfd].address = client_address;
                        users_timer[connfd].sockfd = connfd;
                        util_timer *timer = new util_timer;
                        timer->user_data = &users_timer[connfd];
                        timer->cb_func = cb_func;
                        time_t cur = time(NULL);
                        timer->expire = cur + 3 * TIMESLOT;
                        users_timer[connfd].timer = timer;
                        timer_lst.add_timer(timer);
                    }
                    continue;
                }
            }

            else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
            {
                //服务器端关闭连接，移除对应的定时器
                util_timer *timer = users_timer[sockfd].timer;
                timer->cb_func(&users_timer[sockfd]);

                if (timer)
                {
                    timer_lst.del_timer(timer);
                }
            }

            //处理信号
            else if ((sockfd == pipefd[0]) && (events[i].events & EPOLLIN))
            {
                int sig;
                char signals[1024];
                ret = recv(pipefd[0], signals, sizeof(signals), 0);
                if (ret == -1)
                {
                    continue;
                }
                else if (ret == 0)
                {
                    continue;
                }
                else
                {
                    for (int i = 0; i < ret; ++i)
                    {
                        switch (signals[i])
                        {
                        case SIGALRM:
                        {
                            timeout = true;
                            break;
                        }
                        case SIGTERM:
                        {
                            stop_server = true;
                        }
                        }
                    }
                }
            }

            //处理客户连接上接收到的数据
            else if (events[i].events & EPOLLIN)
            {
                util_timer *timer = users_timer[sockfd].timer;
                if (users[sockfd].read_once())
                {
                    LOG_INFO("deal with the client(%s)", inet_ntoa(users[sockfd].get_address()->sin_addr));
                    Log::get_instance()->flush();
                    //若监测到读事件，将该事件放入请求队列
                    pool->append(users + sockfd);

                    //若有数据传输，则将定时器往后延迟3个单位
                    //并对新的定时器在链表上的位置进行调整
                    if (timer)
                    {
                        time_t cur = time(NULL);
                        timer->expire = cur + 3 * TIMESLOT;
                        LOG_INFO("%s", "adjust timer once");
                        Log::get_instance()->flush();
                        timer_lst.adjust_timer(timer);
                    }
                }
                else
                {
                    timer->cb_func(&users_timer[sockfd]);
                    if (timer)
                    {
                        timer_lst.del_timer(timer);
                    }
                }
            }
            else if (events[i].events & EPOLLOUT)
            {
                util_timer *timer = users_timer[sockfd].timer;
                if (users[sockfd].write())
                {
                    LOG_INFO("send data to the client(%s)", inet_ntoa(users[sockfd].get_address()->sin_addr));
                    Log::get_instance()->flush();

                    //若有数据传输，则将定时器往后延迟3个单位
                    //并对新的定时器在链表上的位置进行调整
                    if (timer)
                    {
                        time_t cur = time(NULL);
                        timer->expire = cur + 3 * TIMESLOT;
                        LOG_INFO("%s", "adjust timer once");
                        Log::get_instance()->flush();
                        timer_lst.adjust_timer(timer);
                    }
                }
                else
                {
                    timer->cb_func(&users_timer[sockfd]);
                    if (timer)
                    {
                        timer_lst.del_timer(timer);
                    }
                }
            }
        }
        if (timeout)
        {
            utils.timer_handler();
            timeout = false;
        }
    }
    close(epollfd);
    close(listenfd);
    close(pipefd[1]);
    close(pipefd[0]);
    delete[] users;
    delete[] users_timer;
    delete pool;
    return 0;
}
