#include "config.h"

Config::Config(){
    //端口号,默认9006
    PORT = 9006;

    //数据库校验方式，默认同步
    SQLVerify = 0;

    //日志写入方式，默认同步
    LOGWrite = 0;

    //触发模式，默认LT
    TRIGMode = 0;

    //优雅关闭链接，默认不使用
    OPT_LINGER = 0;

    //数据库连接池数量,默认8
    sql_num = 8;

    //线程池内的线程数量,默认8
    thread_num = 8;

    //关闭日志,默认不关闭
    close_log = 0;
}

void Config::parse_arg(int argc, char*argv[]){
    int opt;
    const char *str = "p:v:l:m:o:s:t:c:";
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
            thread_num = atoi(optarg);
            break;
        }
        case 'c':
        {
            close_log = atoi(optarg);
            break;
        }
        default:
            break;
        }
    }
}