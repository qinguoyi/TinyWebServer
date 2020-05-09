

Raw_version文档
===============
Linux下C++轻量级Web服务器，助力初学者快速实践网络编程，搭建属于自己的服务器.

* 使用**线程池 + epoll(ET和LT均实现) + 模拟Proactor模式**的并发模型
* 使用**状态机**解析HTTP请求报文，支持解析**GET和POST**请求
* 通过访问服务器数据库实现web端用户**注册、登录**功能，可以请求服务器**图片和视频文件**
* 实现**同步/异步日志系统**，记录服务器运行状态
* 经Webbench压力测试可以实现**上万的并发连接**数据交换

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
    // root root修改为服务器数据库的登录名和密码
	// qgydb修改为上述创建的yourdb库名
    connPool->init("localhost", "root", "root", "qgydb", 3306, 8);
    ```

* 修改http_conn.cpp中的root路径

    ```C++
	// 修改为root文件夹所在路径
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

> * 校验方式，代码中使用同步校验，可以修改为CGI.

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


> * I/O复用方式，代码中使用LT阻塞，可以修改为ET非阻塞.

- [x] LT阻塞
	* 关闭main.c中ET，打开LT
	    
	    ```C++
	    28 //#define ET       //边缘触发非阻塞
	    29 #define LT         //水平触发阻塞
	    ```
	
	* 关闭http_conn.cpp中ET，打开LT
	    
	    ```C++
	    16 //#define ET       //边缘触发非阻塞
	    17 #define LT         //水平触发阻塞
	    ```

- [ ] ET非阻塞
	* 关闭main.c中LT，打开ET
	    
	    ```C++
	    28 #define ET         //边缘触发非阻塞
	    29 //#define LT       //水平触发阻塞
	    ```

	* 关闭http_conn.cpp中LT，打开ET
	    
	    ```C++
	    16 #define ET       //边缘触发非阻塞
	    17 //#define LT         //水平触发阻塞
	    ```

> * 日志写入方式，代码中使用同步日志，可以修改为异步写入.

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

反馈
------------
请关注公众号 **“两猿社”**，**项目模块详细讲解，在公众号内持续更新.**

<div align=center><img src="http://ww1.sinaimg.cn/large/005TJ2c7ly1ge0j3jrepyj30ij0cvjt8.jpg" height="350"/> </div>

致谢
------------
Linux高性能服务器编程，游双著.