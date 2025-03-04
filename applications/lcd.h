/**
 ****************************************************************************************************
 * @file        lcd.h
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

#ifndef __LCD_H
#define __LCD_H

#include "stdlib.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

/******************************************************************************************/
/* LCD RST/WR/RD/BL/CS/RS ���� ����
 * LCD_D0~D15,��������̫��,�Ͳ������ﶨ����,ֱ����lcd_init�����޸�.��������ֲ��ʱ��,���˸�
 * ��6��IO��, ���ø�LCD_Init�����D0~D15���ڵ�IO��.
 */

/* RESET ��ϵͳ��λ�Ź��� �������ﲻ�ö��� RESET���� */
//#define LCD_RST_GPIO_PORT               GPIOx
//#define LCD_RST_GPIO_PIN                SYS_GPIO_PINx
//#define LCD_RST_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOx_CLK_ENABLE(); }while(0)   /* ����IO��ʱ��ʹ�� */
#if 1
#define LCD_WR_GPIO_PORT                GPIOD
#define LCD_WR_GPIO_PIN                 GPIO_PIN_5
#define LCD_WR_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)     /* ����IO��ʱ��ʹ�� */

#define LCD_RD_GPIO_PORT                GPIOD
#define LCD_RD_GPIO_PIN                 GPIO_PIN_4
#define LCD_RD_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)     /* ����IO��ʱ��ʹ�� */

#define LCD_BL_GPIO_PORT                GPIOB
#define LCD_BL_GPIO_PIN                 GPIO_PIN_15
#define LCD_BL_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)     /* ��������IO��ʱ��ʹ�� */

/* LCD_CS(��Ҫ����LCD_FSMC_NEX������ȷ��IO��) �� LCD_RS(��Ҫ����LCD_FSMC_AX������ȷ��IO��) ���� ���� */
#define LCD_CS_GPIO_PORT                GPIOG
#define LCD_CS_GPIO_PIN                 GPIO_PIN_12
#define LCD_CS_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)     /* ����IO��ʱ��ʹ�� */

#define LCD_RS_GPIO_PORT                GPIOF
#define LCD_RS_GPIO_PIN                 GPIO_PIN_12
#define LCD_RS_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)     /* ����IO��ʱ��ʹ�� */
#endif
///** 定义引脚*/
//#define LCD_WR    GET_PIN(D, 5)
//#define LCD_RD    GET_PIN(D, 4)
//#define LCD_BLK    GET_PIN(D, 15)
//#define LCD_CS    GET_PIN(D, 12)
//#define LCD_RS    GET_PIN(F, 12)

/* FSMC��ز��� ����
 * ע��: ����Ĭ����ͨ��FSMC��1������LCD, ��1��4��Ƭѡ: FSMC_NE1~4
 *
 * �޸�LCD_FSMC_NEX, ��Ӧ��LCD_CS_GPIO�������Ҳ�ø�
 * �޸�LCD_FSMC_AX , ��Ӧ��LCD_RS_GPIO�������Ҳ�ø�
 */
#define LCD_FSMC_NEX         4              /* ʹ��FSMC_NE4��LCD_CS,ȡֵ��Χֻ����: 1~4 */
#define LCD_FSMC_AX          6              /* ʹ��FSMC_A6��LCD_RS,ȡֵ��Χ��: 0 ~ 25 */

#define LCD_FSMC_BCRX        FSMC_Bank1->BTCR[(LCD_FSMC_NEX - 1) * 2]       /* BCR�Ĵ���,����LCD_FSMC_NEX�Զ����� */
#define LCD_FSMC_BTRX        FSMC_Bank1->BTCR[(LCD_FSMC_NEX - 1) * 2 + 1]   /* BTR�Ĵ���,����LCD_FSMC_NEX�Զ����� */
#define LCD_FSMC_BWTRX       FSMC_Bank1E->BWTR[(LCD_FSMC_NEX - 1) * 2]      /* BWTR�Ĵ���,����LCD_FSMC_NEX�Զ����� */

/******************************************************************************************/

/* LCD��Ҫ������ */
typedef struct
{
    rt_uint16_t width;     /* LCD ��� */
    rt_uint16_t height;    /* LCD �߶� */
    rt_uint16_t id;        /* LCD ID */
    rt_uint8_t dir;        /* ���������������ƣ�0��������1�������� */
    rt_uint16_t wramcmd;   /* ��ʼдgramָ�� */
    rt_uint16_t setxcmd;   /* ����x����ָ�� */
    rt_uint16_t setycmd;   /* ����y����ָ�� */
} _lcd_dev;

/* LCD���� */
extern _lcd_dev lcddev; /* ����LCD��Ҫ���� */

/* LCD�Ļ�����ɫ�ͱ���ɫ */
extern rt_uint32_t  g_point_color;     /* Ĭ�Ϻ�ɫ */
extern rt_uint32_t  g_back_color;      /* ������ɫ.Ĭ��Ϊ��ɫ */

/* LCD������� */
#define LCD_BL(x)   do{ x ? \
                      HAL_GPIO_WritePin(LCD_BL_GPIO_PORT, LCD_BL_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LCD_BL_GPIO_PORT, LCD_BL_GPIO_PIN, GPIO_PIN_RESET); \
                     }while(0)
/* LCD��ַ�ṹ�� */
typedef struct
{
    volatile rt_uint16_t LCD_REG;
    volatile rt_uint16_t LCD_RAM;
} LCD_TypeDef;


/* LCD_BASE����ϸ���㷽��:
 * ����һ��ʹ��FSMC�Ŀ�1(BANK1)������TFTLCDҺ����(MCU��), ��1��ַ��Χ�ܴ�СΪ256MB,���ֳ�4��:
 * �洢��1(FSMC_NE1)��ַ��Χ: 0x6000 0000 ~ 0x63FF FFFF
 * �洢��2(FSMC_NE2)��ַ��Χ: 0x6400 0000 ~ 0x67FF FFFF
 * �洢��3(FSMC_NE3)��ַ��Χ: 0x6800 0000 ~ 0x6BFF FFFF
 * �洢��4(FSMC_NE4)��ַ��Χ: 0x6C00 0000 ~ 0x6FFF FFFF
 *
 * ������Ҫ����Ӳ�����ӷ�ʽѡ����ʵ�Ƭѡ(����LCD_CS)�͵�ַ��(����LCD_RS)
 * ̽����F407������ʹ��FSMC_NE4����LCD_CS, FSMC_A6����LCD_RS ,16λ������,���㷽������:
 * ����FSMC_NE4�Ļ���ַΪ: 0x6C00 0000;     NEX�Ļ�ַΪ(x=1/2/3/4): 0x6000 0000 + (0x400 0000 * (x - 1))
 * FSMC_A6��Ӧ��ֵַ: 2^6 * 2 = 0x80;    FSMC_Ay��Ӧ�ĵ�ַΪ(y = 0 ~ 25): 2^y * 2
 *
 * LCD->LCD_REG,��ӦLCD_RS = 0(LCD�Ĵ���); LCD->LCD_RAM,��ӦLCD_RS = 1(LCD����)
 * �� LCD->LCD_RAM�ĵ�ַΪ:  0x6C00 0000 + 2^6 * 2 = 0x6C00 0080
 *    LCD->LCD_REG�ĵ�ַ����Ϊ LCD->LCD_RAM֮��������ַ.
 * ��������ʹ�ýṹ�����LCD_REG �� LCD_RAM(REG��ǰ,RAM�ں�,��Ϊ16λ���ݿ��)
 * ��� �ṹ��Ļ���ַ(LCD_BASE) = LCD_RAM - 2 = 0x6C00 0080 -2
 *
 * ����ͨ�õļ��㹫ʽΪ((Ƭѡ��FSMC_NEX)X=1/2/3/4, (RS�ӵ�ַ��FSMC_Ay)y=0~25):
 *          LCD_BASE = (0x6000 0000 + (0x400 0000 * (x - 1))) | (2^y * 2 -2)
 *          ��Ч��(ʹ����λ����)
 *          LCD_BASE = (0x6000 0000 + (0x400 0000 * (x - 1))) | ((1 << y) * 2 -2)
 */
#define LCD_BASE        (rt_uint32_t)((0x60000000 + (0x4000000 * (LCD_FSMC_NEX - 1))) | (((1 << LCD_FSMC_AX) * 2) -2))
#define LCD             ((LCD_TypeDef *) LCD_BASE)

/******************************************************************************************/
/* LCDɨ�跽�����ɫ ���� */

/* ɨ�跽���� */
#define L2R_U2D         0           /* ������,���ϵ��� */
#define L2R_D2U         1           /* ������,���µ��� */
#define R2L_U2D         2           /* ���ҵ���,���ϵ��� */
#define R2L_D2U         3           /* ���ҵ���,���µ��� */

#define U2D_L2R         4           /* ���ϵ���,������ */
#define U2D_R2L         5           /* ���ϵ���,���ҵ��� */
#define D2U_L2R         6           /* ���µ���,������ */
#define D2U_R2L         7           /* ���µ���,���ҵ��� */

#define DFT_SCAN_DIR    L2R_U2D     /* Ĭ�ϵ�ɨ�跽�� */

/* ���û�����ɫ */
#define WHITE           0xFFFF      /* ��ɫ */
#define BLACK           0x0000      /* ��ɫ */
#define RED             0xF800      /* ��ɫ */
#define GREEN           0x07E0      /* ��ɫ */
#define BLUE            0x001F      /* ��ɫ */
#define MAGENTA         0xF81F      /* Ʒ��ɫ/�Ϻ�ɫ = BLUE + RED */
#define YELLOW          0xFFE0      /* ��ɫ = GREEN + RED */
#define CYAN            0x07FF      /* ��ɫ = GREEN + BLUE */

/* �ǳ�����ɫ */
#define BROWN           0xBC40      /* ��ɫ */
#define BRRED           0xFC07      /* �غ�ɫ */
#define GRAY            0x8430      /* ��ɫ */
#define DARKBLUE        0x01CF      /* ����ɫ */
#define LIGHTBLUE       0x7D7C      /* ǳ��ɫ */
#define GRAYBLUE        0x5458      /* ����ɫ */
#define LIGHTGREEN      0x841F      /* ǳ��ɫ */
#define LGRAY           0xC618      /* ǳ��ɫ(PANNEL),���屳��ɫ */
#define LGRAYBLUE       0xA651      /* ǳ����ɫ(�м����ɫ) */
#define LBBLUE          0x2B12      /* ǳ����ɫ(ѡ����Ŀ�ķ�ɫ) */

/******************************************************************************************/
/* SSD1963������ò���(һ�㲻�ø�) */

/* LCD�ֱ������� */
#define SSD_HOR_RESOLUTION      800     /* LCDˮƽ�ֱ��� */
#define SSD_VER_RESOLUTION      480     /* LCD��ֱ�ֱ��� */

/* LCD������������ */
#define SSD_HOR_PULSE_WIDTH     1       /* ˮƽ���� */
#define SSD_HOR_BACK_PORCH      46      /* ˮƽǰ�� */
#define SSD_HOR_FRONT_PORCH     210     /* ˮƽ���� */

#define SSD_VER_PULSE_WIDTH     1       /* ��ֱ���� */
#define SSD_VER_BACK_PORCH      23      /* ��ֱǰ�� */
#define SSD_VER_FRONT_PORCH     22      /* ��ֱǰ�� */

/* ���¼����������Զ����� */
#define SSD_HT          (SSD_HOR_RESOLUTION + SSD_HOR_BACK_PORCH + SSD_HOR_FRONT_PORCH)
#define SSD_HPS         (SSD_HOR_BACK_PORCH)
#define SSD_VT          (SSD_VER_RESOLUTION + SSD_VER_BACK_PORCH + SSD_VER_FRONT_PORCH)
#define SSD_VPS         (SSD_VER_BACK_PORCH)
   
/******************************************************************************************/
/* �������� */

void lcd_ex_st7789_reginit(void);
void lcd_ex_ili9341_reginit(void);
void lcd_ex_nt35310_reginit(void);
void lcd_ex_st7796_reginit(void);
void lcd_ex_nt35510_reginit(void);
void lcd_ex_ili9806_reginit(void);
void lcd_ex_ssd1963_reginit(void);

void lcd_wr_data(volatile rt_uint16_t data);            /* LCDд���� */
void lcd_wr_regno(volatile rt_uint16_t regno);          /* LCDд�Ĵ������/��ַ */
void lcd_write_reg(rt_uint16_t regno, rt_uint16_t data);   /* LCDд�Ĵ�����ֵ */

void lcd_init(void);                        /* ��ʼ��LCD */
void lcd_display_on(void);                  /* ����ʾ */
void lcd_display_off(void);                 /* ����ʾ */
void lcd_scan_dir(rt_uint8_t dir);             /* ������ɨ�跽�� */
void lcd_display_dir(rt_uint8_t dir);          /* ������Ļ��ʾ���� */
void lcd_ssd_backlight_set(rt_uint8_t pwm);    /* SSD1963 ������� */

void lcd_write_ram_prepare(void);                           /* ׼��дGRAM */
void lcd_set_cursor(rt_uint16_t x, rt_uint16_t y);                /* ���ù�� */
rt_uint32_t lcd_read_point(rt_uint16_t x, rt_uint16_t y);            /* ����(32λ��ɫ,����LTDC) */
void lcd_draw_point(rt_uint16_t x, rt_uint16_t y, rt_uint32_t color);/* ����(32λ��ɫ,����LTDC) */

void lcd_clear(rt_uint16_t color);                                                             /* LCD���� */
void lcd_fill_circle(rt_uint16_t x, rt_uint16_t y, rt_uint16_t r, rt_uint16_t color);                   /* ���ʵ��Բ */
void lcd_draw_circle(rt_uint16_t x0, rt_uint16_t y0, rt_uint8_t r, rt_uint16_t color);                  /* ��Բ */
void lcd_draw_hline(rt_uint16_t x, rt_uint16_t y, rt_uint16_t len, rt_uint16_t color);                  /* ��ˮƽ�� */
void lcd_set_window(rt_uint16_t sx, rt_uint16_t sy, rt_uint16_t width, rt_uint16_t height);             /* ���ô��� */
void lcd_fill(rt_uint16_t sx, rt_uint16_t sy, rt_uint16_t ex, rt_uint16_t ey, rt_uint32_t color);          /* ��ɫ������(32λ��ɫ,����LTDC) */
void lcd_color_fill(rt_uint16_t sx, rt_uint16_t sy, rt_uint16_t ex, rt_uint16_t ey, rt_uint16_t *color);   /* ��ɫ������ */
void lcd_draw_line(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2, rt_uint16_t color);     /* ��ֱ�� */
void lcd_draw_rectangle(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2, rt_uint16_t color);/* ������ */

void lcd_show_char(rt_uint16_t x, rt_uint16_t y, char chr, rt_uint8_t size, rt_uint8_t mode, rt_uint16_t color);                       /* ��ʾһ���ַ� */
void lcd_show_num(rt_uint16_t x, rt_uint16_t y, rt_uint32_t num, rt_uint8_t len, rt_uint8_t size, rt_uint16_t color);                     /* ��ʾ���� */
void lcd_show_xnum(rt_uint16_t x, rt_uint16_t y, rt_uint32_t num, rt_uint8_t len, rt_uint8_t size, rt_uint8_t mode, rt_uint16_t color);      /* ��չ��ʾ���� */
void lcd_show_string(rt_uint16_t x, rt_uint16_t y, rt_uint16_t width, rt_uint16_t height, rt_uint8_t size, char *p, rt_uint16_t color);   /* ��ʾ�ַ��� */

#endif

















