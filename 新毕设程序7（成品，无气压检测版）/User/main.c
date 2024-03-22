/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-23
 * @brief       多通道ADC采集(DMA读取) 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./USMART/usmart.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/ADC/adc.h"
#include "./BSP/MQ-7/mq-7.h"
#include "./BSP/MQ-4/mq-4.h"
#include "./BSP/MQ-136/mq-136.h"
#include "./BSP/DHT11/dht11.h"
#include "./BSP/RTC/rtc.h"
#include "./BSP/EXTI/exti.h"
#include "./BSP/BEEP/beep.h"

/* 定义字符数组用于显示周 */
char* weekdays[]={"Sunday","Monday","Tuesday","Wednesday",
                  "Thursday","Friday","Saterday"};

uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];   /* ADC DMA BUF */

extern uint8_t g_adc_dma_sta;               /* DMA传输状态标志, 0,未完成; 1, 已完成 */

int main(void)
{
    uint16_t j;
    float CO_ppm = 0;
    float CH4_ppm = 0;
    float H2S_ppm = 0;
    uint8_t temperature;
    uint8_t humidity;
    uint8_t tbuf[40];
    
    HAL_Init();                                 /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9);         /* 设置时钟, 72Mhz */
    delay_init(72);                             /* 延时初始化 */
    usart_init(115200);                         /* 串口初始化为115200 */
    led_init();                                 /* 初始化LED */
    lcd_init();                                 /* 初始化LCD */
    rtc_init();                             /* 初始化RTC */
    extix_init();                          /* 初始化外部中断输入 */
    
    while (dht11_init()) /* DHT11初始化 */
    {
        printf("dht11 init error\r\n");
        lcd_show_string(30, 180, 200, 16, 16, "DHT11 Init Error", RED);
        delay_ms(200);
        lcd_fill(30, 180, 239, 179 + 16, WHITE);
        delay_ms(200);
    }
    
    lcd_show_string(30, 50, 200, 16, 16, "Name: LiuNanHuang", RED);
    lcd_show_string(30, 70, 200, 16, 16, "Name: LiuYiChen", RED);
    
    lcd_show_string(30, 180, 200, 16, 16, "DHT11 OK", RED);
    lcd_show_string(30, 200, 200, 16, 16, "Temp:00C", BLUE);
    lcd_show_string(30, 220, 200, 16, 16, "Humi:00%", BLUE);
    
    lcd_show_string(30, 240, 200, 16, 16, "MQ-7 OK", RED);
    lcd_show_string(30, 260, 200, 16, 16, "C0:00ppm", BLUE);
    
    lcd_show_string(30, 280, 200, 16, 16, "MQ-4 OK", RED);
    lcd_show_string(30, 300, 200, 16, 16, "CH4:00ppm", BLUE);
    
    lcd_show_string(30, 320, 200, 16, 16, "MQ-135 OK", RED);
    lcd_show_string(30, 340, 200, 16, 16, "H2S:00ppm", BLUE);
    
    adc_nch_dma_init((uint32_t)&g_adc_dma_buf); /* 初始化ADC DMA采集 */
    
    adc_dma_enable(ADC_DMA_BUF_SIZE);   /* 启动ADC DMA采集 */

    while (1)
    {
        delay_ms(10);
        
        rtc_get_time();
        sprintf((char *)tbuf, "Time:%02d:%02d:%02d", calendar.hour, calendar.min, calendar.sec);
        printf("Time =\r\n");
        lcd_show_string(30, 120, 210, 16, 16, (char *)tbuf, RED);
        sprintf((char *)tbuf, "Date:%04d-%02d-%02d", calendar.year, calendar.month, calendar.date);
        lcd_show_string(30, 140, 210, 16, 16, (char *)tbuf, RED);
        sprintf((char *)tbuf, "Week:%s", weekdays[calendar.week]);
        lcd_show_string(30, 160, 210, 16, 16, (char *)tbuf, RED);
        
        /* 读取并显示DHT11数据 */
        dht11_read_data(&temperature, &humidity);             /* 读取温湿度值 */
        printf("temperature = %d\r\n", temperature);
        printf("humidity = %d\r\n", humidity);
        lcd_show_num(30 + 40, 200, temperature, 2, 16, BLUE); /* 显示温度 */
        lcd_show_num(30 + 40, 220, humidity, 2, 16, BLUE);    /* 显示湿度 */

        if (g_adc_dma_sta == 1)
        {
                /* 循环显示通道1通道2的结果 */
                for(j = 0; j < 3; j++)  /* 遍历3个通道 */
                {
                    if (j == 0)
                    {
                        CO_ppm = MQ7_GetPPM(ADC_CHANNEL_1, j, 2);
                        lcd_show_num(30 + 24, 260, CO_ppm, 2, 16, BLUE);
                        if (CO_ppm >= 24)
                        {
                            BEEP(1);
                            LED1_TOGGLE();
                        }

                    }
                    if (j == 1)
                    {
                        CH4_ppm = MQ4_GetPPM(ADC_CHANNEL_5, j, 2);
                        lcd_show_num(30 + 32, 300, CH4_ppm, 2, 16, BLUE);
                        if (CH4_ppm >= 10000)
                        {
                            BEEP(1);
                            LED1_TOGGLE();
                        }
                    }
                    if (j == 2)
                    {
                        H2S_ppm = MQ136_GetPPM(ADC_CHANNEL_6, j, 2);
                        lcd_show_num(30 + 32, 340, H2S_ppm, 2, 16, BLUE);
                        if (H2S_ppm >= 10)
                        {
                            BEEP(1);
                            LED1_TOGGLE();
                        }
                    }
                }
            g_adc_dma_sta = 0;                      /* 清除DMA采集完成状态标志 */
            adc_dma_enable(ADC_DMA_BUF_SIZE);       /* 启动下一次ADC DMA采集 */
        }

        LED0_TOGGLE();
        delay_ms(100);
    }
}



















