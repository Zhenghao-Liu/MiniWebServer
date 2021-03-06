﻿/*
 * @Author: Zhenghao-Liu
 * @Date: 2020-11-18 11:06:18
 * @LastEditTime: 2020-11-18 15:08:32
 * @LastEditors: Please set LastEditors
 * @Description: 校验&数据库连接池头文件
 * @FilePath: \MiniWebServer\code\CGImysql\sql_connection_pool.h
 */

#ifndef _CONNECTION_POOL_
#define _CONNECTION_POOL_

#include <stdio.h>
#include <list>
#include <mysql/mysql.h>
#include <error.h>
#include <string.h>
#include <iostream>
#include <string>
#include "../lock/locker.h"
#include "../log/log.h"
using namespace std;

/*
 * m_MaxConn 最大连接数
   m_CurConn 当前已使用的连接数
   m_FreeConn 当前空闲的连接数
   connList 连接池
 * m_url 主机地址
   m_Port 数据库端口号
   m_User 登陆数据库用户名
   m_PassWord 登陆数据库密码
   m_DatabaseName 使用数据库名
   int m_close_log 日志开关
 * 单例模式：将构造写入private，通过指针进行实例化，这样保证只能实例化一次
 * 懒汉模式：需要用到创建实例了程序再去创建实例
 * GetConnection 获取数据库连接，从连接池中返回一个可用连接
   ReleaseConnection 回收连接，将连接放回连接池中
   GetFreeConn 获取当前连接池中空闲(可用)的连接数
   DestroyPool 释放连接池，即释放所有连接
   GetInstance 懒汉模式+单例模式
   init 实例化后真正的构造函数
 */
class connection_pool
{
private:
    int m_MaxConn;
    int m_CurConn;
    int m_FreeConn;
    locker lock;
    list<MYSQL *> connList;
    sem reserve;
    connection_pool();
    ~connection_pool();

public:
    string m_url;
    string m_Port;
    string m_User;
    string m_PassWord;
    string m_DatabaseName;
    int m_close_log;
    MYSQL *GetConnection();
    bool ReleaseConnection(MYSQL *conn);
    int GetFreeConn();
    void DestroyPool();
    static connection_pool *GetInstance();
    void init(string url, string User, string PassWord, string DataBaseName, int Port, int MaxConn, int close_log);
};

/*
 * 采用RAII封装数据库连接池
 * RAII：是使用一个对象，在其构造时获取资源，在对象生命期控制对资源的访问使之始终保持有效，最后在对象析构的时候释放资源
 */
class connectionRAII
{
private:
    MYSQL *conRAII;
    connection_pool *poolRAII;

public:
    connectionRAII(MYSQL * *con, connection_pool * connPool);
    ~connectionRAII();
};

#endif