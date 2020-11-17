/*
 * @Author: Zhenghao-liu
 * @Date: 2020-11-17 14:54:47
 * @LastEditTime: 2020-11-17 19:11:17
 * @LastEditors: Please set LastEditors
 * @Description: 输出errno
 * @FilePath: \MiniWebServer\show_errno\show_errno.h
 */

#ifndef SHOW_ERRNO
#define SHOW_ERRNO

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
void show_errno()
{
    printf("Error: %s\n", strerror(errno));
    printf("Program was terminated unexpectedly.\n");
    exit(EXIT_FAILURE);
}

#endif