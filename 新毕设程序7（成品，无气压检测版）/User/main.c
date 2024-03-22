/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-23
 * @brief       ��ͨ��ADC�ɼ�(DMA��ȡ) ʵ��
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
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

/* �����ַ�����������ʾ�� */
char* weekdays[]={"Sunday","Monday","Tuesday","Wednesday",
                  "Thursday","Friday","Saterday"};

uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];   /* ADC DMA BUF */

extern uint8_t g_adc_dma_sta;               /* DMA����״̬��־, 0,δ���; 1, ����� */

int main(void)
{
    uint16_t j;
    float CO_ppm = 0;
    float CH4_ppm = 0;
    float H2S_ppm = 0;
    uint8_t temperature;
    uint8_t humidity;
    uint8_t tbuf[40];
    
    HAL_Init();                                 /* ��ʼ��HAL�� */
    sys_stm32_clock_init(RCC_PLL_MUL9);         /* ����ʱ��, 72Mhz */
    delay_init(72);                             /* ��ʱ��ʼ�� */
    usart_init(115200);                         /* ���ڳ�ʼ��Ϊ115200 */
    led_init();                                 /* ��ʼ��LED */
    lcd_init();                                 /* ��ʼ��LCD */
    rtc_init();                             /* ��ʼ��RTC */
    extix_init();                          /* ��ʼ���ⲿ�ж����� */
    
    while (dht11_init()) /* DHT11��ʼ�� */
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
    
    adc_nch_dma_init((uint32_t)&g_adc_dma_buf); /* ��ʼ��ADC DMA�ɼ� */
    
    adc_dma_enable(ADC_DMA_BUF_SIZE);   /* ����ADC DMA�ɼ� */

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
        
        /* ��ȡ����ʾDHT11���� */
        dht11_read_data(&temperature, &humidity);             /* ��ȡ��ʪ��ֵ */
        printf("temperature = %d\r\n", temperature);
        printf("humidity = %d\r\n", humidity);
        lcd_show_num(30 + 40, 200, temperature, 2, 16, BLUE); /* ��ʾ�¶� */
        lcd_show_num(30 + 40, 220, humidity, 2, 16, BLUE);    /* ��ʾʪ�� */

        if (g_adc_dma_sta == 1)
        {
                /* ѭ����ʾͨ��1ͨ��2�Ľ�� */
                for(j = 0; j < 3; j++)  /* ����3��ͨ�� */
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
            g_adc_dma_sta = 0;                      /* ���DMA�ɼ����״̬��־ */
            adc_dma_enable(ADC_DMA_BUF_SIZE);       /* ������һ��ADC DMA�ɼ� */
        }

        LED0_TOGGLE();
        delay_ms(100);
    }
}



















