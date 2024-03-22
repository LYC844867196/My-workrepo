#ifndef __MQ_7_H
#define __MQ_7_H

#include "./SYSTEM/sys/sys.h"

//#define ADC3_CHY_GPIO_PORT                  GPIOA
//#define ADC3_CHY_GPIO_PIN                   GPIO_PIN_1 
//#define ADC3_CHY_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)  /* PA口时钟使能 */

#define CO_READ_TIMES   10          //定义CO传感器读取次数,读这么多次,然后取平均值
#define CO_MIN_PPM         10         //洁净空气中的一氧化碳浓度（传感器最小浓度值）
#define RL              10  // RL阻值
//#define R0              16  // R0阻值

//void mq_7_init(void);
//uint16_t ADC2_Average_Data(uint8_t ADC_Channel);
//float CO_Get_Vol(void);
//float MQ7_GetPPM(void);

uint16_t CO_Average_Data(uint8_t ADC_Channel, uint8_t order, uint8_t nch);
float CO_Get_Vol(uint8_t ADC_Channel, uint8_t order, uint8_t nch);
float MQ7_GetPPM(uint8_t ADC_Channel, uint8_t order, uint8_t nch);

#endif
