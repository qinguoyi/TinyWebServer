/*
 * @Author: your name
 * @Date: 2020-11-21 15:55:51
 * @LastEditTime: 2020-11-21 18:55:54
 * @LastEditors: your name
 * @Description: In User Settings Edit
 * @FilePath: \MiniWebServerd:\my_project\TinyWebServer\config.h
 */
#ifndef CONFIG_H
#define CONFIG_H

#include "webserver.h"

using namespace std;

class Config
{
public:
    Config();
    ~Config(){};

    void parse_arg(int argc, char *argv[]);

    //端口号
    int PORT;

    //日志写入方式
    int LOGWrite;

    //触发组合模式
    int TRIGMode;

    //listenfd触发模式
    int LISTENTrigmode;

    //connfd触发模式
    int CONNTrigmode;

    //优雅关闭链接
    int OPT_LINGER;

    //数据库连接池数量
    int sql_num;

    //线程池内的线程数量
    int thread_num;

    //是否关闭日志
    int close_log;

    //并发模型选择
    int actor_model;
};

#endif