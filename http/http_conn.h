#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H
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
#include <map>

#include "../lock/locker.h"
#include "../CGImysql/sql_connection_pool.h"
#include "../timer/lst_timer.h"
#include "../log/log.h"

/*
 * 线程池的工作类型类
 * FILENAME_LEN 文件名的最大长度
   READ_BUFFER_SIZE 度缓冲区的大小
   WRITE_BUFFER_SIZE 写缓冲区的大小
   METHOD HTTP请求方法
       GET 申请获取资源
       POST 客户端向服务器提交数据的方法
       HEAD 要求服务器返回头部信息
       PUT 上传某个资源
       DELETE 删除某个资源
       TRACE 要求目标服务器返回原始HTTP请求的内容
       OPTIONS 查看服务器对某个特定URL都支持哪些请求方法
       CONNECT 用于某些代理服务器，能把请求的连接转化为一个安全隧道
       PATCH 对某个资源做部分修改
   CHECK_STATE 解析客户请求时，主状态机所处的状态
       CHECK_STATE_REQUESTLINE 分析请求行
       CHECK_STATE_HEADER 分析头部域
       CHECK_STATE_CONTENT 分析消息体
   HTTP_CODE 服务器处理HTTP请求可能的结果
       NO_REQUEST 请求不完整，需要继续读取数据
       GET_REQUEST 获得了一个完整的客户请求
       BAD_REQUEST 客户请求有语法错误
       NO_RESOURCE 没有对应的资源
       FORBIDDEN_REQUEST 客户对资源没有足够访问权限
       FILE_REQUEST 客户请求是一个可读的文件
       INTERAL_ERROR 服务器内部错误
       CLOSED_CONNECTION 客户端已经关闭连接
   LINE_STATUS 行的读取状态
       LINE_OK 读取到一个完整的行
       LINE_BAD 行出错
       LINE_OPEN 行数据尚且不完整
 * init 初始化接受新的连接
   close_conn 关闭连接
   process 处理客户请求
   read_once 循环读取客户数据，知道无数据刻度或对方关闭连接
   write 非阻塞写入数据
   get_address 返回服务器端通信的socket地址
   initmysql_result 初始化mysql连接，取出数据库保存的账号和密码，存入map中
   timer_flag 是否开启了定时器
   improv
 * init 初始化连接
   process_read 解析HTTP请求
   以下一组函数被process_read调用以分析HTTP请求
       parse_request_line 解析http请求行，获得请求方法，目标url及http版本号
       parse_headers 解析http请求的一个头部信息
       parse_content 判断http请求是否被完整读入，读完消息体，这里实质上没有真正解析消息体
       do_request 当得到一个完整、正确的HTTP请求时，分析用户的请求内容：登陆、注册、获取文件
       get_line 移动指针读取一行
       parse_line 从状态机，用于分析出一行内容
   process_write 填充HTTP应答
   以下一组函数被process_write调用用以填充HTTP应答
       unmap 释放共享内存映射区
       add_response 往HTTP应答中写入待发送的数据
       add_content 往HTTP应答写入消息体
       add_status_line 往HTTP应答写入状态行
       add_content_type 往HTTP应答写入头部字段的目标文档的MIME类型
       add_content_length 往HTTP应答写入头部字段的目标文档的长度
       add_linger 往HTTP应答写入头部字段的此次通信后是否关闭TCP连接
       add_blank_line 往HTTP应答写入一个空行 \r\n
 * m_pollfd epoll的事件表文件描述符
   m_user_count 统计用户数量
   mysql 连接的数据库
   m_state 读为0，写为1
 * m_sockfd HTTP连接的socket文件描述符
   m_address HTTP对方的socket地址
   m_read_buf 读缓冲区
   m_read_idx 表示读缓冲中已经读入的客户数据的最后一个字节的下一个位置
   m_check_idx 当前正在分析的字符在读缓冲区中的位置
   m_start_line 当前正在解析的行的起始位置
   m_write_buf 写缓冲区
   m_write_idx 写缓冲区中待发送的字节数
   m_check_state 主状态机当前所处的状态
   m_method 请求方法
   m_real_file 客户请求的目标文件的完整路径，其内容等于网站根目录doc_root+m_url
   m_url 客户请求的目标文件的文件名
   m_version HTTP 协议版本号，此程序只支持HTTP/1.1
   m_host 主机名
   m_content_length HTTP请求的消息体的长度
   m_linger HTTP其你去是否要求保持连接
   m_file_address 客户请求的目标文件被mmap到内存中的起始位置
   m_file_stat 目标文件的状态：是否存在、是否为目录、是否刻度、文件信息
   m_iv 采用writev写操作的内存分块
   m_iv_count 采用writev写操作的内存分块数量
   cgi 是否启用新的POST
   m_string 存储请求头数据
   bytes_to_send 需要发送的字节数
   bytes_have_send 已经发送的字节数
   doc_root 服务器端网站根目录
   m_users 客户账号和密码
   m_TRIGMode 是否采用ET工作模式
   m_close_log 是否关闭日志功能
   sql_uesr MYSQL用户名
   sql_passwd MYSQL密码
   sql_name MYSQL数据库名
 */
class http_conn
{
public:
    static const int FILENAME_LEN = 200;
    static const int READ_BUFFER_SIZE = 2048;
    static const int WRITE_BUFFER_SIZE = 1024;
    enum METHOD
    {
        GET = 0,
        POST,
        HEAD,
        PUT,
        DELETE,
        TRACE,
        OPTIONS,
        CONNECT,
        PATCH
    };
    enum CHECK_STATE
    {
        CHECK_STATE_REQUESTLINE = 0,
        CHECK_STATE_HEADER,
        CHECK_STATE_CONTENT
    };
    enum HTTP_CODE
    {
        NO_REQUEST,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURCE,
        FORBIDDEN_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION
    };
    enum LINE_STATUS
    {
        LINE_OK = 0,
        LINE_BAD,
        LINE_OPEN
    };

public:
    http_conn() {}
    ~http_conn() {}

public:
    void init(int sockfd, const sockaddr_in &addr, char *, int, int, string user, string passwd, string sqlname);
    void close_conn(bool real_close = true);
    void process();
    bool read_once();
    bool write();
    sockaddr_in *get_address()
    {
        return &m_address;
    }
    void initmysql_result(connection_pool *connPool);
    int timer_flag;
    int improv;

private:
    void init();
    HTTP_CODE process_read();
    bool process_write(HTTP_CODE ret);
    HTTP_CODE parse_request_line(char *text);
    HTTP_CODE parse_headers(char *text);
    HTTP_CODE parse_content(char *text);
    HTTP_CODE do_request();
    char *get_line() { return m_read_buf + m_start_line; };
    LINE_STATUS parse_line();
    void unmap();
    bool add_response(const char *format, ...);
    bool add_content(const char *content);
    bool add_status_line(int status, const char *title);
    bool add_headers(int content_length);
    bool add_content_type();
    bool add_content_length(int content_length);
    bool add_linger();
    bool add_blank_line();

public:
    static int m_epollfd;
    static int m_user_count;
    MYSQL *mysql;
    int m_state; //读为0, 写为1

private:
    int m_sockfd;
    sockaddr_in m_address;
    char m_read_buf[READ_BUFFER_SIZE];
    int m_read_idx;
    int m_checked_idx;
    int m_start_line;
    char m_write_buf[WRITE_BUFFER_SIZE];
    int m_write_idx;
    CHECK_STATE m_check_state;
    METHOD m_method;
    char m_real_file[FILENAME_LEN];
    char *m_url;
    char *m_version;
    char *m_host;
    int m_content_length;
    bool m_linger;
    char *m_file_address;
    struct stat m_file_stat;
    struct iovec m_iv[2];
    int m_iv_count;
    int cgi;        //是否启用的POST
    char *m_string; //存储请求头数据
    int bytes_to_send;
    int bytes_have_send;
    char *doc_root;

    map<string, string> m_users;
    int m_TRIGMode;
    int m_close_log;

    char sql_user[100];
    char sql_passwd[100];
    char sql_name[100];
};

#endif
