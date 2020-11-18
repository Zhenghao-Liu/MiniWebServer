/*
 * @Author: Zhenghao-Liu
 * @Date: 2020-11-18 15:21:46
 * @LastEditTime: 2020-11-18 21:29:52
 * @LastEditors: Please set LastEditors
 * @Description: 线程池封装类头文件
 * @FilePath: \MiniWebServer\code\threadpool\threadpool.h
 */

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>
#include "../lock/locker.h"
#include "../CGImysql/sql_connection_pool.h"

/*
 * m_thread_number 线程池中的线程数
   m_max_requests 请求队列中允许的最大请求数
   m_threads 描述线程池的数组，其大小为m_thread_number
   m_workqueue 请求队列
   m_queuelocker 保护请求队列的互斥锁
   m_queuestat 是否有任务需要处理
   m_connPool 数据库
   m_actor_model 模型切换
 * worker 工作线程运行的函数
   run 工作线程的真正运行函数，它不断从工作队列中取出任务并执行
 * append 往请求队列中添加任务
 */
template <typename T>
class threadpool
{
private:
    int m_thread_number;
    int m_max_requests;
    pthread_t *m_threads;
    std::list<T *> m_workqueue;
    locker m_queuelocker;
    sem m_queuestat;
    connection_pool *m_connPool;
    int m_actor_model;
    static void *worker(void *arg);
    void run();

public:
    threadpool(int actor_model, connection_pool *connPool, int thread_number = 8, int max_request = 10000);
    ~threadpool();
    bool append(T *request, int state);
    bool append_p(T *request);
};

template <typename T>
threadpool<T>::threadpool(int actor_model, connection_pool *connPool, int thread_number, int max_requests) :
    : m_actor_model(actor_model), m_thread_number(thread_number), m_max_requests(max_requests), m_threads(NULL), m_connPool(connPool)
{
    if (thread_number <= 0 || max_requests <= 0)
        throw std::exception();
    m_threads = new pthread_t[m_thread_number];
    if (m_threads == NULL)
        throw std::exception();
    for (int i = 0; i < thread_number; ++i)
    {
        if (pthread_create(m_threads + i, NULL, worker, thiis) != 0)
        {
            delete[] m_threads;
            throw std::exception();
        }
        if (pthread_detach(m_threads[i]))
        {
            delete[] m_threads;
            throw std::exception();
        }
    }
}

template <typename T>
threadpool<T>::~threadpool() { delete[] m_threads; }

template <typename T>
bool threadpool<T>::append(T *request, int state)
{
    m_queuelocker.lock();
    if ((int)m_workqueue.size() >= m_max_requests)
    {
        m_queuelocker.unlock();
        return false;
    }
    request->m_state = state;
    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.post();
    return true;
}

template <typename T>
bool threadpool<T>::append_p(T *request)
{
    m_queuelocker.lock();
    if (m_workqueue.size() >= m_max_requests)
    {
        m_queuelocker.unlock();
        return false;
    }
    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.post();
    return true;
}

template <typename T>
void *threadpool<T>::worker(void *arg)
{
    threadpool *pool = (threadpool *)arg;
    pool->run();
    return pool;
}

template <typename T>
void threadpool<T>::run()
{
    while (true)
    {
        m_queuestat.wait();
        m_queuelocker.lock();
        if (m_workqueue.empty())
        {
            m_queuelocker.unlock();
            continue;
        }
        T *request = m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelocker.unlock();
        if (request == NULL)
            continue;
        if (m_actor_model == 1)
        {
            if (request->m_state == 0)
            {
                if (request->read_once())
                {
                    request->improv = 1;
                    connectionRAII mysqlcon(&request->mysql, m_connPool);
                    request->process();
                }
                else
                {
                    request->improv = 1;
                    request->timer_flag = 1;
                }
            }
            else
            {
                if (request->write())
                {
                    request->improv = 1;
                }
                else
                {
                    request->improv = 1;
                    request->timer_flag = 1;
                }
            }
        }
        else
        {
            connectionRAII mysqlcon(&request->mysql, m_connPool);
            request->process();
        }
    }
}

#endif