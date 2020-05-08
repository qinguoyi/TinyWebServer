#include "config.h"

int main(int argc, char *argv[])
{
    //需要修改的数据库信息,登录名,密码,库名
    string user = "root";
    string passwd = "root";
    string databasename = "qgydb";

    //命令行解析
    Config config;
    config.parse_arg(argc, argv);

    WebServer server;

    //初始化
    server.init(config.PORT, user, passwd, databasename, config.LOGWrite, config.SQLVerify,
                config.OPT_LINGER, config.TRIGMode,  config.sql_num,  config.thread_num);
    
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
