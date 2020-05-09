

TinyWebServer
===============
Linux下C++轻量级Web服务器，助力初学者快速实践网络编程，搭建属于自己的服务器.

* 使用**线程池 + epoll(ET和LT均实现) + 模拟Proactor模式**的并发模型
* 使用**状态机**解析HTTP请求报文，支持解析**GET和POST**请求
* 访问服务器数据库实现web端用户**注册、登录**功能，可以请求服务器**图片和视频文件**
* 实现**同步/异步日志系统**，记录服务器运行状态
* 经Webbench压力测试可以实现**上万的并发连接**数据交换

Download
-------
代码下载经常失败，访问太慢，无需科学上网，与Github最新提交同步.

* 重构版本下载地址 : [BaiduYun](https://pan.baidu.com/s/1Xzp4NfbVYoaJWZuxJc5u1g)
* 提取码 : wcbj
* raw_version下载地址 : [BaiduYun](https://pan.baidu.com/s/1f9dkc5t-joh_D40wif4_6g)
* 提取码 : pybt

模块概述
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

Update
----------
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


基础测试
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

* 修改main.c中的数据库初始化信息

    ```C++
    //数据库登录名,密码,库名
    string user = "root";
    string passwd = "root";
    string databasename = "qgydb";
    ```

* 生成server

    ```C++
    make server
    ```

* 启动server

    ```C++
    ./server
    ```

* 浏览器端

    ```C++
    ip:9006
    ```

个性化测试
------

```C++
./server [-p port] [-v SQLVerify] [-l LOGWrite] [-m TRIGMode] [-o OPT_LINGER] [-s sql_num] [-t thread_num]
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

若使用CGI数据库校验方式，按如下编译代码，若使用同步校验，则跳过下面修改与生成CGI步骤.

* 修改sign.cpp中的数据库初始化信息

    ```C++
    // root root修改为服务器数据库的登录名和密码
	// qgydb修改为上述创建的yourdb库名
    con = mysql_real_connect(con, "localhost", "root", "root", "qgydb", 3306, NULL, 0);
    ```

* 生成CGISQL.cgi

    ```C++
    make CGISQL.cgi
    ```

测试示例命令与含义

```C++
./server -p 9007 -v 1 -l 1 -m 0 -o 1 -s 10 -t 10
```

- [x] 端口9007
- [x] 同步数据库校验，使用连接池
- [x] 异步写入日志
- [x] 使用LT水平触发
- [x] 使用优雅关闭连接
- [x] 数据库连接池内有10条连接
- [x] 线程池内有10条线程


Demo
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
Webbench对服务器进行压力测试，在ET非阻塞和LT阻塞模式下均可实现上万的并发连接. 这里以同步日志，不使用优雅关闭连接为例.

> * ET非阻塞，18657 QPS

<div align=center><img src="http://ww1.sinaimg.cn/large/005TJ2c7ly1gejjt9plmfj30fm05fgnt.jpg" height="201"/> </div>

> * LT阻塞，22868 QPS

<div align=center><img src="http://ww1.sinaimg.cn/large/005TJ2c7ly1gejjtn0judj30fj05htay.jpg" height="201"/> </div>

> * 并发连接总数：10500
> * 访问服务器时间：10s
> * 所有访问均成功

**注意：** 使用本项目的webbench进行压测时，若报错显示webbench命令找不到，将可执行文件webbench删除后，重新编译即可。


框架
-------------
<div align=center><img src="http://ww1.sinaimg.cn/large/005TJ2c7ly1ge0j1atq5hj30g60lm0w4.jpg" height="765"/> </div>


web端界面
-------------

> * 判断是否注册   
> * 注册
> * 注册失败提示
> * 登录
> * 登录失败提示


<div align=center><img src="http://ww1.sinaimg.cn/large/005TJ2c7ly1ge0j1qt5w7j306i06w746.jpg" height="200"/>         <img src="http://ww1.sinaimg.cn/large/005TJ2c7ly1ge0j21iwi6j306306i3ye.jpg" height="200"/>         <img src="http://ww1.sinaimg.cn/large/005TJ2c7ly1ge0j2b44jgj306v075q2w.jpg" height="200"/></div>

<div align=center><img src="http://ww1.sinaimg.cn/large/005TJ2c7ly1ge0j2r5rnfj306j06ewed.jpg" height="200"/><img src="http://ww1.sinaimg.cn/large/005TJ2c7ly1ge0j30o0wpj308n07cq2y.jpg" height="200"/></div>


庖丁解牛
------------
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
* 持续更新ing...

<div align=center><img src="http://ww1.sinaimg.cn/large/005TJ2c7ly1ge0j3jrepyj30ij0cvjt8.jpg" height="350"/> </div>

致谢
------------
Linux高性能服务器编程，游双著.

感谢以下朋友的PR和帮助: [RownH](https://github.com/RownH)，[ZWiley](https://github.com/ZWiley)，[zjuHong](https://github.com/zjuHong)，[mamil](https://github.com/mamil)，[byfate](https://github.com/byfate).
