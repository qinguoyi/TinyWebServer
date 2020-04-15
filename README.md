

TinyWebServer
===============
Linux下C++轻量级Web服务器

* 使用**线程池 + epoll(ET和LT均实现) + Proactor模式**的并发模型，支持解析**GET和POST**请求
* 通过访问服务器数据库实现web端用户**注册、登录**功能，可以请求服务器**图片和视频文件**
* 实现**同步/异步日志系统**,记录服务器运行状态
* 经Webbench压力测试可以实现**上万的并发连接**数据交换

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


Demo
----------
> * 注册演示

<div align=center><img src="https://github.com/qinguoyi/TinyWebServer/blob/master/root/registernew.gif" height="429"/> </div>

> * 登录演示

<div align=center><img src="https://github.com/qinguoyi/TinyWebServer/blob/master/root/loginnew.gif" height="429"/> </div>

> * 请求图片文件演示(6M)

<div align=center><img src="https://github.com/qinguoyi/TinyWebServer/blob/master/root/picture.gif" height="429"/> </div>

> * 请求视频文件演示(39M)

<div align=center><img src="https://github.com/qinguoyi/TinyWebServer/blob/master/root/video.gif" height="429"/> </div>


测试结果
-------------
Webbench对服务器进行压力测试，在ET非阻塞和LT阻塞模式下均可实现上万的并发连接.

> * ET非阻塞

<div align=center><img src="https://github.com/qinguoyi/TinyWebServer/blob/master/root/ET.png" height="201"/> </div>

> * LT阻塞

<div align=center><img src="https://github.com/qinguoyi/TinyWebServer/blob/master/root/LT.png" height="201"/> </div>

> * 并发连接总数：10500
> * 访问服务器时间：5s
> * 所有访问均成功

**注意：** 使用本项目的webbench进行压测时，若报错显示webbench命令找不到，将可执行文件webbench删除后，重新编译即可。


框架
-------------
<div align=center><img src="https://github.com/qinguoyi/TinyWebServer/blob/master/root/frame.jpg" height="765"/> </div>


web端界面
-------------

> * 判断是否注册   
> * 注册
> * 注册失败提示
> * 登录
> * 登录失败提示


<div align=center><img src="https://github.com/qinguoyi/TinyWebServer/blob/master/interface/judge.jpg" height="200"/>         <img src="https://github.com/qinguoyi/TinyWebServer/blob/master/interface/signup.jpg" height="200"/>         <img src="https://github.com/qinguoyi/TinyWebServer/blob/master/interface/signupfail.jpg" height="200"/></div>

<div align=center><img src="https://github.com/qinguoyi/TinyWebServer/blob/master/interface/signin.jpg" height="200"/><img src="https://github.com/qinguoyi/TinyWebServer/blob/master/interface/signinfail.jpg" height="200"/></div>

基础测试
------------
* 服务器测试环境
	* Ubuntu版本16.04
	* MySQL版本5.7.29

* 测试前确认已安装MySQL数据库

    ```C++
    //建立yourdb库
    create database yourdb set utf8;

    //创建user表
    USE yourdb;
    CREATE TABLE user(
        username char(50) NULL,
        passwd char(50) NULL
    )ENGINE=InnoDB;

    //添加数据
    INSERT INTO user(username, passwd) VALUES('name', 'passwd');
    ```

* 修改main.c中的数据库初始化信息

    ```C++
    //root root为服务器数据库的登录名和密码
    connection_pool *connPool=connection_pool::GetInstance("localhost","root","root","yourdb",3306,5);
    ```

* 修改http_conn.cpp中的root路径

    ```C++
    const char* doc_root="/home/qgy/TinyWebServer/root";
    ```

* 生成server

    ```C++
    make server
    ```

* 启动server

    ```C++
    ./server port
    ```

* 浏览器端

    ```C++
    ip:port
    ```

个性化测试
------
* 选择任一**校验方式**，代码中使用同步校验，可以修改为CGI.

- [x] 同步线程数据库校验
	* 关闭main.c中CGISQLPOOL，打开SYNSQL

	   ```C++
		23 #define SYNSQL    //同步数据库校验
		24 //#define CGISQLPOOL  //CGI数据库校验
	    ```

	* 关闭http_conn.cpp中两种CGI，打开SYNSQL
	    
	    ```C++
		7 //同步校验
		8 #define SYNSQL

		10 //CGI多进程使用链接池
		11 //#define CGISQLPOOL

		13 //CGI多进程不用连接池
		14 //#define CGISQL
	    ```

- [ ] CGI多进程数据库校验，不使用连接池
	* 关闭main.c中SYNSQL和CGISQLPOOL

	   ```C++
		23 //#define SYNSQL    //同步数据库校验
		24 //#define CGISQLPOOL  //CGI数据库校验
	    ```

	* 关闭http_conn.cpp中SYNSQL和CGISQLPOOL，打开CGISQL
	    
	    ```C++
		7 //同步校验
		8 //#define SYNSQL

		10 //CGI多进程使用链接池
		11 //#define CGISQLPOOL

		13 //CGI多进程不用连接池
		14 #define CGISQL
	    ```
	
	* 关闭sign.cpp中的CGISQLPOOL，打开CGISQL

	    ```C++
	    12 #define CGISQL    //不使用连接池
		13 //#define CGISQLPOOL  //使用连接池
	    ```
	* 修改sign.cpp中的数据库初始化信息

	    ```C++
	    //root root为服务器数据库的登录名和密码
	    connection_pool *connPool=connection_pool::GetInstance("localhost","root","root","yourdb",3306,5);
	    ```
	* 生成CGISQL.cgi

	    ```C++
	    make CGISQL.cgi
	    ```

- [ ] CGI多进程数据库校验，使用连接池
	* 关闭main.c中SYNSQL，打开CGISQLPOOL

	   ```C++
		23 //#define SYNSQL    //同步数据库校验
		24 #define CGISQLPOOL  //CGI数据库校验
	    ```

	* 关闭http_conn.cpp中SYNSQL和CGISQL，打开CGISQLPOOL
	    
	    ```C++
		7 //同步校验
		8 //#define SYNSQL

		10 //CGI多进程使用链接池
		11 #define CGISQLPOOL

		13 //CGI多进程不用连接池
		14 //#define CGISQL
	    ```
	* 关闭sign.cpp中的CGISQL，打开CGISQLPOOL

	    ```C++
	    12 //#define CGISQL    //不使用连接池
		13 #define CGISQLPOOL  //使用连接池
	    ```
	* 生成CGISQL.cgi

	    ```C++
	    make CGISQL.cgi
	    ```

* 选择任一**I/O复用方式**，代码中使用LT阻塞，可以修改为ET非阻塞.

- [x] LT阻塞
	* 关闭main.c中ET，打开LT
	    
	    ```C++
	    28 //#define ET       //边缘触发非阻塞
	    29 #define LT         //水平触发阻塞
	    ```

- [ ] ET非阻塞
	* 关闭main.c中LT，打开ET
	    
	    ```C++
	    28 #define ET         //边缘触发非阻塞
	    29 //#define LT       //水平触发阻塞
	    ```

* 选择任一**日志方式**，代码中使用同步日志，可以修改为异步写入.

- [x] 同步写入日志
	* 关闭main.c中ASYNLOG，打开同步写入SYNLOG
	    
	    ```C++
	    25 #define SYNLOG //同步写日志
	    26 //#define ASYNLOG   /异步写日志
	    ```

- [ ] 异步写入日志
	* 关闭main.c中SYNLOG，打开异步写入ASYNLOG
	    
	    ```C++
	    25 //#define SYNLOG //同步写日志
	    26 #define ASYNLOG   /异步写日志
	    ```
* 选择数据库访问、I/O复用方式或日志写入方式后，按照前述生成server，启动server，即可进行测试.

更多资料
------------
请关注公众号 **“两猿社”**.
> * **带你丰富互联网相关项目经验，轻松应对校招！！！**
> * **项目模块详细讲解，在公众号内持续更新！！！**

<div align=center><img src="https://github.com/qinguoyi/TinyWebServer/blob/master/root/test1.jpg" height="350"/> </div>

致谢
------------
Linux高性能服务器编程，游双著.
