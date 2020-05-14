

TinyWebServer
===============
Linux下C++轻量级Web服务器，助力初学者快速实践网络编程，搭建属于自己的服务器.

* 使用 **线程池 + epoll(ET和LT均实现) + 反应堆(Reactor和Proactor均实现)** 的并发模型
* 使用**状态机**解析HTTP请求报文，支持解析**GET和POST**请求
* 访问服务器数据库实现web端用户**注册、登录**功能，可以请求服务器**图片和视频文件**
* 实现**同步/异步日志系统**，记录服务器运行状态
* 经Webbench压力测试可以实现**上万的并发连接**数据交换

目录
-----

| [概述](#概述) | [框架](#框架) | [Demo演示](#Demo演示) | [压力测试](#压力测试) |[更新与下载](#更新与下载) | [快速运行](#快速运行) | [个性化运行](#个性化运行) | [庖丁解牛](#庖丁解牛) | [致谢](#致谢) |
|:--------:|:--------:|:--------:|:--------:|:--------:|:--------:|:--------:|:--------:|:--------:|


概述
----------

> * C/C++
> * B/S模型
> * [线程同步机制包装类](https://github.com/qinguoyi/TinyWebServer/tree/master/lock)
> * [http连接请求处理类](https://github.com/qinguoyi/TinyWebServer/tree/master/http)
> * [半同步/半反应堆线程池](https://github.com/qinguoyi/TinyWebServer/tree/master/threadpool)
> * [定时器处理非活动连接](https://github.com/qinguoyi/TinyWebServer/tree/master/timer)
> * [同步/异步日志系统 ](https://github.com/qinguoyi/TinyWebServer/tree/master/log)  
> * [数据库连接池](https://github.com/qinguoyi/TinyWebServer/tree/master/CGImysql) 
> * [CGI及同步线程注册和登录校验](https://github.com/qinguoyi/TinyWebServer/tree/master/CGImysql) 
> * [简易服务器压力测试](https://github.com/qinguoyi/TinyWebServer/tree/master/test_presure)


框架
-------------
<div align=center><img src="http://ww1.sinaimg.cn/large/005TJ2c7ly1ge0j1atq5hj30g60lm0w4.jpg" height="765"/> </div>

Demo演示
----------
> * 注册演示

<div align=center><img src="http://ww1.sinaimg.cn/large/005TJ2c7ly1ge0iz0dkleg30m80bxjyj.gif" height="429"/> </div>

> * 登录演示

<div align=center><img src="http://ww1.sinaimg.cn/large/005TJ2c7ly1ge0izcc0r1g30m80bxn6a.gif" height="429"/> </div>

> * 请求图片文件演示(6M)

<div align=center><img src="http://ww1.sinaimg.cn/large/005TJ2c7ly1ge0juxrnlfg30go07x4qr.gif" height="429"/> </div>

> * 请求视频文件演示(39M)

<div align=center><img src="http://ww1.sinaimg.cn/large/005TJ2c7ly1ge0jtxie8ng30go07xb2b.gif" height="429"/> </div>


压力测试
-------------
在关闭日志后，使用Webbench对服务器进行压力测试，在ET非阻塞和LT阻塞模式下均可实现上万的并发连接. 

> * Proactor，LT阻塞，84016 QPS

<div align=center><img src="http://ww1.sinaimg.cn/large/005TJ2c7ly1geotkkw531j30f906tn0f.jpg" height="201"/> </div>

> * Proactor，ET非阻塞，83419 QPS

<div align=center><img src="http://ww1.sinaimg.cn/large/005TJ2c7ly1geotkozc4nj30f806p0vu.jpg" height="201"/> </div>

> * Reactor，LT阻塞，60218 QPS

<div align=center><img src="http://ww1.sinaimg.cn/large/005TJ2c7ly1geotkad1v2j30f906rad8.jpg" height="201"/> </div>

> * Reactor，ET非阻塞，58138 QPS

<div align=center><img src="http://ww1.sinaimg.cn/large/005TJ2c7ly1geotk12vwxj30fa06t0w3.jpg" height="201"/> </div>

> * 并发连接总数：10500
> * 访问服务器时间：5s
> * 所有访问均成功

**注意：** 使用本项目的webbench进行压测时，若报错显示webbench命令找不到，将可执行文件webbench删除后，重新编译即可。

更新与下载
-------
- [x] 解决请求服务器上大文件的Bug
- [x] 增加请求视频文件的页面
- [x] 解决数据库同步校验内存泄漏
- [x] 实现两种CGI数据库访问逻辑
- [x] 实现ET非阻塞和LT阻塞，并完成压力测试
- [x] 完善`lock.h`中的封装类，统一使用该同步机制
- [x] 改进代码结构，更新局部变量懒汉单例模式
- [x] 优化数据库连接池信号量与代码结构
- [x] 使用RAII机制优化数据库连接的获取与释放
- [x] 优化代码结构，封装工具类以减少全局变量
- [x] 编译一次即可，命令行进行个性化测试更加友好
- [x] main函数封装重构
- [x] 新增命令行日志开关，关闭日志后更新压力测试结果
- [x] 改进编译方式，只配置一次SQL信息即可
- [x] 新增Reactor模式，并完成压力测试

目前有两个版本，版本间的代码结构有较大改动，文档和代码运行方法也不一致. 重构版本更简洁，原始版本(raw_version)更大保留游双代码的原汁原味，从原始版本更容易入手.

如果遇到github代码下载失败，或访问太慢，可以从以下链接下载，与Github最新提交同步.

* 重构版本下载地址 : [BaiduYun](https://pan.baidu.com/s/1Z-W7clT-HpdLysNRzO4CPg)
    * 提取码 : 18kv
* 原始版本(raw_version)下载地址 : [BaiduYun](https://pan.baidu.com/s/1a-UfktwKrCMc73UvWGESMw)
    * 提取码 : i9xt
    * [原始版本文档](https://github.com/qinguoyi/TinyWebServer/tree/master/raw_version)

快速运行
------------
* 服务器测试环境
	* Ubuntu版本16.04
	* MySQL版本5.7.29
* 浏览器测试环境
	* Windows、Linux均可
	* Chrome
	* FireFox
	* 其他浏览器暂无测试

* 测试前确认已安装MySQL数据库

    ```C++
    // 建立yourdb库
    create database yourdb set utf8;

    // 创建user表
    USE yourdb;
    CREATE TABLE user(
        username char(50) NULL,
        passwd char(50) NULL
    )ENGINE=InnoDB;

    // 添加数据
    INSERT INTO user(username, passwd) VALUES('name', 'passwd');
    ```

* 修改main.cpp中的数据库初始化信息

    ```C++
    //数据库登录名,密码,库名
    string user = "root";
    string passwd = "root";
    string databasename = "yourdb";
    ```

* build

    ```C++
    sh ./build.sh
    ```

* 启动server

    ```C++
    ./server
    ```

* 浏览器端

    ```C++
    ip:9006
    ```

个性化运行
------

```C++
./server [-p port] [-v SQLVerify] [-l LOGWrite] [-m TRIGMode] [-o OPT_LINGER] [-s sql_num] [-t thread_num] [-c close_log] [-a actor_model]
```

温馨提示:以上参数不是非必须，不用全部使用，根据个人情况搭配选用即可.

* -p，自定义端口号
	* 默认9006
* -v，选择数据库校验方式，默认同步校验
	* 0，同步校验，使用连接池
	* 1，CGI校验，使用连接池
	* 2，CGI校验，不使用连接池
* -l，选择日志写入方式，默认同步写入
	* 0，同步写入
	* 1，异步写入
* -m，epoll的触发模式，默认使用LT
	* 0，表示使用LT
	* 1，表示使用ET
* -o，优雅关闭连接，默认不使用
	* 0，不使用
	* 1，使用
* -s，数据库连接数量
	* 默认为8
* -t，线程数量
	* 默认为8
* -c，关闭日志，默认打开
	* 0，打开日志
	* 1，关闭日志
* -a，选择反应堆模型，默认Proactor
	* 0，Proactor模型
	* 1，Reactor模型

测试示例命令与含义

```C++
./server -p 9007 -v 1 -l 1 -m 0 -o 1 -s 10 -t 10 -c 1 -a 1
```

- [x] 端口9007
- [x] 同步数据库校验，使用连接池
- [x] 异步写入日志
- [x] 使用LT水平触发
- [x] 使用优雅关闭连接
- [x] 数据库连接池内有10条连接
- [x] 线程池内有10条线程
- [x] 关闭日志
- [x] Reactor反应堆模型

庖丁解牛
------------
近期版本迭代较快，以下内容多以旧版本(raw_version)代码为蓝本进行详解，重构版本讲解将于下月重制发布，感谢理解.

* [最新版Web服务器项目详解 - 01 线程同步机制封装类](https://mp.weixin.qq.com/s?__biz=MzAxNzU2MzcwMw==&mid=2649274278&idx=3&sn=5840ff698e3f963c7855d702e842ec47&chksm=83ffbefeb48837e86fed9754986bca6db364a6fe2e2923549a378e8e5dec6e3cf732cdb198e2&scene=0&xtrack=1#rd)
* [最新版Web服务器项目详解 - 02 半同步半反应堆线程池（上）](https://mp.weixin.qq.com/s?__biz=MzAxNzU2MzcwMw==&mid=2649274278&idx=4&sn=caa323faf0c51d882453c0e0c6a62282&chksm=83ffbefeb48837e841a6dbff292217475d9075e91cbe14042ad6e55b87437dcd01e6d9219e7d&scene=0&xtrack=1#rd)
* [最新版Web服务器项目详解 - 03 半同步半反应堆线程池（下）](https://mp.weixin.qq.com/s/PB8vMwi8sB4Jw3WzAKpWOQ)
* [最新版Web服务器项目详解 - 04 http连接处理（上）](https://mp.weixin.qq.com/s/BfnNl-3jc_x5WPrWEJGdzQ)
* [最新版Web服务器项目详解 - 05 http连接处理（中）](https://mp.weixin.qq.com/s/wAQHU-QZiRt1VACMZZjNlw)
* [最新版Web服务器项目详解 - 06 http连接处理（下）](https://mp.weixin.qq.com/s/451xNaSFHxcxfKlPBV3OCg)
* [最新版Web服务器项目详解 - 07 定时器处理非活动连接（上）](https://mp.weixin.qq.com/s/mmXLqh_NywhBXJvI45hchA)
* [最新版Web服务器项目详解 - 08 定时器处理非活动连接（下）](https://mp.weixin.qq.com/s/fb_OUnlV1SGuOUdrGrzVgg)
* [最新版Web服务器项目详解 - 09 日志系统（上）](https://mp.weixin.qq.com/s/IWAlPzVDkR2ZRI5iirEfCg)
* [最新版Web服务器项目详解 - 10 日志系统（下）](https://mp.weixin.qq.com/s/f-ujwFyCe1LZa3EB561ehA)
* [最新版Web服务器项目详解 - 11 数据库连接池](https://mp.weixin.qq.com/s?__biz=MzAxNzU2MzcwMw==&mid=2649274326&idx=1&sn=5af78e2bf6552c46ae9ab2aa22faf839&chksm=83ffbe8eb4883798c3abb82ddd124c8100a39ef41ab8d04abe42d344067d5e1ac1b0cac9d9a3&token=1450918099&lang=zh_CN#rd)
* 持续更新ing...

<div align=center><img src="http://ww1.sinaimg.cn/large/005TJ2c7ly1ge0j3jrepyj30ij0cvjt8.jpg" height="350"/> </div>

致谢
------------
Linux高性能服务器编程，游双著.

感谢以下朋友的PR和帮助: [RownH](https://github.com/RownH)，[ZWiley](https://github.com/ZWiley)，[zjuHong](https://github.com/zjuHong)，[mamil](https://github.com/mamil)，[byfate](https://github.com/byfate)，[MaJun827](https://github.com/MaJun827)，[BBLiu-coder](https://github.com/BBLiu-coder).
