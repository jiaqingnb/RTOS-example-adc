/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-05-01     Administrator       the first version
 */
#include "eeprom.h"


#define AHT10_I2C_BUS_NAME          "i2c1"  /* 传感器连接的I2C总线设备名称 */
#define AT24C02_ADDRESS             0x50    /* 设备地址*/

static struct rt_i2c_bus_device *i2c_bus = RT_NULL;     /* I2C总线设备句柄 */
/**
 *
 */
void init_eeprom()
{


    /* 查找I2C总线设备，获取I2C总线设备句柄 */
    i2c_bus = (struct rt_i2c_bus_device *)rt_device_find(AHT10_I2C_BUS_NAME);

    if (i2c_bus == RT_NULL)
    {
        rt_kprintf("can't find %s device!\n", AHT10_I2C_BUS_NAME);
    }
    else
    {
        rt_kprintf("find %s device!\n", AHT10_I2C_BUS_NAME);
    }
}
/**
 *
 */
rt_size_t write_data(rt_uint8_t* data,rt_uint32_t len,rt_uint8_t page)
{
    rt_size_t res;
    rt_uint8_t wr_data[1] = {0};
    wr_data[0] = page;
    res = rt_i2c_master_send(i2c_bus, AT24C02_ADDRESS, RT_I2C_WR|RT_I2C_NO_STOP, wr_data, 1);//写起始信号

    res = rt_i2c_master_send(i2c_bus, AT24C02_ADDRESS, RT_I2C_WR|RT_I2C_NO_START, data, len);//写起始信号

}

rt_size_t read_data(rt_uint8_t* data,rt_uint32_t len,rt_uint8_t page)
{
    rt_size_t res;
    rt_uint8_t wr_data[1] = {0};
    wr_data[0] = page;
    res = rt_i2c_master_send(i2c_bus, AT24C02_ADDRESS, RT_I2C_WR, wr_data, 1);//写起始信号+地址信号

    res = rt_i2c_master_recv(i2c_bus, AT24C02_ADDRESS, RT_I2C_RD, data, len);
}
