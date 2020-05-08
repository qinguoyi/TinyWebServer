#include "webserver.h"

int main(int argc, char *argv[])
{
    //需要修改的数据库信息,登录名,密码,库名
    string user = "root";
    string passwd = "root";
    string databasename = "qgydb";

    //端口号,默认9006
    int PORT = 9006;

    //数据库校验方式，默认同步
    int SQLVerify = 0;

    //日志写入方式，默认同步
    int LOGWrite = 0;

    //触发模式，默认LT
    int TRIGMode = 0;

    //优雅关闭链接，默认不使用
    int OPT_LINGER = 0;

    //数据库连接池数量,默认8
    int sql_num = 8;

    //线程池内的线程数量,默认8
    int thread_num = 8;

    int opt;
    const char *str = "p:v:l:m:o:s:t:";
    while ((opt = getopt(argc, argv, str)) != -1)
    {
        switch (opt)
        {
        case 'p':
        {
            PORT = atoi(optarg);
            break;
        }
        case 'v':
        {
            SQLVerify = atoi(optarg);
            break;
        }
        case 'l':
        {
            LOGWrite = atoi(optarg);
            break;
        }
        case 'm':
        {
            TRIGMode = atoi(optarg);
            break;
        }
        case 'o':
        {
            OPT_LINGER = atoi(optarg);
            break;
        }
        case 's':
        {
            sql_num = atoi(optarg);
            break;
        }
        case 't':
        {
            thread_num = atoi(optarg);;
            break;
        }
        default:
            break;
        }
    }

    WebServer server;

    //初始化
    server.init(PORT, user, passwd, databasename, LOGWrite, SQLVerify,
                OPT_LINGER, TRIGMode, sql_num, thread_num);
    //日志
    server.log_write();
    //数据库
    server.sql_pool();
    //线程池
    server.thread_pool();

    //服务器
    server.eventListen();
    //定时器
    server.timer();

    server.eventLoop();

    return 0;
}
