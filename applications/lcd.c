/**
 ****************************************************************************************************
 * @file        lcd.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2023-05-29
 * @brief       2.8��/3.5��/4.3��/7�� TFTLCD(MCU��) ��������
 *              ֧������IC�ͺŰ���:ILI9341/NT35310/NT35510/SSD1963/ST7789/ST7796/ILI9806��
 *
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ̽���� F407������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20211016
 * ��һ�η���
 * V1.1 20230529
 * 1��������ST7796��ILI9806 IC֧��
 * 2���򻯲��ִ��룬���ⳤ�ж�
 ****************************************************************************************************
 */

#include "stdlib.h"
#include "lcdfont.h"
#include "lcd.h"
#include "stm32f4xx_hal_sdram.h"
#include "drv_common.h"


/* lcd_ex.c��Ÿ���LCD����IC�ļĴ�����ʼ�����ִ���,�Լ�lcd.c,��.c�ļ�
 * ��ֱ�Ӽ��뵽��������,ֻ��lcd.c���õ�,����ͨ��include����ʽ���.(��Ҫ��
 * �����ļ��ٰ�����.c�ļ�!!����ᱨ��!)
 */
//#include "lcd_ex.c"


SRAM_HandleTypeDef g_sram_handle;   /* SRAM���(���ڿ���LCD) */

/* LCD�Ļ�����ɫ�ͱ���ɫ */
rt_uint32_t g_point_color = 0xF800;    /* ������ɫ */
rt_uint32_t g_back_color  = 0xFFFF;    /* ����ɫ */

/* ����LCD��Ҫ���� */
_lcd_dev lcddev;

/**
 * @brief       LCDд����
 * @param       data: Ҫд�������
 * @retval      ��
 */
void lcd_wr_data(volatile rt_uint16_t data)
{
    data = data;            /* ʹ��-O2�Ż���ʱ��,����������ʱ */
    LCD->LCD_RAM = data;
}

/**
 * @brief       LCDд�Ĵ������/��ַ����
 * @param       regno: �Ĵ������/��ַ
 * @retval      ��
 */
void lcd_wr_regno(volatile rt_uint16_t regno)
{
    regno = regno;          /* ʹ��-O2�Ż���ʱ��,����������ʱ */
    LCD->LCD_REG = regno;   /* д��Ҫд�ļĴ������ */
}

/**
 * @brief       LCDд�Ĵ���
 * @param       regno:�Ĵ������/��ַ
 * @param       data:Ҫд�������
 * @retval      ��
 */
void lcd_write_reg(rt_uint16_t regno, rt_uint16_t data)
{
    LCD->LCD_REG = regno;   /* д��Ҫд�ļĴ������ */
    LCD->LCD_RAM = data;    /* д������ */
}

/**
 * @brief       LCD��ʱ����,�����ڲ�����mdk -O1ʱ���Ż�ʱ��Ҫ���õĵط�
 * @param       t:��ʱ����ֵ
 * @retval      ��
 */
static void lcd_opt_delay(rt_uint32_t i)
{
    while (i--); /* ʹ��AC6ʱ��ѭ�����ܱ��Ż�,��ʹ��while(1) __asm volatile(""); */
}

/**
 * @brief       LCD������
 * @param       ��
 * @retval      ��ȡ��������
 */
static rt_uint16_t lcd_rd_data(void)
{
    volatile rt_uint16_t ram;  /* ��ֹ���Ż� */
    lcd_opt_delay(2);
    ram = LCD->LCD_RAM;
    return ram;
}

/**
 * @brief       ׼��дGRAM
 * @param       ��
 * @retval      ��
 */
void lcd_write_ram_prepare(void)
{
    LCD->LCD_REG = lcddev.wramcmd;
}

/**
 * @brief       ��ȡ��ĳ�����ɫֵ
 * @param       x,y:����
 * @retval      �˵����ɫ(32λ��ɫ,�������LTDC)
 */
rt_uint32_t lcd_read_point(rt_uint16_t x, rt_uint16_t y)
{
    rt_uint16_t r = 0, g = 0, b = 0;

    if (x >= lcddev.width || y >= lcddev.height)
    {
        return 0;   /* �����˷�Χ,ֱ�ӷ��� */
    }

    lcd_set_cursor(x, y);       /* �������� */

    if (lcddev.id == 0x5510)
    {
        lcd_wr_regno(0x2E00);   /* 5510 ���Ͷ�GRAMָ�� */
    }
    else
    {
        lcd_wr_regno(0x2E);     /* 9341/5310/1963/7789/7796/9806 �ȷ��Ͷ�GRAMָ�� */
    }


    r = lcd_rd_data();          /* �ٶ�(dummy read) */

    if (lcddev.id == 0x1963)
    {
        return r;   /* 1963ֱ�Ӷ��Ϳ��� */
    }

    r = lcd_rd_data();          /* ʵ��������ɫ */
    
    if (lcddev.id == 0x7796)    /* 7796 һ�ζ�ȡһ������ֵ */
    {
        return r;
    }
    
    /* 9341/5310/5510/7789/9806Ҫ��2�ζ��� */
    b = lcd_rd_data();
    g = r & 0xFF;               /* ����9341/5310/5510/7789/9806,��һ�ζ�ȡ����RG��ֵ,R��ǰ,G�ں�,��ռ8λ */
    g <<= 8;
    
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11));  /* ILI9341/NT35310/NT35510/ST7789/ILI9806��Ҫ��ʽת��һ�� */
}

/**
 * @brief       LCD������ʾ
 * @param       ��
 * @retval      ��
 */
void lcd_display_on(void)
{
    if (lcddev.id == 0x5510)
    {
        lcd_wr_regno(0x2900);   /* ������ʾ */
    }
    else                        /* 9341/5310/1963/7789/7796/9806 �ȷ��Ϳ�����ʾָ�� */
    {
        lcd_wr_regno(0x29);     /* ������ʾ */
    }
}

/**
 * @brief       LCD�ر���ʾ
 * @param       ��
 * @retval      ��
 */
void lcd_display_off(void)
{
    if (lcddev.id == 0x5510)
    {
        lcd_wr_regno(0x2800);   /* �ر���ʾ */
    }
    else                        /* 9341/5310/1963/7789/7796/9806 �ȷ��͹ر���ʾָ�� */
    {
        lcd_wr_regno(0x28);     /* �ر���ʾ */
    }
}

/**
 * @brief       ���ù��λ��(��RGB����Ч)
 * @param       x,y: ����
 * @retval      ��
 */
void lcd_set_cursor(rt_uint16_t x, rt_uint16_t y)
{
    if (lcddev.id == 0x1963)
    {
        if (lcddev.dir == 0)    /* ����ģʽ, x������Ҫ�任 */
        {
            x = lcddev.width - 1 - x;
            lcd_wr_regno(lcddev.setxcmd);
            lcd_wr_data(0);
            lcd_wr_data(0);
            lcd_wr_data(x >> 8);
            lcd_wr_data(x & 0xFF);
        }
        else                    /* ����ģʽ */
        {
            lcd_wr_regno(lcddev.setxcmd);
            lcd_wr_data(x >> 8);
            lcd_wr_data(x & 0xFF);
            lcd_wr_data((lcddev.width - 1) >> 8);
            lcd_wr_data((lcddev.width - 1) & 0xFF);
        }

        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(y >> 8);
        lcd_wr_data(y & 0xFF);
        lcd_wr_data((lcddev.height - 1) >> 8);
        lcd_wr_data((lcddev.height - 1) & 0xFF);

    }
    else if (lcddev.id == 0x5510)
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(x >> 8);
        lcd_wr_regno(lcddev.setxcmd + 1);
        lcd_wr_data(x & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(y >> 8);
        lcd_wr_regno(lcddev.setycmd + 1);
        lcd_wr_data(y & 0xFF);
    }
    else    /* 9341/5310/7789/7796/9806 �� �������� */
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(x >> 8);
        lcd_wr_data(x & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(y >> 8);
        lcd_wr_data(y & 0xFF);
    }
}

/**
 * @brief       ����LCD���Զ�ɨ�跽��(��RGB����Ч)
 *   @note
 *              9341/5310/5510/1963/7789/7796/9806��IC�Ѿ�ʵ�ʲ���
 *              ע��:�����������ܻ��ܵ��˺������õ�Ӱ��(������9341),
 *              ����,һ������ΪL2R_U2D����,�������Ϊ����ɨ�跽ʽ,���ܵ�����ʾ������.
 *
 * @param       dir:0~7,����8������(���嶨���lcd.h)
 * @retval      ��
 */
void lcd_scan_dir(rt_uint8_t dir)
{
    rt_uint16_t regval = 0;
    rt_uint16_t dirreg = 0;
    rt_uint16_t temp;

    /* ����ʱ����1963���ı�ɨ�跽������ʱ1963�ı䷽��(���������1963�����⴦��,����������IC��Ч) */
    if ((lcddev.dir == 1 && lcddev.id != 0x1963) || (lcddev.dir == 0 && lcddev.id == 0x1963))
    {
        switch (dir)   /* ����ת�� */
        {
            case 0:
                dir = 6;
                break;

            case 1:
                dir = 7;
                break;

            case 2:
                dir = 4;
                break;

            case 3:
                dir = 5;
                break;

            case 4:
                dir = 1;
                break;

            case 5:
                dir = 0;
                break;

            case 6:
                dir = 3;
                break;

            case 7:
                dir = 2;
                break;
        }
    }


    /* ����ɨ�跽ʽ ���� 0x36/0x3600 �Ĵ��� bit 5,6,7 λ��ֵ */
    switch (dir)
    {
        case L2R_U2D:   /* ������,���ϵ��� */
            regval |= (0 << 7) | (0 << 6) | (0 << 5);
            break;

        case L2R_D2U:   /* ������,���µ��� */
            regval |= (1 << 7) | (0 << 6) | (0 << 5);
            break;

        case R2L_U2D:   /* ���ҵ���,���ϵ��� */
            regval |= (0 << 7) | (1 << 6) | (0 << 5);
            break;

        case R2L_D2U:   /* ���ҵ���,���µ��� */
            regval |= (1 << 7) | (1 << 6) | (0 << 5);
            break;

        case U2D_L2R:   /* ���ϵ���,������ */
            regval |= (0 << 7) | (0 << 6) | (1 << 5);
            break;

        case U2D_R2L:   /* ���ϵ���,���ҵ��� */
            regval |= (0 << 7) | (1 << 6) | (1 << 5);
            break;

        case D2U_L2R:   /* ���µ���,������ */
            regval |= (1 << 7) | (0 << 6) | (1 << 5);
            break;

        case D2U_R2L:   /* ���µ���,���ҵ��� */
            regval |= (1 << 7) | (1 << 6) | (1 << 5);
            break;
    }

    dirreg = 0x36;  /* �Ծ��󲿷�����IC, ��0x36�Ĵ������� */

    if (lcddev.id == 0x5510)
    {
        dirreg = 0x3600;    /* ����5510, ����������ic�ļĴ����в��� */
    }

     /* 9341 & 7789 & 7796 Ҫ����BGRλ */
    if (lcddev.id == 0x9341 || lcddev.id == 0x7789 || lcddev.id == 0x7796)
    {
        regval |= 0x08;
    }

    lcd_write_reg(dirreg, regval);

    if (lcddev.id != 0x1963)                    /* 1963�������괦�� */
    {
        if (regval & 0x20)
        {
            if (lcddev.width < lcddev.height)   /* ����X,Y */
            {
                temp = lcddev.width;
                lcddev.width = lcddev.height;
                lcddev.height = temp;
            }
        }
        else
        {
            if (lcddev.width > lcddev.height)   /* ����X,Y */
            {
                temp = lcddev.width;
                lcddev.width = lcddev.height;
                lcddev.height = temp;
            }
        }
    }

    /* ������ʾ����(����)��С */
    if (lcddev.id == 0x5510)
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(0);
        lcd_wr_regno(lcddev.setxcmd + 1);
        lcd_wr_data(0);
        lcd_wr_regno(lcddev.setxcmd + 2);
        lcd_wr_data((lcddev.width - 1) >> 8);
        lcd_wr_regno(lcddev.setxcmd + 3);
        lcd_wr_data((lcddev.width - 1) & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(0);
        lcd_wr_regno(lcddev.setycmd + 1);
        lcd_wr_data(0);
        lcd_wr_regno(lcddev.setycmd + 2);
        lcd_wr_data((lcddev.height - 1) >> 8);
        lcd_wr_regno(lcddev.setycmd + 3);
        lcd_wr_data((lcddev.height - 1) & 0xFF);
    }
    else
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(0);
        lcd_wr_data(0);
        lcd_wr_data((lcddev.width - 1) >> 8);
        lcd_wr_data((lcddev.width - 1) & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(0);
        lcd_wr_data(0);
        lcd_wr_data((lcddev.height - 1) >> 8);
        lcd_wr_data((lcddev.height - 1) & 0xFF);
    }
}

/**
 * @brief       ����
 * @param       x,y: ����
 * @param       color: �����ɫ(32λ��ɫ,�������LTDC)
 * @retval      ��
 */
void lcd_draw_point(rt_uint16_t x, rt_uint16_t y, rt_uint32_t color)
{
    lcd_set_cursor(x, y);       /* ���ù��λ�� */
    lcd_write_ram_prepare();    /* ��ʼд��GRAM */
    LCD->LCD_RAM = color;
}

/**
 * @brief       SSD1963�����������ú���
 * @param       pwm: ����ȼ�,0~100.Խ��Խ��.
 * @retval      ��
 */
void lcd_ssd_backlight_set(rt_uint8_t pwm)
{
    lcd_wr_regno(0xBE);         /* ����PWM��� */
    lcd_wr_data(0x05);          /* 1����PWMƵ�� */
    lcd_wr_data(pwm * 2.55);    /* 2����PWMռ�ձ� */
    lcd_wr_data(0x01);          /* 3����C */
    lcd_wr_data(0xFF);          /* 4����D */
    lcd_wr_data(0x00);          /* 5����E */
    lcd_wr_data(0x00);          /* 6����F */
}

/**
 * @brief       ����LCD��ʾ����
 * @param       dir:0,����; 1,����
 * @retval      ��
 */
void lcd_display_dir(rt_uint8_t dir)
{
    lcddev.dir = dir;   /* ����/���� */

    if (dir == 0)       /* ���� */
    {
        lcddev.width = 240;
        lcddev.height = 320;

        if (lcddev.id == 0x5510)
        {
            lcddev.wramcmd = 0x2C00;
            lcddev.setxcmd = 0x2A00;
            lcddev.setycmd = 0x2B00;
            lcddev.width = 480;
            lcddev.height = 800;
        }
        else if (lcddev.id == 0x1963)
        {
            lcddev.wramcmd = 0x2C;  /* ����д��GRAM��ָ�� */
            lcddev.setxcmd = 0x2B;  /* ����дX����ָ�� */
            lcddev.setycmd = 0x2A;  /* ����дY����ָ�� */
            lcddev.width = 480;     /* ���ÿ��480 */
            lcddev.height = 800;    /* ���ø߶�800 */
        }
        else   /* ����IC, ����: 9341/5310/7789/7796/9806��IC */
        {
            lcddev.wramcmd = 0x2C;
            lcddev.setxcmd = 0x2A;
            lcddev.setycmd = 0x2B;
        }

        if (lcddev.id == 0x5310 || lcddev.id == 0x7796)     /* �����5310/7796 ���ʾ�� 320*480�ֱ��� */
        {
            lcddev.width = 320;
            lcddev.height = 480;
        }
        
        if (lcddev.id == 0X9806)    /* �����9806 ���ʾ�� 480*800 �ֱ��� */
        {
            lcddev.width = 480;
            lcddev.height = 800;
        }  
    }
    else        /* ���� */
    {
        lcddev.width = 320;         /* Ĭ�Ͽ�� */
        lcddev.height = 240;        /* Ĭ�ϸ߶� */

        if (lcddev.id == 0x5510)
        {
            lcddev.wramcmd = 0x2C00;
            lcddev.setxcmd = 0x2A00;
            lcddev.setycmd = 0x2B00;
            lcddev.width = 800;
            lcddev.height = 480;
        }
        else if (lcddev.id == 0x1963 || lcddev.id == 0x9806)
        {
            lcddev.wramcmd = 0x2C;  /* ����д��GRAM��ָ�� */
            lcddev.setxcmd = 0x2A;  /* ����дX����ָ�� */
            lcddev.setycmd = 0x2B;  /* ����дY����ָ�� */
            lcddev.width = 800;     /* ���ÿ��800 */
            lcddev.height = 480;    /* ���ø߶�480 */
        }
        else   /* ����IC, ����:9341/5310/7789/7796��IC */
        {
            lcddev.wramcmd = 0x2C;
            lcddev.setxcmd = 0x2A;
            lcddev.setycmd = 0x2B;
        }

        if (lcddev.id == 0x5310 || lcddev.id == 0x7796)     /* �����5310/7796 ���ʾ�� 320*480�ֱ��� */
        {
            lcddev.width = 480;
            lcddev.height = 320;
        }
    }

    lcd_scan_dir(DFT_SCAN_DIR);     /* Ĭ��ɨ�跽�� */
}

/**
 * @brief       ���ô���(��RGB����Ч), ���Զ����û������굽�������Ͻ�(sx,sy).
 * @param       sx,sy:������ʼ����(���Ͻ�)
 * @param       width,height:���ڿ�Ⱥ͸߶�,�������0!!
 *   @note      �����С:width*height.
 *
 * @retval      ��
 */
void lcd_set_window(rt_uint16_t sx, rt_uint16_t sy, rt_uint16_t width, rt_uint16_t height)
{
    rt_uint16_t twidth, theight;
    twidth = sx + width - 1;
    theight = sy + height - 1;

   
   if (lcddev.id == 0x1963 && lcddev.dir != 1)     /* 1963�������⴦�� */
    {
        sx = lcddev.width - width - sx;
        height = sy + height - 1;
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(sx >> 8);
        lcd_wr_data(sx & 0xFF);
        lcd_wr_data((sx + width - 1) >> 8);
        lcd_wr_data((sx + width - 1) & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(sy >> 8);
        lcd_wr_data(sy & 0xFF);
        lcd_wr_data(height >> 8);
        lcd_wr_data(height & 0xFF);
    }
    else if (lcddev.id == 0x5510)
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(sx >> 8);
        lcd_wr_regno(lcddev.setxcmd + 1);
        lcd_wr_data(sx & 0xFF);
        lcd_wr_regno(lcddev.setxcmd + 2);
        lcd_wr_data(twidth >> 8);
        lcd_wr_regno(lcddev.setxcmd + 3);
        lcd_wr_data(twidth & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(sy >> 8);
        lcd_wr_regno(lcddev.setycmd + 1);
        lcd_wr_data(sy & 0xFF);
        lcd_wr_regno(lcddev.setycmd + 2);
        lcd_wr_data(theight >> 8);
        lcd_wr_regno(lcddev.setycmd + 3);
        lcd_wr_data(theight & 0xFF);
    }
    else    /* 9341/5310/7789/1963/7796/9806���� �� ���ô��� */
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(sx >> 8);
        lcd_wr_data(sx & 0xFF);
        lcd_wr_data(twidth >> 8);
        lcd_wr_data(twidth & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(sy >> 8);
        lcd_wr_data(sy & 0xFF);
        lcd_wr_data(theight >> 8);
        lcd_wr_data(theight & 0xFF);
    }
}

/**
 * @brief       SRAM�ײ�������ʱ��ʹ�ܣ����ŷ���
 * @note        �˺����ᱻHAL_SRAM_Init()����,��ʼ����д��������
 * @param       hsram:SRAM���
 * @retval      ��
 */
//void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
//{
//    GPIO_InitTypeDef gpio_init_struct;
//
//    __HAL_RCC_FSMC_CLK_ENABLE();            /* ʹ��FSMCʱ�� */
//    __HAL_RCC_GPIOD_CLK_ENABLE();           /* ʹ��GPIODʱ�� */
//    __HAL_RCC_GPIOE_CLK_ENABLE();           /* ʹ��GPIOEʱ�� */
//
//    /* ��ʼ��PD0,1, 8,9,10,14,15 */
//    gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 \
//                           | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
//    gpio_init_struct.Mode = GPIO_MODE_AF_PP;            /* ���츴�� */
//    gpio_init_struct.Pull = GPIO_PULLUP;                /* ���� */
//    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;      /* ���� */
//    gpio_init_struct.Alternate = GPIO_AF12_FSMC;        /* ����ΪFSMC */
//
//    HAL_GPIO_Init(GPIOD, &gpio_init_struct);            /* ��ʼ�� */
//
//    /* ��ʼ��PE7,8,9,10,11,12,13,14,15 */
//    gpio_init_struct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 \
//                           | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
//    HAL_GPIO_Init(GPIOE, &gpio_init_struct);
//}

/**
 * @brief       ��ʼ��LCD
 *   @note      �ó�ʼ���������Գ�ʼ�������ͺŵ�LCD(�����.c�ļ���ǰ�������)
 *
 * @param       ��
 * @retval      ��
 */
void lcd_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    FSMC_NORSRAM_TimingTypeDef fsmc_read_handle;
    FSMC_NORSRAM_TimingTypeDef fsmc_write_handle;
#if 1
    LCD_CS_GPIO_CLK_ENABLE();   /* LCD_CS��ʱ��ʹ�� */
    LCD_WR_GPIO_CLK_ENABLE();   /* LCD_WR��ʱ��ʹ�� */
    LCD_RD_GPIO_CLK_ENABLE();   /* LCD_RD��ʱ��ʹ�� */
    LCD_RS_GPIO_CLK_ENABLE();   /* LCD_RS��ʱ��ʹ�� */
    LCD_BL_GPIO_CLK_ENABLE();   /* LCD_BL��ʱ��ʹ�� */
    
    gpio_init_struct.Pin = LCD_CS_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* ���츴�� */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* ���� */
    gpio_init_struct.Alternate = GPIO_AF12_FSMC;            /* ����ΪFSMC */
    HAL_GPIO_Init(LCD_CS_GPIO_PORT, &gpio_init_struct);     /* ��ʼ��LCD_CS���� */

    gpio_init_struct.Pin = LCD_WR_GPIO_PIN;
    HAL_GPIO_Init(LCD_WR_GPIO_PORT, &gpio_init_struct);     /* ��ʼ��LCD_WR���� */

    gpio_init_struct.Pin = LCD_RD_GPIO_PIN;
    HAL_GPIO_Init(LCD_RD_GPIO_PORT, &gpio_init_struct);     /* ��ʼ��LCD_RD���� */

    gpio_init_struct.Pin = LCD_RS_GPIO_PIN;
    HAL_GPIO_Init(LCD_RS_GPIO_PORT, &gpio_init_struct);     /* ��ʼ��LCD_RS���� */

    gpio_init_struct.Pin = LCD_BL_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* ������� */
    HAL_GPIO_Init(LCD_BL_GPIO_PORT, &gpio_init_struct);     /* LCD_BL����ģʽ����(�������) */
#endif
//    rt_pin_mode(LCD_CS, PIN_MODE_OUTPUT);
//    rt_pin_mode(LCD_WR, PIN_MODE_OUTPUT);
//    rt_pin_mode(LCD_RD, PIN_MODE_OUTPUT);
//    rt_pin_mode(LCD_BLK, PIN_MODE_OUTPUT);
//    rt_pin_mode(LCD_RS, PIN_MODE_OUTPUT);
    g_sram_handle.Instance = FSMC_NORSRAM_DEVICE;
    g_sram_handle.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
    
    g_sram_handle.Init.NSBank = FSMC_NORSRAM_BANK4;                        /* ʹ��NE4 */
    g_sram_handle.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;     /* ��ַ/�����߲����� */
    g_sram_handle.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;    /* 16λ���ݿ�� */
    g_sram_handle.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;   /* �Ƿ�ʹ��ͻ������,����ͬ��ͻ���洢����Ч,�˴�δ�õ� */
    g_sram_handle.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW; /* �ȴ��źŵļ���,����ͻ��ģʽ���������� */
    g_sram_handle.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;      /* �洢�����ڵȴ�����֮ǰ��һ��ʱ�����ڻ��ǵȴ������ڼ�ʹ��NWAIT */
    g_sram_handle.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;       /* �洢��дʹ�� */
    g_sram_handle.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;              /* �ȴ�ʹ��λ,�˴�δ�õ� */
    g_sram_handle.Init.ExtendedMode = FSMC_EXTENDED_MODE_ENABLE;           /* ��дʹ�ò�ͬ��ʱ�� */
    g_sram_handle.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;  /* �Ƿ�ʹ��ͬ������ģʽ�µĵȴ��ź�,�˴�δ�õ� */
    g_sram_handle.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;              /* ��ֹͻ��д */
    
    /* FSMC��ʱ����ƼĴ��� */
    fsmc_read_handle.AddressSetupTime = 0x0F;           /* ��ַ����ʱ��(ADDSET)Ϊ15��fsmc_ker_ck(1/168=6)��6*15=90ns */
    fsmc_read_handle.AddressHoldTime = 0x00;            /* ��ַ����ʱ��(ADDHLD) ģʽA��û���õ� */
    fsmc_read_handle.DataSetupTime = 60;                /* ���ݱ���ʱ��(DATAST)Ϊ60��fsmc_ker_ck=6*60=360ns */
                                                        /* ��ΪҺ������IC�Ķ����ݵ�ʱ��,�ٶȲ���̫��,�����Ǹ�������оƬ */
    fsmc_read_handle.AccessMode = FSMC_ACCESS_MODE_A;   /* ģʽA */
    
    /* FSMCдʱ����ƼĴ��� */
    fsmc_write_handle.AddressSetupTime = 9;             /* ��ַ����ʱ��(ADDSET)Ϊ9��fsmc_ker_ck=6*9=54ns */
    fsmc_write_handle.AddressHoldTime = 0x00;           /* ��ַ����ʱ��(ADDHLD) ģʽA��û���õ� */
    fsmc_write_handle.DataSetupTime = 9;                /* ���ݱ���ʱ��(DATAST)Ϊ9��fsmc_ker_ck=6*9=54ns */
                                                        /* ע�⣺ĳЩҺ������IC��д�ź���������Ҳ��50ns */
    fsmc_write_handle.AccessMode = FSMC_ACCESS_MODE_A;  /* ģʽA */
    
    HAL_SRAM_Init(&g_sram_handle, &fsmc_read_handle, &fsmc_write_handle);
//    delay_ms(50);
//    rt_hw_us_delay(50*1000);
    rt_thread_delay(50);

    /* ����9341 ID�Ķ�ȡ */
    lcd_wr_regno(0xD3);
    lcddev.id = lcd_rd_data();  /* dummy read */
    lcddev.id = lcd_rd_data();  /* ����0x00 */
    lcddev.id = lcd_rd_data();  /* ��ȡ93 */
    lcddev.id <<= 8;
    lcddev.id |= lcd_rd_data(); /* ��ȡ41 */

    if (lcddev.id != 0x9341)    /* ���� 9341 , ���Կ����ǲ��� ST7789 */
    {
        lcd_wr_regno(0x04);
        lcddev.id = lcd_rd_data();      /* dummy read */
        lcddev.id = lcd_rd_data();      /* ����0x85 */
        lcddev.id = lcd_rd_data();      /* ��ȡ0x85 */
        lcddev.id <<= 8;
        lcddev.id |= lcd_rd_data();     /* ��ȡ0x52 */
        
        if (lcddev.id == 0x8552)        /* ��8552��IDת����7789 */
        {
            lcddev.id = 0x7789;
        }

        if (lcddev.id != 0x7789)        /* Ҳ����ST7789, �����ǲ��� NT35310 */
        {
            lcd_wr_regno(0xD4);
            lcddev.id = lcd_rd_data();  /* dummy read */
            lcddev.id = lcd_rd_data();  /* ����0x01 */
            lcddev.id = lcd_rd_data();  /* ����0x53 */
            lcddev.id <<= 8;
            lcddev.id |= lcd_rd_data(); /* �������0x10 */

            if (lcddev.id != 0x5310)    /* Ҳ����NT35310,���Կ����ǲ���ST7796 */
            {
                lcd_wr_regno(0XD3);
                lcddev.id = lcd_rd_data();  /* dummy read */
                lcddev.id = lcd_rd_data();  /* ����0X00 */
                lcddev.id = lcd_rd_data();  /* ��ȡ0X77 */
                lcddev.id <<= 8;
                lcddev.id |= lcd_rd_data(); /* ��ȡ0X96 */
                
                if (lcddev.id != 0x7796)    /* Ҳ����ST7796,���Կ����ǲ���NT35510 */
                {
                    /* ������Կ�������ṩ�� */
                    lcd_write_reg(0xF000, 0x0055);
                    lcd_write_reg(0xF001, 0x00AA);
                    lcd_write_reg(0xF002, 0x0052);
                    lcd_write_reg(0xF003, 0x0008);
                    lcd_write_reg(0xF004, 0x0001);
                    
                    lcd_wr_regno(0xC500);       /* ��ȡID�Ͱ�λ */
                    lcddev.id = lcd_rd_data();  /* ����0x80 */
                    lcddev.id <<= 8;

                    lcd_wr_regno(0xC501);       /* ��ȡID�߰�λ */
                    lcddev.id |= lcd_rd_data(); /* ����0x00 */

//                    delay_ms(5);                /* �ȴ�5ms, ��Ϊ0XC501ָ���1963��˵���������λָ��, �ȴ�5ms��1963��λ����ٲ��� */
                    rt_thread_delay(5*1000);
                    
                    if (lcddev.id != 0x5510)    /* Ҳ����NT5510,���Կ����ǲ���ILI9806 */
                    {
                        lcd_wr_regno(0XD3);
                        lcddev.id = lcd_rd_data();  /* dummy read */
                        lcddev.id = lcd_rd_data();  /* ����0X00 */
                        lcddev.id = lcd_rd_data();  /* ����0X98 */
                        lcddev.id <<= 8;
                        lcddev.id |= lcd_rd_data(); /* ����0X06 */
                        
                        if (lcddev.id != 0x9806)    /* Ҳ����ILI9806,���Կ����ǲ���SSD1963 */
                        {
                            lcd_wr_regno(0xA1);
                            lcddev.id = lcd_rd_data();
                            lcddev.id = lcd_rd_data();  /* ����0x57 */
                            lcddev.id <<= 8;
                            lcddev.id |= lcd_rd_data(); /* ����0x61 */

                            if (lcddev.id == 0x5761) lcddev.id = 0x1963; /* SSD1963���ص�ID��5761H,Ϊ��������,����ǿ������Ϊ1963 */
                        }
                    }
                }
            }
        }
    }

    /* �ر�ע��, �����main�����������δ���1��ʼ��, ��Ῠ����printf
     * ����(������f_putc����), ����, �����ʼ������1, �������ε�����
     * ���� printf ��� !!!!!!!
     */
//    printf("LCD ID:%x\r\n", lcddev.id); /* ��ӡLCD ID */

    if (lcddev.id == 0x7789)
    {
        lcd_ex_st7789_reginit();    /* ִ��ST7789��ʼ�� */
    }
    else if (lcddev.id == 0x9341)
    {
        lcd_ex_ili9341_reginit();   /* ִ��ILI9341��ʼ�� */
    }
    else if (lcddev.id == 0x5310)
    {
        lcd_ex_nt35310_reginit();   /* ִ��NT35310��ʼ�� */
    }
    else if (lcddev.id == 0x7796)
    {
        lcd_ex_st7796_reginit();    /* ִ��ST7796��ʼ�� */
    }
    else if (lcddev.id == 0x5510)
    {
        lcd_ex_nt35510_reginit();   /* ִ��NT35510��ʼ�� */
    }
    else if (lcddev.id == 0x9806)
    {
        lcd_ex_ili9806_reginit();   /* ִ��ILI9806��ʼ�� */
    }
    else if (lcddev.id == 0x1963)
    {
        lcd_ex_ssd1963_reginit();   /* ִ��SSD1963��ʼ�� */
        lcd_ssd_backlight_set(100); /* ��������Ϊ���� */
    }

    /* ���ڲ�ͬ��Ļ��дʱ��ͬ�������ʱ����Ը����Լ�����Ļ�����޸�
      �������ϳ����߶�ʱ��Ҳ����Ӱ�죬��Ҫ�Լ���������޸ģ� */
    /* ��ʼ������Ժ�,���� */
    if (lcddev.id == 0x7789)
    {
        /* ��������дʱ����ƼĴ�����ʱ�� */
        fsmc_write_handle.AddressSetupTime = 3; /* ��ַ����ʱ��(ADDSET)Ϊ3��fsmc_ker_ck=6*3=18ns */
        fsmc_write_handle.DataSetupTime = 3;    /* ���ݱ���ʱ��(DATAST)Ϊ3��fsmc_ker_ck=6*3=18ns */
        FSMC_NORSRAM_Extended_Timing_Init(g_sram_handle.Extended, &fsmc_write_handle, g_sram_handle.Init.NSBank, g_sram_handle.Init.ExtendedMode);
    }
    else if (lcddev.id == 0x9806 || lcddev.id == 0x9341 || lcddev.id == 0x5510)
    {
        /* ��������дʱ����ƼĴ�����ʱ�� */
        fsmc_write_handle.AddressSetupTime = 2; /* ��ַ����ʱ��(ADDSET)Ϊ2��fsmc_ker_ck=6*2=12ns */
        fsmc_write_handle.DataSetupTime = 2;    /* ���ݱ���ʱ��(DATAST)Ϊ2��fsmc_ker_ck=6*2=12ns */
        FSMC_NORSRAM_Extended_Timing_Init(g_sram_handle.Extended, &fsmc_write_handle, g_sram_handle.Init.NSBank, g_sram_handle.Init.ExtendedMode);
    }
    else if (lcddev.id == 0x5310 || lcddev.id == 0x7796 || lcddev.id == 0x1963)
    {
        /* ��������дʱ����ƼĴ�����ʱ�� */
        fsmc_write_handle.AddressSetupTime = 1; /* ��ַ����ʱ��(ADDSET)Ϊ1��fsmc_ker_ck=6*1=6ns */
        fsmc_write_handle.DataSetupTime = 1;    /* ���ݱ���ʱ��(DATAST)Ϊ1��fsmc_ker_ck=6*1=6ns */
        FSMC_NORSRAM_Extended_Timing_Init(g_sram_handle.Extended, &fsmc_write_handle, g_sram_handle.Init.NSBank, g_sram_handle.Init.ExtendedMode);
    }

    lcd_display_dir(0); /* Ĭ��Ϊ���� */
    LCD_BL(1);          /* �������� */
    lcd_clear(WHITE);
}

/**
 * @brief       ��������
 * @param       color: Ҫ��������ɫ
 * @retval      ��
 */
void lcd_clear(rt_uint16_t color)
{
    rt_uint32_t index = 0;
    rt_uint32_t totalpoint = lcddev.width;

    totalpoint *= lcddev.height;    /* �õ��ܵ��� */
    lcd_set_cursor(0x00, 0x0000);   /* ���ù��λ�� */
    lcd_write_ram_prepare();        /* ��ʼд��GRAM */

    for (index = 0; index < totalpoint; index++)
    {
        LCD->LCD_RAM = color;
    }
}

/**
 * @brief       ��ָ����������䵥����ɫ
 * @param       (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex - sx + 1) * (ey - sy + 1)
 * @param       color:  Ҫ������ɫ(32λ��ɫ,�������LTDC)
 * @retval      ��
 */
void lcd_fill(rt_uint16_t sx, rt_uint16_t sy, rt_uint16_t ex, rt_uint16_t ey, rt_uint32_t color)
{
    rt_uint16_t i, j;
    rt_uint16_t xlen = 0;
    xlen = ex - sx + 1;

    for (i = sy; i <= ey; i++)
    {
        lcd_set_cursor(sx, i);      /* ���ù��λ�� */
        lcd_write_ram_prepare();    /* ��ʼд��GRAM */

        for (j = 0; j < xlen; j++)
        {
            LCD->LCD_RAM = color;   /* ��ʾ��ɫ */
        }
    }
}

/**
 * @brief       ��ָ�����������ָ����ɫ��
 * @param       (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex - sx + 1) * (ey - sy + 1)
 * @param       color: Ҫ������ɫ�����׵�ַ
 * @retval      ��
 */
void lcd_color_fill(rt_uint16_t sx, rt_uint16_t sy, rt_uint16_t ex, rt_uint16_t ey, rt_uint16_t *color)
{
    rt_uint16_t height, width;
    rt_uint16_t i, j;

    width = ex - sx + 1;            /* �õ����Ŀ�� */
    height = ey - sy + 1;           /* �߶� */

    for (i = 0; i < height; i++)
    {
        lcd_set_cursor(sx, sy + i); /* ���ù��λ�� */
        lcd_write_ram_prepare();    /* ��ʼд��GRAM */

        for (j = 0; j < width; j++)
        {
            LCD->LCD_RAM = color[i * width + j]; /* д������ */
        }
    }
}

/**
 * @brief       ����
 * @param       x1,y1: �������
 * @param       x2,y2: �յ�����
 * @param       color: �ߵ���ɫ
 * @retval      ��
 */
void lcd_draw_line(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2, rt_uint16_t color)
{
    rt_uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;
    delta_x = x2 - x1;      /* ������������ */
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if (delta_x > 0)
    {
        incx = 1;       /* ���õ������� */
    }
    else if (delta_x == 0)
    {
        incx = 0;       /* ��ֱ�� */
    }
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)
    {
        incy = 1;
    }
    else if (delta_y == 0)
    {
        incy = 0;       /* ˮƽ�� */
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y)
    {
        distance = delta_x;  /* ѡȡ�������������� */
    }
    else
    {
        distance = delta_y;
    }

    for (t = 0; t <= distance + 1; t++)     /* ������� */
    {
        lcd_draw_point(row, col, color);    /* ���� */
        xerr += delta_x;
        yerr += delta_y;

        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

/**
 * @brief       ��ˮƽ��
 * @param       x,y   : �������
 * @param       len   : �߳���
 * @param       color : ���ε���ɫ
 * @retval      ��
 */
void lcd_draw_hline(rt_uint16_t x, rt_uint16_t y, rt_uint16_t len, rt_uint16_t color)
{
    if ((len == 0) || (x > lcddev.width) || (y > lcddev.height))
    {
        return;
    }

    lcd_fill(x, y, x + len - 1, y, color);
}

/**
 * @brief       ������
 * @param       x1,y1: �������
 * @param       x2,y2: �յ�����
 * @param       color: ���ε���ɫ
 * @retval      ��
 */
void lcd_draw_rectangle(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2, rt_uint16_t color)
{
    lcd_draw_line(x1, y1, x2, y1, color);
    lcd_draw_line(x1, y1, x1, y2, color);
    lcd_draw_line(x1, y2, x2, y2, color);
    lcd_draw_line(x2, y1, x2, y2, color);
}

/**
 * @brief       ��Բ
 * @param       x0,y0 : Բ��������
 * @param       r     : �뾶
 * @param       color : Բ����ɫ
 * @retval      ��
 */
void lcd_draw_circle(rt_uint16_t x0, rt_uint16_t y0, rt_uint8_t r, rt_uint16_t color)
{
    int a, b;
    int di;

    a = 0;
    b = r;
    di = 3 - (r << 1);       /* �ж��¸���λ�õı�־ */

    while (a <= b)
    {
        lcd_draw_point(x0 + a, y0 - b, color);  /* 5 */
        lcd_draw_point(x0 + b, y0 - a, color);  /* 0 */
        lcd_draw_point(x0 + b, y0 + a, color);  /* 4 */
        lcd_draw_point(x0 + a, y0 + b, color);  /* 6 */
        lcd_draw_point(x0 - a, y0 + b, color);  /* 1 */
        lcd_draw_point(x0 - b, y0 + a, color);
        lcd_draw_point(x0 - a, y0 - b, color);  /* 2 */
        lcd_draw_point(x0 - b, y0 - a, color);  /* 7 */
        a++;

        /* ʹ��Bresenham�㷨��Բ */
        if (di < 0)
        {
            di += 4 * a + 6;
        }
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

/**
 * @brief       ���ʵ��Բ
 * @param       x,y  : Բ��������
 * @param       r    : �뾶
 * @param       color: Բ����ɫ
 * @retval      ��
 */
void lcd_fill_circle(rt_uint16_t x, rt_uint16_t y, rt_uint16_t r, rt_uint16_t color)
{
    rt_uint32_t i;
    rt_uint32_t imax = ((rt_uint32_t)r * 707) / 1000 + 1;
    rt_uint32_t sqmax = (rt_uint32_t)r * (rt_uint32_t)r + (rt_uint32_t)r / 2;
    rt_uint32_t xr = r;

    lcd_draw_hline(x - r, y, 2 * r, color);

    for (i = 1; i <= imax; i++)
    {
        if ((i * i + xr * xr) > sqmax)
        {
            /* draw lines from outside */
            if (xr > imax)
            {
                lcd_draw_hline (x - i + 1, y + xr, 2 * (i - 1), color);
                lcd_draw_hline (x - i + 1, y - xr, 2 * (i - 1), color);
            }

            xr--;
        }

        /* draw lines from inside (center) */
        lcd_draw_hline(x - xr, y + i, 2 * xr, color);
        lcd_draw_hline(x - xr, y - i, 2 * xr, color);
    }
}

/**
 * @brief       ��ָ��λ����ʾһ���ַ�
 * @param       x,y  : ����
 * @param       chr  : Ҫ��ʾ���ַ�:" "--->"~"
 * @param       size : �����С 12/16/24/32
 * @param       mode : ���ӷ�ʽ(1); �ǵ��ӷ�ʽ(0);
 * @param       color : �ַ�����ɫ;
 * @retval      ��
 */
void lcd_show_char(rt_uint16_t x, rt_uint16_t y, char chr, rt_uint8_t size, rt_uint8_t mode, rt_uint16_t color)
{
    rt_uint8_t temp, t1, t;
    rt_uint16_t y0 = y;
    rt_uint8_t csize = 0;
    rt_uint8_t *pfont = 0;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); /* �õ�����һ���ַ���Ӧ������ռ���ֽ��� */
    chr = chr - ' ';    /* �õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩ */

    switch (size)
    {
        case 12:
            pfont = (rt_uint8_t *)asc2_1206[chr];  /* ����1206���� */
            break;

        case 16:
            pfont = (rt_uint8_t *)asc2_1608[chr];  /* ����1608���� */
            break;

        case 24:
            pfont = (rt_uint8_t *)asc2_2412[chr];  /* ����2412���� */
            break;

        case 32:
            pfont = (rt_uint8_t *)asc2_3216[chr];  /* ����3216���� */
            break;

        default:
            return ;
    }

    for (t = 0; t < csize; t++)
    {
        temp = pfont[t];                            /* ��ȡ�ַ��ĵ������� */

        for (t1 = 0; t1 < 8; t1++)                  /* һ���ֽ�8���� */
        {
            if (temp & 0x80)                        /* ��Ч��,��Ҫ��ʾ */
            {
                lcd_draw_point(x, y, color);        /* �������,Ҫ��ʾ����� */
            }
            else if (mode == 0)                     /* ��Ч��,����ʾ */
            {
                lcd_draw_point(x, y, g_back_color); /* ������ɫ,�൱������㲻��ʾ(ע�ⱳ��ɫ��ȫ�ֱ�������) */
            }

            temp <<= 1;                             /* ��λ, �Ա��ȡ��һ��λ��״̬ */
            y++;

            if (y >= lcddev.height)return;          /* �������� */

            if ((y - y0) == size)                   /* ��ʾ��һ����? */
            {
                y = y0; /* y���긴λ */
                x++;    /* x������� */

                if (x >= lcddev.width)
                {
                    return;       /* x���곬������ */
                }

                break;
            }
        }
    }
}

/**
 * @brief       ƽ������, m^n
 * @param       m: ����
 * @param       n: ָ��
 * @retval      m��n�η�
 */
static rt_uint32_t lcd_pow(rt_uint8_t m, rt_uint8_t n)
{
    rt_uint32_t result = 1;

    while (n--)
    {
        result *= m;
    }

    return result;
}

/**
 * @brief       ��ʾlen������
 * @param       x,y : ��ʼ����
 * @param       num : ��ֵ(0 ~ 2^32)
 * @param       len : ��ʾ���ֵ�λ��
 * @param       size: ѡ������ 12/16/24/32
 * @param       color : ���ֵ���ɫ;
 * @retval      ��
 */
void lcd_show_num(rt_uint16_t x, rt_uint16_t y, rt_uint32_t num, rt_uint8_t len, rt_uint8_t size, rt_uint16_t color)
{
    rt_uint8_t t, temp;
    rt_uint8_t enshow = 0;

    for (t = 0; t < len; t++)   /* ������ʾλ��ѭ�� */
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;   /* ��ȡ��Ӧλ������ */

        if (enshow == 0 && t < (len - 1))               /* û��ʹ����ʾ,�һ���λҪ��ʾ */
        {
            if (temp == 0)
            {
                lcd_show_char(x + (size / 2) * t, y, ' ', size, 0, color);  /* ��ʾ�ո�,ռλ */
                continue;       /* �����¸�һλ */
            }
            else
            {
                enshow = 1;     /* ʹ����ʾ */
            }
        }

        lcd_show_char(x + (size / 2) * t, y, temp + '0', size, 0, color);   /* ��ʾ�ַ� */
    }
}

/**
 * @brief       ��չ��ʾlen������(��λ��0Ҳ��ʾ)
 * @param       x,y : ��ʼ����
 * @param       num : ��ֵ(0 ~ 2^32)
 * @param       len : ��ʾ���ֵ�λ��
 * @param       size: ѡ������ 12/16/24/32
 * @param       mode: ��ʾģʽ
 *              [7]:0,�����;1,���0.
 *              [6:1]:����
 *              [0]:0,�ǵ�����ʾ;1,������ʾ.
 * @param       color : ���ֵ���ɫ;
 * @retval      ��
 */
void lcd_show_xnum(rt_uint16_t x, rt_uint16_t y, rt_uint32_t num, rt_uint8_t len, rt_uint8_t size, rt_uint8_t mode, rt_uint16_t color)
{
    rt_uint8_t t, temp;
    rt_uint8_t enshow = 0;

    for (t = 0; t < len; t++)       /* ������ʾλ��ѭ�� */
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;    /* ��ȡ��Ӧλ������ */

        if (enshow == 0 && t < (len - 1))   /* û��ʹ����ʾ,�һ���λҪ��ʾ */
        {
            if (temp == 0)
            {
                if (mode & 0x80)    /* ��λ��Ҫ���0 */
                {
                    lcd_show_char(x + (size / 2) * t, y, '0', size, mode & 0x01, color);    /* ��0ռλ */
                }
                else
                {
                    lcd_show_char(x + (size / 2) * t, y, ' ', size, mode & 0x01, color);    /* �ÿո�ռλ */
                }

                continue;
            }
            else
            {
                enshow = 1;         /* ʹ����ʾ */
            }

        }

        lcd_show_char(x + (size / 2) * t, y, temp + '0', size, mode & 0x01, color);
    }
}

/**
 * @brief       ��ʾ�ַ���
 * @param       x,y         : ��ʼ����
 * @param       width,height: �����С
 * @param       size        : ѡ������ 12/16/24/32
 * @param       p           : �ַ����׵�ַ
 * @param       color       : �ַ�������ɫ;
 * @retval      ��
 */
void lcd_show_string(rt_uint16_t x, rt_uint16_t y, rt_uint16_t width, rt_uint16_t height, rt_uint8_t size, char *p, rt_uint16_t color)
{
    rt_uint8_t x0 = x;
    
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' '))   /* �ж��ǲ��ǷǷ��ַ�! */
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }

        if (y >= height)
        {
            break;      /* �˳� */
        }

        lcd_show_char(x, y, *p, size, 0, color);
        x += size / 2;
        p++;
    }
}















