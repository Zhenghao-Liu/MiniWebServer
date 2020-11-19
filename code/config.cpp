/*
 * @Author: Zhenghao-Liu
 * @Date: 2020-11-19 19:57:33
 * @LastEditTime: 2020-11-19 20:47:00
 * @LastEditors: Please set LastEditors
 * @Description: 参数配置实现
 * @FilePath: \MiniWebServer\code\config.cpp
 */

#include "config.h"
#include <stdlib.h>

Config::Config()
{
    //端口号,默认9006
    PORT = 9006;

    //日志写入方式，默认同步
    LOGWrite = 0;

    //触发组合模式,默认listenfd LT + connfd LT
    TRIGMode = 0;

    //listenfd触发模式，默认LT
    LISTENTrigmode = 0;

    //connfd触发模式，默认LT
    CONNTrigmode = 0;

    //优雅关闭链接，默认不使用
    OPT_LINGER = 0;

    //数据库连接池数量,默认8
    sql_num = 8;

    //线程池内的线程数量,默认8
    thread_num = 8;

    //关闭日志,默认不关闭
    close_log = 0;

    //并发模型,默认是proactor
    actor_model = 0;
}

/*
 * -p，自定义端口号
       默认9006
 * -l，选择日志写入方式，默认同步写入
       0，同步写入
       1，异步写入
 * -m，listenfd和connfd的模式组合，默认使用LT + LT
       0，表示使用LT + LT
       1，表示使用LT + ET
       2，表示使用ET + LT
       3，表示使用ET + ET
 * -o，优雅关闭连接，默认不使用
       0，不使用
       1，使用
 * -s，数据库连接数量
       默认为8
 * -t，线程数量
       默认为8
 * -c，关闭日志，默认打开
       0，打开日志
       1，关闭日志
 * -a，选择反应堆模型，默认Proactor
       0，Proactor模型
       1，Reactor模型
 */
void Config::parse_arg(int argc, char *argv[])
{
    int opt;
    const char *str = "p:l:m:o:s:t:c:a:";
    while ((opt = getopt(argc, argv, str)) != -1)
    {
        switch (opt)
        {
        case 'p':
        {
            PORT = atoi(optarg);
            break;
        }
        case 'l':
        {
            LOGWrite = atoi(optarg);
            break;
        }
        case 'm':
        {
            TRIGMode = atoi(optarg);
            break;
        }
        case 'o':
        {
            OPT_LINGER = atoi(optarg);
            break;
        }
        case 's':
        {
            sql_num = atoi(optarg);
            break;
        }
        case 't':
        {
            thread_num = atoi(optarg);
            break;
        }
        case 'c':
        {
            close_log = atoi(optarg);
            break;
        }
        case 'a':
        {
            actor_model = atoi(optarg);
            break;
        }
        default:
            exit(EXIT_FAILURE);
        }
    }
}