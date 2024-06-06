/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 * 2018-11-19     flybreak     add stm32f407-atk-explorer bsp
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "eeprom.h"
#include "stm32f4xx.h"
#include "lcd.h"
#include "stm32f4xx_hal_tim.h"
#include "threadtable.h"
#include "tim.h"

/* defined the LED0 pin: PF9 */
#define LED0_PIN    GET_PIN(F, 9)

int main(void)
{

    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    NVIC_Config_Init();
    /** 线程表初始化*/
    thredtableinit();
    while(1)
    {
//        rt_kprintf("mjq = %d\n",mjq);
        rt_thread_delay(1000);
    }

#if 0
    lcd_init();
    g_point_color = RED;
    while (1)
        {
            switch (x)
            {
            case 0:
                lcd_clear(WHITE);
                break;

            case 1:
                lcd_clear(BLACK);
                break;

            case 2:
                lcd_clear(BLUE);
                break;

            case 3:
                lcd_clear(RED);
                break;

            case 4:
                lcd_clear(MAGENTA);
                break;

            case 5:
                lcd_clear(GREEN);
                break;

            case 6:
                lcd_clear(CYAN);
                break;

            case 7:
                lcd_clear(YELLOW);
                break;

            case 8:
                lcd_clear(BRRED);
                break;

            case 9:
                lcd_clear(GRAY);
                break;

            case 10:
                lcd_clear(LGRAY);
                break;

            case 11:
                lcd_clear(BROWN);
                break;
            }
//            x++;
            rt_kprintf("page1:=%d\n",x%12);
            rt_thread_delay(1000);
        }
#endif
#if 0
    int count = 1;
    rt_uint8_t data[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};//写入地址+数据
    char* str = "world";

    rt_uint8_t x = 0;
    rt_uint8_t rcvdata[100] = {0};
    init_eeprom();
    while (count == 1)
    {
        count++;
        write_data(data, 8, 0);
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);

        read_data(rcvdata, 8, 0);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);

        rt_kprintf("page1:");
        for(rt_uint8_t i = 0; i<8; i++)
        rt_kprintf("%d ",rcvdata[i]);
        rt_kprintf("\n");

        write_data(&data[8], 8, 8);
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);

        read_data(rcvdata, 8, 8);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);

        rt_kprintf("page2:");
        for(rt_uint8_t i = 0; i<8; i++)
        rt_kprintf("%d ",rcvdata[i]);
        rt_kprintf("\n");
    }
#endif

    return RT_EOK;
}
