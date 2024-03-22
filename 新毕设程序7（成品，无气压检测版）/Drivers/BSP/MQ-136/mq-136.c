#include "./BSP/MQ-136/mq-136.h"
#include "./BSP/ADC/adc.h"
#include "./SYSTEM/delay/delay.h"
#include "math.h"
#include "./SYSTEM/usart/usart.h"

//计算ADC平均值
uint16_t MQ136_Average_Data(uint8_t ADC_Channel, uint8_t order, uint8_t nch)
{
    uint8_t t;
    uint32_t sum = 0;
    uint16_t val = 0;

    for (t = 0; t < ADC_DMA_BUF_SIZE / nch; t++)  /* 每个通道采集了10次数据,进行10次累加 */
    {
        sum += g_adc_dma_buf[(nch * t) + order];      /* 相同通道的转换数据累加 */
    }
    val = sum / (ADC_DMA_BUF_SIZE / nch);        /* 取平均值 */
//    printf("val: %d\r\n", val);
    
    return (uint16_t)val;//返回算出的ADC平均值
}

//读取MQ7传感器的电压值
float MQ136_Get_Vol(uint8_t ADC_Channel, uint8_t order, uint8_t nch)
{
    uint16_t adc_value = 0;//这是从MQ-7传感器模块电压输出的ADC转换中获得的原始数字值，该值的范围为0到4095，将模拟电压表示为数字值
    float voltage = 0;//MQ-7传感器模块的电压输出，与一氧化碳的浓度成正比
    
    adc_value = MQ136_Average_Data(ADC_Channel, order, nch);
    delay_ms(5);
    
    voltage  = (5.0/4096.0)*(adc_value);
//    printf("voltage: %f\r\n", voltage);
    
    return voltage;
}

// 根据公式计算CO_ppm
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
