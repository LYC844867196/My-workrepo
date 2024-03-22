/**
 ****************************************************************************************************
 * @file        exti.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-19
 * @brief       �ⲿ�ж� ��������
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
 * �޸�˵��
 * V1.0 20200420
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/BEEP/beep.h"
#include "./BSP/KEY/key.h"
#include "./BSP/EXTI/exti.h"
#include "./BSP/RTC/rtc.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LCD/lcd.h"


uint8_t set_mode = 0;    /* ģʽ0����������ģʽ��ģʽ1����������ģʽ */

/**
 * @brief       KEY0 �ⲿ�жϷ������
 * @param       ��
 * @retval      ��
 */
void KEY0_INT_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY0_INT_GPIO_PIN);         /* �����жϴ����ú��� ���KEY0�����ж��� ���жϱ�־λ */
    __HAL_GPIO_EXTI_CLEAR_IT(KEY0_INT_GPIO_PIN);         /* HAL��Ĭ�������ж��ٴ���ص����˳�ʱ����һ���жϣ����ⰴ�������󴥷� */
}

/**
 * @brief       KEY1 �ⲿ�жϷ������
 * @param       ��
 * @retval      ��
 */
void KEY1_INT_IRQHandler(void)
{ 
    HAL_GPIO_EXTI_IRQHandler(KEY1_INT_GPIO_PIN);         /* �����жϴ����ú��� ���KEY1�����ж��� ���жϱ�־λ���ж��°벿��HAL_GPIO_EXTI_Callbackִ�� */
    __HAL_GPIO_EXTI_CLEAR_IT(KEY1_INT_GPIO_PIN);         /* HAL��Ĭ�������ж��ٴ���ص����˳�ʱ����һ���жϣ����ⰴ�������󴥷� */
}


/**
 * @brief       WK_UP �ⲿ�жϷ������
 * @param       ��
 * @retval      ��
 */
void WKUP_INT_IRQHandler(void)
{ 
    HAL_GPIO_EXTI_IRQHandler(WKUP_INT_GPIO_PIN);        /* �����жϴ����ú��� ���KEY_UP�����ж��� ���жϱ�־λ���ж��°벿��HAL_GPIO_EXTI_Callbackִ�� */
    __HAL_GPIO_EXTI_CLEAR_IT(WKUP_INT_GPIO_PIN);        /* HAL��Ĭ�������ж��ٴ���ص����˳�ʱ����һ���жϣ����ⰴ�������󴥷� */
}

/**
 * @brief       �жϷ����������Ҫ��������
                ��HAL�������е��ⲿ�жϷ�����������ô˺���
 * @param       GPIO_Pin:�ж����ź�
 * @retval      ��
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    static uint8_t set_shift = 1;   /* ���λ�ã�1~6��Ӧ������ʱ���� */
    uint8_t key; 
    uint8_t tbuf[40];

    
    delay_ms(20);      /* ���� */
    switch(GPIO_Pin)
    {
        case KEY0_INT_GPIO_PIN:
            if (KEY0 == 0)
            {
                LED0_TOGGLE();  /* LED0 ״̬ȡ�� */ 
                LED1_TOGGLE();  /* LED1 ״̬ȡ�� */ 
            }
            break;
        case KEY1_INT_GPIO_PIN:
            if (KEY1 == 0)
            {
                LED0_TOGGLE();  /* LED0 ״̬ȡ�� */ 
            }
            break;
        case WKUP_INT_GPIO_PIN:
            if (WK_UP == 1)     /* ����ģʽ�л������л�����������ģʽ */
            {
                printf("WK_UP����\r\n");
                __HAL_RTC_ALARM_CLEAR_FLAG(&g_rtc_handle, RTC_FLAG_SEC);            /* ������ж� */
                
                while (1)
                {
                    key = key_scan(0);
                    
                    if (key == 1)   /* ���������Լ�1 */
                    {
                        printf("KEY0����\r\n");
                        switch (set_shift)
                        {
                            case 1: calendar.year--; break;
                            case 2: calendar.month--; break;
                            case 3: calendar.date--; break;
                            case 4: calendar.hour--; break;
                            case 5: calendar.min--; break;
                            case 6: calendar.sec--; break;
                            default : break;
                        }
                        sprintf((char *)tbuf, "Time:%02d:%02d:%02d", calendar.hour, calendar.min, calendar.sec);
                        lcd_show_string(30, 120, 210, 16, 16, (char *)tbuf, RED);
                        sprintf((char *)tbuf, "Date:%04d-%02d-%02d", calendar.year, calendar.month, calendar.date);
                        lcd_show_string(30, 140, 210, 16, 16, (char *)tbuf, RED);
                    }
                    if (key == 2)   /* ������������1 */
                    {
                        printf("KEY1����\r\n");
                        switch (set_shift)
                        {
                            case 1: calendar.year++; break;
                            case 2: calendar.month++; break;
                            case 3: calendar.date++; break;
                            case 4: calendar.hour++; break;
                            case 5: calendar.min++; break;
                            case 6: calendar.sec++; break;
                            default : break;
                        }
                        sprintf((char *)tbuf, "Time:%02d:%02d:%02d", calendar.hour, calendar.min, calendar.sec);
                        lcd_show_string(30, 120, 210, 16, 16, (char *)tbuf, RED);
                        sprintf((char *)tbuf, "Date:%04d-%02d-%02d", calendar.year, calendar.month, calendar.date);
                        lcd_show_string(30, 140, 210, 16, 16, (char *)tbuf, RED);
                    }
                    if (key == 3)   /* �л�������ģʽ */
                    {
                        printf("WK_UP����\r\n");
                        rtc_set_time(calendar.year, calendar.month, calendar.date, calendar.hour, calendar.min, calendar.sec);
                        __HAL_RTC_ALARM_ENABLE_IT(&g_rtc_handle, RTC_IT_SEC);   /* �������ж� */
                        break;
                    }
                }
            }
            break;
    }
}

/**
 * @brief       �ⲿ�жϳ�ʼ������
 * @param       ��
 * @retval      ��
 */
void extix_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;

    KEY0_GPIO_CLK_ENABLE();                                  /* KEY0ʱ��ʹ�� */
    KEY1_GPIO_CLK_ENABLE();                                  /* KEY1ʱ��ʹ�� */
    WKUP_GPIO_CLK_ENABLE();                                  /* WKUPʱ��ʹ�� */

    gpio_init_struct.Pin = KEY0_INT_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_IT_FALLING;            /* �����ش��� */
    gpio_init_struct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEY0_INT_GPIO_PORT, &gpio_init_struct);    /* KEY0����Ϊ�½��ش����ж� */

    gpio_init_struct.Pin = KEY1_INT_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_IT_FALLING;            /* �����ش��� */
    gpio_init_struct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEY1_INT_GPIO_PORT, &gpio_init_struct);    /* KEY1����Ϊ�½��ش����ж� */
    
    gpio_init_struct.Pin = WKUP_INT_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_IT_RISING;             /* �����ش��� */
    gpio_init_struct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(WKUP_GPIO_PORT, &gpio_init_struct);        /* WKUP����Ϊ�½��ش����ж� */

    HAL_NVIC_SetPriority(KEY0_INT_IRQn, 0, 2);               /* ��ռ0�������ȼ�2 */
    HAL_NVIC_EnableIRQ(KEY0_INT_IRQn);                       /* ʹ���ж���4 */

    HAL_NVIC_SetPriority(KEY1_INT_IRQn, 1, 2);               /* ��ռ1�������ȼ�2 */
    HAL_NVIC_EnableIRQ(KEY1_INT_IRQn);                       /* ʹ���ж���3 */

    HAL_NVIC_SetPriority(WKUP_INT_IRQn, 2, 2);               /* ��ռ2�������ȼ�2 */
    HAL_NVIC_EnableIRQ(WKUP_INT_IRQn);                       /* ʹ���ж���0 */
}












