/*
 * @Author: Zhenghao-Liu
 * @Date: 2020-11-17 17:01:12
 * @LastEditTime: 2020-11-18 09:57:11
 * @LastEditors: Please set LastEditors
 * @Description: 循环队列
 * @FilePath: \MiniWebServer\log\block_queue.h
 */

#ifndef BLOCK_QUEUE_H
#define BLOCK_QUEUE_H

#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include "../lock/locker.h"
using namespace std;

/*
 * 将生产者-消费者模型封装为阻塞队列，创建一个写线程，
 * 工作线程将要写的内容push进队列，写线程从队列中取出内容，写入日志文件
 * 用一个循环数组来模拟队列
 * m_mutex 互斥锁
   cond 条件变量
   m_array 队列实例化后的元素类型
   m_size 队列当前实际使用大小
   m_max_size 队列最大容量
   m_front 指向队列的头指针
   m_back 指向队列的尾指针，采取左闭右闭[m_front,m_back]
 * m_back = (m_back + 1) % m_max_size; 将事件增加到队尾
 * 
 */
template <typename T>
class block_queue
{
private:
    locker m_mutex;
    cond m_cond;
    T *m_array;
    int m_size;
    int m_max_size;
    int m_front;
    int m_back;
public:
    block_queue(int max_size=1000)
    {
        if (max_size<=0)
        {
            printf("max_size of block_queue is wrong\n");
            exit(EXIT_FAILURE);
        }
        m_max_size=max_size;
        m_array=new T[m_max_size];
        m_size=0;
        m_front=-1;
        m_back=-1;
    }
    ~block_queue()
    {
        m_mutex.lock();
        if (m_array!=NULL)
            delete [] m_array;
        m_mutex.unlock();
    }
    void clear()
    {
        m_mutex.lock();
        m_size=0;
        m_front=-1;
        m_back=-1;
        m_mutex.unlock();
    }
    bool full()
    {
        m_mutex.lock();
        if (m_size>=m_max_size)
        {
            m_mutex.unlock();
            return true;
        }
        m_mutex.unlock();
        return false;
    }
    bool empty()
    {
        m_mutex.lock();
        if (m_size==0)
        {
            m_mutex.unlock();
            return true;
        }
        m_mutex.unlock();
        return false;
    }
    bool front(T &value)
    {
        m_mutex.lock();
        if (m_size==0)
        {
            m_mutex.unlock();
            return false;
        }
        value=m_array[m_front];
        m_mutex.unlock();
        return true;
    }
    bool back(T &value)
    {
        m_mutex.lock();
        if (m_size==0)
        {
            m_mutex.unlock();
            return false;
        }
        value=m_array[m_back];
        m_mutex.unlock();
        return true;
    }
    int size()
    {
        int ans=0;
        m_mutex.lock();
        ans=m_size;
        m_mutex.unlock();
        return ans;
    }
    int max_size()
    {
        int ans=0;
        m_mutex.lock();
        ans=m_max_size;
        m_mutex.unlock();
        return ans;
    }
    bool push(const T & in_elem)
    {
        m_mutex.lock();
        if (m_size>=m_max_size)
        {
            m_cond.broadcast();
            m_mutex.unlock();
            return false;
        }
        if (m_back==-1 && m_front==-1)
        {
            m_back=0;
            m_front=0;
        }
        else
            m_back=(m_back+1)%m_max_size;
        m_array[m_back]=in_elem;
        ++m_size;
        m_cond.broadcast();
        m_mutex.unlock();
        return true;
    }
    bool pop(T & out_elem)
    {
        m_mutex.lock();
        while (m_size<=0)
            if (!m_cond.wait(m_mutex.get()))
            {
                m_mutex.unlock();
                return false;
            }
        out_elem=m_array[m_front];
        m_front=(m_front+1)%m_max_size;
        --m_size;
        m_mutex.unlock();
        return true;
    }
    bool pop(T & out_elem,int ms_timeout)
    {
        struct timeval now={0,0};
        gettimeofday(&now,NULL);
        struct timespec t={0,0};
        m_mutex.lock();
        if (m_size<=0)
        {
            t.tv_sec=now.tv_sec+ms_timeout/1000;
            t.tv_nsec=(now.tv_usec+(ms_timeout%1000)*1000)*1000;
            if (!m_cond.timewait(m_mutex.get(),t))
            {
                m_mutex.unlock();
                return false;
            }
        }
        if (m_size<=0)
        {
            m_mutex.unlock();
            return false;
        }
        out_elem=m_array[m_front];
        m_front=(m_front+1)%m_max_size;
        --m_size;
        m_mutex.unlock();
        return true;
    }
};
