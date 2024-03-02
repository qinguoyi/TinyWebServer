#include "time_wheel_timer.h"
#include "../http/http_conn.h"

time_wheel::time_wheel() : cur_slot(0)
{
    for(int i = 0; i < N; ++i)
    {
        slots[i] = NULL;
    }
}
time_wheel::~time_wheel()
{
    for(int i = 0; i < N; ++i)
    {
        util_timer *tmp = slots[i];
        while (tmp)
        {
            slots[i] = tmp->next;
            delete tmp;
            tmp = slots[i];
        }
    }
    
}

void time_wheel::add_timer(util_timer *timer)
{
    if (!timer)
    {
        return;
    }
    switch_time(timer);
    int ts = timer->time_slot;
    if(!slots[ts])
    {
        slots[ts] = timer;
    }
    else
    {
        timer->next = slots[ts];
        slots[ts]->prev = timer;
        slots[ts] = timer;
    }
}

void time_wheel::adjust_timer(util_timer *timer)
{
    if (!timer)
    {
        return;
    }
    int ots = timer->time_slot;
    switch_time(timer);
    if(timer->time_slot == ots)
    {
        return;
    }
    if(slots[ots] == timer)
    {
        slots[ots] = timer->next;
        if(slots[ots])
        {
            slots[ots]->prev = NULL;
        }  
    }
    else
    {
        timer->prev->next = timer->next;
        if(timer->next)
        {
            timer->next->prev = timer->prev;
        }   
        timer->prev = NULL;
    }
    timer->next = NULL;
    add_timer(timer);
}
void time_wheel::del_timer(util_timer *timer)
{
    if (!timer)
    {
        return;
    }
    int ts = timer->time_slot;
    if(timer == slots[ts])
    {
        slots[ts] = slots[ts]->next;
        if(slots[ts])
        {
            slots[ts]->prev = NULL;
        }
    }
    else
    {
        timer->prev->next = timer->next;
        if(timer->next)
        {
            timer->next->prev = timer->prev;
        } 
    }
    delete timer;
}
void time_wheel::tick()
{
    
    for(int i = 0; i < timeslot / SI; i++)
    {
        util_timer *tmp = slots[cur_slot];
        while(tmp)
        {
            if(tmp->rotation > 0)
            {
                tmp->rotation--;
                tmp = tmp->next;
            }
            else
            {
                tmp->cb_func(tmp->user_data);
                util_timer *tmp2 = tmp->next;
                del_timer(tmp);
                tmp = tmp2;
            }
        }
        cur_slot = ++cur_slot % N;
    }
}

void time_wheel::set_timeslot(int timeslot)
{
    this->timeslot = timeslot;
}

void time_wheel::switch_time(util_timer *timer)
{
    time_t cur = time(NULL);
    int timeout = timer->expire - cur;
    if(timeout < 0)timeout = 0;
    timeout /= SI;
    timer->rotation = timeout / N;
    timer->time_slot = (cur_slot + (timeout % N)) % N;
}

void Utils::init(int timeslot)
{
    m_TIMESLOT = timeslot;
    m_timer_lst.set_timeslot(timeslot);
}

//对文件描述符设置非阻塞
int Utils::setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

//将内核事件表注册读事件，ET模式，选择开启EPOLLONESHOT
void Utils::addfd(int epollfd, int fd, bool one_shot, int TRIGMode)
{
    epoll_event event;
    event.data.fd = fd;

    if (1 == TRIGMode)
        event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    else
        event.events = EPOLLIN | EPOLLRDHUP;

    if (one_shot)
        event.events |= EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

//信号处理函数
void Utils::sig_handler(int sig)
{
    //为保证函数的可重入性，保留原来的errno
    int save_errno = errno;
    int msg = sig;
    send(u_pipefd[1], (char *)&msg, 1, 0);
    errno = save_errno;
}

//设置信号函数
void Utils::addsig(int sig, void(handler)(int), bool restart)
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = handler;
    if (restart)
        sa.sa_flags |= SA_RESTART;
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != -1);
}

//定时处理任务，重新定时以不断触发SIGALRM信号
void Utils::timer_handler()
{
    m_timer_lst.tick();
    alarm(m_TIMESLOT);
}

void Utils::show_error(int connfd, const char *info)
{
    send(connfd, info, strlen(info), 0);
    close(connfd);
}

int *Utils::u_pipefd = 0;
int Utils::u_epollfd = 0;

class Utils;
void cb_func(client_data *user_data)
{
    epoll_ctl(Utils::u_epollfd, EPOLL_CTL_DEL, user_data->sockfd, 0);
    assert(user_data);
    close(user_data->sockfd);
    http_conn::m_user_count--;
}
