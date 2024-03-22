#ifndef __MQ_136_H
#define __MQ_136_H

#include "./SYSTEM/sys/sys.h"

#define MQ136_READ_TIMES   10          //����CO��������ȡ����,����ô���,Ȼ��ȡƽ��ֵ
#define MQ136_RL              4.7  // RL��ֵ
#define MQ136_R0              1  // R0��ֵ

uint16_t MQ136_Average_Data(uint8_t ADC_Channel, uint8_t order, uint8_t nch);
float MQ136_Get_Vol(uint8_t ADC_Channel, uint8_t order, uint8_t nch);
float MQ136_GetPPM(uint8_t ADC_Channel, uint8_t order, uint8_t nch);

#endif
