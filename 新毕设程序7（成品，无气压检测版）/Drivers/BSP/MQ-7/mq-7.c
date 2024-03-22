#include "./BSP/MQ-7/mq-7.h"
#include "./BSP/ADC/adc.h"
#include "./SYSTEM/delay/delay.h"
#include "math.h"
#include "./SYSTEM/usart/usart.h"

float MQ7_R0 = 0;

//计算平均值
uint16_t CO_Average_Data(uint8_t ADC_Channel, uint8_t order, uint8_t nch)
{
    uint8_t t;
    uint32_t sum = 0;
    uint16_t val = 0;

    for (t = 0; t < ADC_DMA_BUF_SIZE / nch; t++)  /* 每个通道采集了10次数据,进行10次累加 */
    {
        sum += g_adc_dma_buf[(nch * t) + order];      /* 相同通道的转换数据累加 */
    }
    val = sum / (ADC_DMA_BUF_SIZE / nch);        /* 取平均值 */
    printf("MQ7_val: %d\r\n", val);
    
    return (uint16_t)val;//返回算出的ADC平均值
}

//读取MQ7传感器的电压值
float CO_Get_Vol(uint8_t ADC_Channel, uint8_t order, uint8_t nch)
{
    uint16_t adc_value = 0;//这是从MQ-7传感器模块电压输出的ADC转换中获得的原始数字值，该值的范围为0到4095，将模拟电压表示为数字值
    float voltage = 0;//MQ-7传感器模块的电压输出，与一氧化碳的浓度成正比
    
    adc_value = CO_Average_Data(ADC_Channel, order, nch);
    delay_ms(5);
    voltage  = (3.3/4096.0)*(adc_value);
    printf("MQ7_voltage = %f\r\n", voltage);
    
    return voltage;
}

// 传感器校准函数
void MQ7_PPM_Calibration(float RS)
{
    MQ7_R0 = RS / pow(CO_MIN_PPM / 98.322f, 1 / -1.458f);
    printf("MQ7_R0: %f\r\n", MQ7_R0);
}

// 根据公式计算CO_ppm
float MQ7_GetPPM(uint8_t ADC_Channel, uint8_t order, uint8_t nch)
{
    float RS = 0;
    float ppm = 0;
    
    RS = (3.3f - CO_Get_Vol(ADC_Channel, order, nch)) / CO_Get_Vol(ADC_Channel, order, nch) * RL; 
        //Rs/RL = (Vc - VRL) / VRL      源自传感器基本测试电路
    MQ7_PPM_Calibration(RS);
    ppm = 98.322f * pow(RS/MQ7_R0, -1.458f);    //源自公式拟合曲线的软件
    printf("MQ7_ppm = %f\r\n", ppm);    //打印出负值，超出传感器检测范围的最低值
    
    return  ppm;
}
