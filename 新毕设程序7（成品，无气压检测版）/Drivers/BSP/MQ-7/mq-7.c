#include "./BSP/MQ-7/mq-7.h"
#include "./BSP/ADC/adc.h"
#include "./SYSTEM/delay/delay.h"
#include "math.h"
#include "./SYSTEM/usart/usart.h"

float MQ7_R0 = 0;

//����ƽ��ֵ
uint16_t CO_Average_Data(uint8_t ADC_Channel, uint8_t order, uint8_t nch)
{
    uint8_t t;
    uint32_t sum = 0;
    uint16_t val = 0;

    for (t = 0; t < ADC_DMA_BUF_SIZE / nch; t++)  /* ÿ��ͨ���ɼ���10������,����10���ۼ� */
    {
        sum += g_adc_dma_buf[(nch * t) + order];      /* ��ͬͨ����ת�������ۼ� */
    }
    val = sum / (ADC_DMA_BUF_SIZE / nch);        /* ȡƽ��ֵ */
    printf("MQ7_val: %d\r\n", val);
    
    return (uint16_t)val;//���������ADCƽ��ֵ
}

//��ȡMQ7�������ĵ�ѹֵ
float CO_Get_Vol(uint8_t ADC_Channel, uint8_t order, uint8_t nch)
{
    uint16_t adc_value = 0;//���Ǵ�MQ-7������ģ���ѹ�����ADCת���л�õ�ԭʼ����ֵ����ֵ�ķ�ΧΪ0��4095����ģ���ѹ��ʾΪ����ֵ
    float voltage = 0;//MQ-7������ģ��ĵ�ѹ�������һ����̼��Ũ�ȳ�����
    
    adc_value = CO_Average_Data(ADC_Channel, order, nch);
    delay_ms(5);
    voltage  = (3.3/4096.0)*(adc_value);
    printf("MQ7_voltage = %f\r\n", voltage);
    
    return voltage;
}

// ������У׼����
void MQ7_PPM_Calibration(float RS)
{
    MQ7_R0 = RS / pow(CO_MIN_PPM / 98.322f, 1 / -1.458f);
    printf("MQ7_R0: %f\r\n", MQ7_R0);
}

// ���ݹ�ʽ����CO_ppm
float MQ7_GetPPM(uint8_t ADC_Channel, uint8_t order, uint8_t nch)
{
    float RS = 0;
    float ppm = 0;
    
    RS = (3.3f - CO_Get_Vol(ADC_Channel, order, nch)) / CO_Get_Vol(ADC_Channel, order, nch) * RL; 
        //Rs/RL = (Vc - VRL) / VRL      Դ�Դ������������Ե�·
    MQ7_PPM_Calibration(RS);
    ppm = 98.322f * pow(RS/MQ7_R0, -1.458f);    //Դ�Թ�ʽ������ߵ����
    printf("MQ7_ppm = %f\r\n", ppm);    //��ӡ����ֵ��������������ⷶΧ�����ֵ
    
    return  ppm;
}
