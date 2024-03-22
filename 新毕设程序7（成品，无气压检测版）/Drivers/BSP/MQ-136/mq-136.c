#include "./BSP/MQ-136/mq-136.h"
#include "./BSP/ADC/adc.h"
#include "./SYSTEM/delay/delay.h"
#include "math.h"
#include "./SYSTEM/usart/usart.h"

//����ADCƽ��ֵ
uint16_t MQ136_Average_Data(uint8_t ADC_Channel, uint8_t order, uint8_t nch)
{
    uint8_t t;
    uint32_t sum = 0;
    uint16_t val = 0;

    for (t = 0; t < ADC_DMA_BUF_SIZE / nch; t++)  /* ÿ��ͨ���ɼ���10������,����10���ۼ� */
    {
        sum += g_adc_dma_buf[(nch * t) + order];      /* ��ͬͨ����ת�������ۼ� */
    }
    val = sum / (ADC_DMA_BUF_SIZE / nch);        /* ȡƽ��ֵ */
//    printf("val: %d\r\n", val);
    
    return (uint16_t)val;//���������ADCƽ��ֵ
}

//��ȡMQ7�������ĵ�ѹֵ
float MQ136_Get_Vol(uint8_t ADC_Channel, uint8_t order, uint8_t nch)
{
    uint16_t adc_value = 0;//���Ǵ�MQ-7������ģ���ѹ�����ADCת���л�õ�ԭʼ����ֵ����ֵ�ķ�ΧΪ0��4095����ģ���ѹ��ʾΪ����ֵ
    float voltage = 0;//MQ-7������ģ��ĵ�ѹ�������һ����̼��Ũ�ȳ�����
    
    adc_value = MQ136_Average_Data(ADC_Channel, order, nch);
    delay_ms(5);
    
    voltage  = (5.0/4096.0)*(adc_value);
//    printf("voltage: %f\r\n", voltage);
    
    return voltage;
}

// ���ݹ�ʽ����CO_ppm
float MQ136_GetPPM(uint8_t ADC_Channel, uint8_t order, uint8_t nch)
{
    float Voltage = 0;
    float ppm = 0;
    
    Voltage = MQ136_Get_Vol(ADC_Channel, order, nch);
    
////    ppm = pow((3.4880 * 10 * Voltage) / (5 - Voltage), (1.0 / 0.3203));
    ppm = 450.25 * pow(Voltage, 5) - 3049.4 * pow(Voltage, 4) + 8088.3 * pow(Voltage, 3) - 10422 * pow(Voltage, 2)
        + 6524.1 * Voltage - 1586.1 * pow(MQ136_R0, 2) - 0.998;
    printf("MQ136_ppm: %f\r\n", ppm);
    
    return  ppm;
}
