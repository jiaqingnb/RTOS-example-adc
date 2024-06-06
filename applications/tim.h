/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-06-06     BlackWarrior       the first version
 */
#ifndef APPLICATIONS_TIM_H_
#define APPLICATIONS_TIM_H_

#include <rtthread.h>

/* Private typedef -----------------------------------------------------------*/
typedef struct{
    TIM_HandleTypeDef* interr_handles;    //中断结构体句柄
    rt_uint32_t   period;                 //重装载数
    rt_uint32_t   prescaler;              //分频因子
}s_interrconfig;
/* Private define ------------------------------------------------------------*/
#define INTERRUPT_CONFIGNUM      1    //中断配置表数量

#define BASIC_TIM6   TIM6
#define BASIC_TIM6_CLK_ENABLE() __TIM6_CLK_ENABLE()
#define BASIC_TIM6_IRQn            TIM6_DAC_IRQn
#define BASIC_TIM6_IRQHandler      TIM6_DAC_IRQHandler


void NVIC_Config_Init(void);

#endif /* APPLICATIONS_TIM_H_ */
