#ifndef __MQ_4_H
#define __MQ_4_H

#include "./SYSTEM/sys/sys.h"

#define MQ4_READ_TIMES   10          //定义CO传感器读取次数,读这么多次,然后取平均值
#define CH4_MIN_PPM         10         //洁净空气中的一氧化碳浓度（传感器最小浓度值）

//void mq_4_init(void);
//uint16_t ADC2_Average_Data(uint8_t ADC_Channel);
//float MQ4_Get_Vol(void);
//float MQ4_GetPPM(void);

uint16_t CH4_Average_Data(uint8_t ADC_Channel, uint8_t order, uint8_t nch);
float CH4_Get_Vol(uint8_t ADC_Channel, uint8_t order, uint8_t nch);
float MQ4_GetPPM(uint8_t ADC_Channel, uint8_t order, uint8_t nch);

#endif
