/*
 * @Author: Zhenghao-Liu
 * @Date: 2020-11-18 09:59:17
 * @LastEditTime: 2020-11-18 11:03:56
 * @LastEditors: Please set LastEditors
 * @Description: 日志头文件
 * @FilePath: \MiniWebServer\log\log.h
 */

#include <stdio.h>
#include <iostream>
#include <string>
#include <stdarg.h>
#include <pthread.h>
#include "block_queue.h"
using namespace std;

/*
 * dir_name 路径名
   log_name 日志文件名
   m_split_lines 日志最大行数
   m_log_bug_size 日志缓冲区大小
   m_count 日志当前已用行数
   m_today 当前时间是哪一天
   m_fp 打开log的文件指针
   m_buf 缓冲区指针
   m_log_queue 阻塞队列
   m_is_async 是否同步标志位
   m_mutex 互斥锁
   m_clog_log 是否关闭日志
 * 单例模式：将构造写入private，通过指针进行实例化，这样保证只能实例化一次
 * 懒汉模式：需要用到创建实例了程序再去创建实例
 * async_write_log 同步的从消息队列中取出消息，写入日志文件中
 */
class Log
{
private:
    char dir_name[128];
    char log_name[128];
    int m_split_lines;
    int m_log_buf_size;
    long long m_count;
    int m_today;
    FILE *m_fp;
    char *m_buf;
    block_queue<string> *m_log_queue;
    bool m_is_async;
    locker m_mutex;
    int m_close_log;

private:
    Log();
    virtual ~Log();
    void *async_write_log()
    {
        string single_log;
        while (m_log_queue->pop(single_log))
        {
            m_mutex.lock();
            fputs(single_log.c_str(), m_fp);
            m_mutex.lock();
        }
    }

public:
    static Log *get_instance()
    {
        static Log instance();
        return &instance;
    }
    static void *flush_log_thread(void *args) { Log::get_instance()->async_write_log(); }
    bool init(const char *file_name, int close_log, int log_buf_size = 8192, int split_lines = 5000000, int max_queue_size = 0);
    void write_log(int level, const char *format, ...);
    void flush(void);
};

#define LOG_DEBUG(format, ...)                                    \
    if (0 == m_close_log)                                         \
    {                                                             \
        Log::get_instance()->write_log(0, format, ##__VA_ARGS__); \
        Log::get_instance()->flush();                             \
    }
#define LOG_INFO(format, ...)                                     \
    if (0 == m_close_log)                                         \
    {                                                             \
        Log::get_instance()->write_log(1, format, ##__VA_ARGS__); \
        Log::get_instance()->flush();                             \
    }
#define LOG_WARN(format, ...)                                     \
    if (0 == m_close_log)                                         \
    {                                                             \
        Log::get_instance()->write_log(2, format, ##__VA_ARGS__); \
        Log::get_instance()->flush();                             \
    }
#define LOG_ERROR(format, ...)                                    \
    if (0 == m_close_log)                                         \
    {                                                             \
        Log::get_instance()->write_log(3, format, ##__VA_ARGS__); \
        Log::get_instance()->flush();                             \
    }

#endif