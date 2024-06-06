/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-06-04     BlackWarrior       the first version
 */
#ifndef APPLICATIONS_THREATABLE_THREADTABLE_H_
#define APPLICATIONS_THREATABLE_THREADTABLE_H_

#include <rtthread.h>




typedef struct{
    struct rt_thread* thread;        //线程句柄
    char* name;                      //线程名字
    void (*entry)(void *parameter);  //线程入口函数
    void* parameter;                 //线程入口函数参数
    void  *stack_start;              //线程栈起始地址
    rt_uint32_t   stack_size;        //线程栈大小
    rt_uint8_t    priority;          //线程优先级
    rt_uint32_t   tick;              //线程时间片大小
}s_thredtable;


void thredtableinit(void);

#endif /* APPLICATIONS_THREATABLE_THREADTABLE_H_ */
