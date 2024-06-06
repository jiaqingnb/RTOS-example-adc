/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-06-06     BlackWarrior       the first version
 */
#include "stm32f4xx.h"
#include "stm32f4xx_hal_tim.h"
#include "tim.h"

static TIM_HandleTypeDef TIM6_TimeBaseStructure;


//创建中断配置表，{中断句柄，重装载计数器，分频因子}
s_interrconfig interrupt_config[INTERRUPT_CONFIGNUM] = {
        {&TIM6_TimeBaseStructure, 5000-1, 8000-1}
};

void NVIC_Config_Init(void)
{
    HAL_NVIC_SetPriority(BASIC_TIM6_IRQn, 0, 3);
    HAL_NVIC_EnableIRQ(BASIC_TIM6_IRQn);

    // 开启TIMx_CLK,x[6,7]
    BASIC_TIM6_CLK_ENABLE();

    interrupt_config[0].interr_handles->Instance = BASIC_TIM6;
    /* 累计 TIM_Period个后产生一个更新或者中断*/
    //当定时器从0计数到4999，即为5000次，为一个定时周期
    interrupt_config[0].interr_handles->Init.Period = 5000-1;
    //定时器时钟源TIMxCLK = 2 * PCLK1
    //        PCLK1 = HCLK / 4
    //        => TIMxCLK=HCLK/2=SystemCoreClock/2=84MHz
    // 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=10000Hz
    interrupt_config[0].interr_handles->Init.Prescaler = 8400-1;

    // 初始化定时器TIMx, x[2,3,4,5]
    HAL_TIM_Base_Init(interrupt_config[0].interr_handles);

    // 开启定时器更新中断
    HAL_TIM_Base_Start_IT(interrupt_config[0].interr_handles);
}
rt_uint32_t mjq = 0;
void BASIC_TIM_IRQHandler(void)
{
    //关闭中断标志位
    __HAL_TIM_CLEAR_IT(interrupt_config[0].interr_handles, TIM_FLAG_UPDATE);
    /** 中断处理*/

}
