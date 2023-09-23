#include "config.h"

int main(int argc, char *argv[])
{
    //需要修改的数据库信息,登录名,密码,库名
    //The database information, login name, password, and database name that need to be modified.
    string user = "root";
    string passwd = "root";
    string databasename = "qgydb";

    //命令行解析
    //Command Line Parsing
    Config config;
    config.parse_arg(argc, argv);

    WebServer server;

    //初始化
    //Initialization
    server.init(config.PORT, user, passwd, databasename, config.LOGWrite, 
                config.OPT_LINGER, config.TRIGMode,  config.sql_num,  config.thread_num, 
                config.close_log, config.actor_model);
    

    //日志
    //Log
    server.log_write();

    //数据库
    //Database
    server.sql_pool();

    //线程池
    //Thread Pool
    server.thread_pool();

    //触发模式
    //Trigger mode
    server.trig_mode();

    //监听
    //Listening
    server.eventListen();

    //运行
    //Run
    server.eventLoop();

    return 0;
}
