#ifndef __MQ_136_H
#define __MQ_136_H

#include "./SYSTEM/sys/sys.h"

#define MQ136_READ_TIMES   10          //定义CO传感器读取次数,读这么多次,然后取平均值
#define MQ136_RL              4.7  // RL阻值
#define MQ136_R0              1  // R0阻值

uint16_t MQ136_Average_Data(uint8_t ADC_Channel, uint8_t order, uint8_t nch);
float MQ136_Get_Vol(uint8_t ADC_Channel, uint8_t order, uint8_t nch);
float MQ136_GetPPM(uint8_t ADC_Channel, uint8_t order, uint8_t nch);

#endif
