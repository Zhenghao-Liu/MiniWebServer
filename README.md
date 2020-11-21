MiniWebServer
----------

Linux下C++轻量级Web服务器，助力初学者快速实践网络编程，搭建属于自己的服务器.

* [测试页](http://175.24.234.48:9006)
* 使用 **线程池 + 非阻塞socket + epoll(ET和LT均实现) + 事件处理(Reactor和模拟Proactor均实现)** 的并发模型
* 使用**状态机**解析HTTP请求报文，支持解析**GET和POST**请求
* 访问服务器数据库实现web端用户**注册、登录**功能，可以请求服务器**图片和视频文件**
* 实现**同步/异步日志系统**，记录服务器运行状态
* 经Webbench压力测试可以实现**上万的并发连接**数据交换

框架
----------
![frame](./pic/frame.jpg)

Code目录树
----------
```
.
|-- build.sh
|-- CGImysql
|   |-- README.md
|   |-- sql_connection_pool.cpp
|   `-- sql_connection_pool.h
|-- config.cpp
|-- config.h
|-- http
|   |-- http_conn.cpp
|   |-- http_conn.h
|   `-- README.md
|-- lock
|   |-- locker.h
|   `-- README.md
|-- log
|   |-- block_queue.h
|   |-- log.cpp
|   |-- log.h
|   `-- README.md
|-- main.cpp
|-- makefile
|-- root
|   |-- fans.html
|   |-- favicon.ico
|   |-- judge.html
|   |-- logError.html
|   |-- log.html
|   |-- picture.html
|   |-- README.md
|   |-- registerError.html
|   |-- register.html
|   |-- test1.jpg
|   |-- video.html
|   |-- welcome.html
|   |-- xxx.jpg
|   `-- xxx.mp4
|-- server
|-- show_errno
|   |-- README.md
|   `-- show_errno.h
|-- test_presure
|   |-- README.md
|   `-- webbench-1.5
|       |-- ChangeLog
|       |-- COPYRIGHT
|       |-- debian
|       |   |-- changelog
|       |   |-- control
|       |   |-- copyright
|       |   |-- dirs
|       |   `-- rules
|       |-- Makefile
|       |-- socket.c
|       |-- tags
|       |-- webbench
|       |-- webbench.1
|       |-- webbench.c
|       `-- webbench.o
|-- threadpool
|   |-- half-sync-reactive.bmp
|   |-- README.md
|   `-- threadpool.h
|-- timer
|   |-- lst_timer.cpp
|   |-- lst_timer.h
|   `-- README.md
|-- webserver.cpp
`-- webserver.h
```

运行
----------

* 系统环境
    * Centos 7
    * MYSQL 8.0.22
    * 以上是作者运行的环境，可自行调整

* 浏览器环境
    * Chrome

* 测试前确保已经安装好MYSQL
```MYSQL
// 建立yourdb库
// 这里的yourdb是自定义名字，是后面main中的databasename
CREATE DATABASE yourdb;
USE yourdb;

// 创建user表，表名不更改
CREATE TABLE user(
    username char(50) NULL,
    passwd char(50) NULL
)ENGINE=InnoDB;

// 添加数据，这里作为测试表是否可用，其实也是等于创建了一个用户
INSERT INTO user(username, passwd) VALUES('name', 'passwd');
```

* 修改main.cpp中的数据库初始化信息
```cpp
//MYSQL登录名,密码,DATABASE数据库名
string user = "root";
string passwd = "root";
string databasename = "yourdb";
```

* 编译
```bash
# 方式1
$ sh ./build.sh
# 方式2
$ make server
```

* 运行主程序
```bash
$ ./server
```

* 也可个性化配置参数运行主程序

```bash
$ ./server [-p port] [-l LOGWrite] [-m TRIGMode] [-o OPT_LINGER] [-s sql_num] [-t thread_num] [-c close_log] [-a actor_model]
```

温馨提示:以上参数不是非必须，不用全部使用，根据个人情况搭配选用即可.

* -p，自定义端口号
    * 默认9006
* -l，选择日志写入方式，默认同步写入
    * 0，同步写入
    * 1，异步写入
* -m，listenfd和connfd的模式组合，默认使用LT + LT
    * 0，表示使用LT + LT
    * 1，表示使用LT + ET
    * 2，表示使用ET + LT
    * 3，表示使用ET + ET
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

浏览器运行结果查看
----------
1. 方式1：以个人主机ip和端口的方式
```
主机ip:端口
http://175.24.234.48:9006/
```

2. 方式2：在本机上运行则可直接输入端口
```
端口号
localhost:9006
```

压力测试
----------
* [webbench说明文件](./code/test_presure/README.md)

一些遇到的问题
----------
* [问题归档](./problem_meet.md)

学习自
----------
1. 《Linux高性能服务器编程》游双
2. [TinyWebServer](https://github.com/qinguoyi/TinyWebServer) 已经提交Pull request请求merge
3. 文章
* [小白视角：一文读懂社长的TinyWebServer](https://huixxi.github.io/2020/06/02/%E5%B0%8F%E7%99%BD%E8%A7%86%E8%A7%92%EF%BC%9A%E4%B8%80%E6%96%87%E8%AF%BB%E6%87%82%E7%A4%BE%E9%95%BF%E7%9A%84TinyWebServer/#more)
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
* [最新版Web服务器项目详解 - 12 注册登录](https://mp.weixin.qq.com/s?__biz=MzAxNzU2MzcwMw==&mid=2649274431&idx=4&sn=7595a70f06a79cb7abaebcd939e0cbee&chksm=83ffb167b4883871ce110aeb23e04acf835ef41016517247263a2c3ab6f8e615607858127ea6&token=1686112912&lang=zh_CN#rd)
* [最新版Web服务器项目详解 - 13 踩坑与面试题](https://mp.weixin.qq.com/s?__biz=MzAxNzU2MzcwMw==&mid=2649274431&idx=1&sn=2dd28c92f5d9704a57c001a3d2630b69&chksm=83ffb167b48838715810b27b8f8b9a576023ee5c08a8e5d91df5baf396732de51268d1bf2a4e&token=1686112912&lang=zh_CN#rd)

