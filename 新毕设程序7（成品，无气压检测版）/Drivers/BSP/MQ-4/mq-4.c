#include "./BSP/MQ-4/mq-4.h"
#include "./BSP/ADC/adc.h"
#include "./SYSTEM/delay/delay.h"
#include "math.h"
#include "./SYSTEM/usart/usart.h"

//����ƽ��ֵ
uint16_t CH4_Average_Data(uint8_t ADC_Channel, uint8_t order, uint8_t nch)
{
    uint8_t t;
    uint32_t sum = 0;
    uint16_t val = 0;

    for (t = 0; t < ADC_DMA_BUF_SIZE / nch; t++)  /* ÿ��ͨ���ɼ���10������,����10���ۼ� */
    {
        sum += g_adc_dma_buf[(nch * t) + order];      /* ��ͬͨ����ת�������ۼ� */
    }
    val = sum / (ADC_DMA_BUF_SIZE / nch);        /* ȡƽ��ֵ */
    printf("MQ4_val: %d\r\n", val);
    
    return (uint16_t)val;//���������ADCƽ��ֵ
}

//��ȡMQ4�������ĵ�ѹֵ��ʵ����RL�ĵ�ѹ��
float CH4_Get_Vol(uint8_t ADC_Channel, uint8_t order, uint8_t nch)
{
    uint16_t adc_value = 0;//���Ǵ�MQ-7������ģ���ѹ�����ADCת���л�õ�ԭʼ����ֵ����ֵ�ķ�ΧΪ0��4095����ģ���ѹ��ʾΪ����ֵ
    float voltage = 0;//MQ-7������ģ��ĵ�ѹ�������һ����̼��Ũ�ȳ�����
    
    adc_value = CH4_Average_Data(ADC_Channel, order, nch);
    delay_ms(5);
    
    voltage  = (5.0 / 4096.0) * adc_value;
    printf("MQ4_voltage = %f\r\n", voltage);
    
    return voltage;
}

// ���ݹ�ʽ����CO_ppm
float MQ4_GetPPM(uint8_t ADC_Channel, uint8_t order, uint8_t nch)
{
    float Voltage;
    float ppm;
    
    Voltage  = CH4_Get_Vol(ADC_Channel, order, nch);
    ppm = (Voltage - 0.5) / 0.1 * 200;  //��ΪMQ4ֱ�Ӹ�����RL��ѹ��PPM�Ĺ�ϵ���ߣ����Բ���ҪRSת��
    printf("MQ4_ppm : %f\r\n", ppm);
    
    return  ppm;
}
