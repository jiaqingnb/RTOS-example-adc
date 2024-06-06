/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-05-01     Administrator       the first version
 */
#ifndef APPLICATIONS_EEPROM_EEPROM_H_
#define APPLICATIONS_EEPROM_EEPROM_H_
#include <rtthread.h>
#include <rtdevice.h>

void init_eeprom();
rt_size_t write_data(rt_uint8_t* data,rt_uint32_t len,rt_uint8_t page);
rt_size_t read_data(rt_uint8_t* data,rt_uint32_t len,rt_uint8_t page);

#endif /* APPLICATIONS_EEPROM_EEPROM_H_ */
