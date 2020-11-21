/*
 * @Author: Zhenghao-Liu
 * @Date: 2020-11-18 14:33:52
 * @LastEditTime: 2020-11-18 15:18:19
 * @LastEditors: Please set LastEditors
 * @Description: 校验&数据库连接池定义文件
 * @FilePath: \MiniWebServer\code\CGImysql\sql_connection_pool.cpp
 */

#include <mysql/mysql.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <list>
#include <pthread.h>
#include <iostream>
#include "sql_connection_pool.h"
using namespace std;

connection_pool::connection_pool()
{
    m_CurConn = 0;
    m_FreeConn = 0;
}

connection_pool *connection_pool::GetInstance()
{
    static connection_pool connPool;
    return &connPool;
}

void connection_pool::init(string url, string User, string PassWord, string DataBaseName, int Port, int MaxConn, int close_log)
{
    m_url = url;
    m_Port = Port;
    m_User = User;
    m_PassWord = PassWord;
    m_DatabaseName = DataBaseName;
    m_close_log = close_log;
    for (int i = 0; i < MaxConn; ++i)
    {
        MYSQL *con = NULL;
        con = mysql_init(con);
        if (con == NULL)
        {
            LOG_ERROR("MySQL Error");
            exit(EXIT_FAILURE);
        }
        con = mysql_real_connect(con, url.c_str(), User.c_str(), PassWord.c_str(), DataBaseName.c_str(), Port, NULL, 0);
        if (con == NULL)
        {
            LOG_ERROR("MySQL Errot");
            exit(EXIT_FAILURE);
        }
        connList.push_back(con);
        ++m_FreeConn;
    }
    reserve = sem(m_FreeConn);
    m_MaxConn = m_FreeConn;
}

MYSQL *connection_pool::GetConnection()
{
    MYSQL *con = NULL;
    if (connList.empty())
        return NULL;
    reserve.wait();
    lock.lock();
    con = connList.front();
    connList.pop_front();
    --m_FreeConn;
    ++m_CurConn;
    lock.unlock();
    return con;
}

bool connection_pool::ReleaseConnection(MYSQL *con)
{
    if (con == NULL)
        return false;
    lock.lock();
    connList.push_back(con);
    ++m_FreeConn;
    --m_CurConn;
    lock.unlock();
    reserve.post();
    return true;
}

void connection_pool::DestroyPool()
{
    lock.lock();
    if (connList.size() > 0)
    {
        list<MYSQL *>::iterator it, ed;
        ed = connList.end();
        for (it = connList.begin(); it != ed; ++it)
        {
            MYSQL *con = *it;
            mysql_close(con);
        }
        m_CurConn = 0;
        m_FreeConn = 0;
        connList.clear();
    }
    lock.unlock();
}

int connection_pool::GetFreeConn() { return this->m_FreeConn; }

connection_pool::~connection_pool() { DestroyPool(); }

connectionRAII::connectionRAII(MYSQL **SQL, connection_pool *connPool)
{
    *SQL = connPool->GetConnection();
    conRAII = *SQL;
    poolRAII = connPool;
}

connectionRAII::~connectionRAII() { poolRAII->ReleaseConnection(conRAII); }
