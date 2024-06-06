/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-06-04     BlackWarrior       the first version
 */
#include "threadtable.h"

static void lcd_thread_entry();
static void adc_thread_entry();

#define TABLE_ENRTY  2   //线程表数量

/** 创建LCD线程相关变量*/
static struct rt_thread lcd_thread;
static char lcd_thread_stack[2048] = {0};
#define LCD_THREAD_PRIORITY   10
#define LCD_THREAD_TIMRSLICE  10
/** 创建LCD线程相关变量*/
static struct rt_thread adc_thread;
static char adc_thread_stack[2048] = {0};
#define ADC_THREAD_PRIORITY   9
#define ADC_THREAD_TIMRSLICE  10

s_thredtable thread_table[TABLE_ENRTY] = {
        {&lcd_thread,"lcdthread",lcd_thread_entry,RT_NULL,&lcd_thread_stack[0],sizeof(lcd_thread_stack),LCD_THREAD_PRIORITY,LCD_THREAD_TIMRSLICE},
        {&adc_thread,"adcthread",adc_thread_entry,RT_NULL,&adc_thread_stack[0],sizeof(adc_thread_stack),ADC_THREAD_PRIORITY,ADC_THREAD_TIMRSLICE}
};
/** 功能————本文件的功能就是做一张线程表，上层使用可以仅在表中添加线程并配置相应参数即可
 *
 *  说明： 暂时使用静态内存方式初始化线程
 *
 *
 */



void thredtableinit(void)
{
    for(rt_uint8_t i = 0;i < TABLE_ENRTY;i++)
    {
        rt_thread_init(thread_table[i].thread, thread_table[i].name, thread_table[i].entry,
                       thread_table[i].parameter, thread_table[i].stack_start, thread_table[i].stack_size,
                       thread_table[i].priority, thread_table[i].tick);
        rt_thread_startup(thread_table[i].thread);
    }
}

static void lcd_thread_entry()
{
    rt_kprintf("lcd_thread start!\n");

    rt_thread_mdelay(500);
}

static void adc_thread_entry()
{
    rt_kprintf("adc_thread start!\n");

    rt_thread_mdelay(500);
}


