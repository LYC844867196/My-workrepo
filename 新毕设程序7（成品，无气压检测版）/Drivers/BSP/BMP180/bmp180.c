#include "./BSP/BMP180/bmp180.h"
#include "./BSP/IIC/myiic.h"
#include "./SYSTEM/delay/delay.h"
#include "./USMART/usmart.h"
#include "string.h"

UART_HandleTypeDef huart1;

uint8_t BMP180_SlaveAddress = 0xee;

// calibration datas
short 		    AC1 = 0;
short 		    AC2 = 0;
short 			AC3 = 0;
unsigned short 	AC4 = 0;
unsigned short 	AC5 = 0;
unsigned short 	AC6 = 0;
short 			B1 = 0;
short 			B2 = 0;
short 			MB = 0;
short 			MC = 0;
short 			MD = 0;

//********************************************************************
/**
 * @brief      iic bus read
 * @param[in]  addr is the iic device write address
 * @param[in]  reg is the iic register address
 * @param[out] *buf points to a data buffer
 * @param[in]  len is the length of the data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       addr = device_address_7bits << 1
 */
uint8_t Multiple_read(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    /* send a start */
    iic_start();
    
    /* send the write addr */
    iic_send_byte(addr);
    if (iic_wait_ack() != 0)
    {
        iic_stop();
        
        return 1;
    }
    
    /* send the reg */
    iic_send_byte(reg);
    if (iic_wait_ack() != 0)
    {
        iic_stop();
        
        return 1;
    }
    
    /* send a start */
    iic_start();
    
    /* send the read addr */
    iic_send_byte(addr + 1);
    if (iic_wait_ack() != 0)
    {
        iic_stop();
        
        return 1;
    }
    
    /* read the data */
    while (len != 0)
    {
        /* if the last */
        if (len == 1)
        {
            /* send nack */
            *buf = iic_read_byte(0);
        }
        else
        {
            /* send ack */
            *buf = iic_read_byte(1);
        }
        len--;
        buf++;
    }
    
    /* send a stop */
    iic_stop();
    
    return 0;
}

uint16_t bmp180ReadTemp(void)
{
	int cnt = 0;
	iic_start();                  //起始信号
	iic_send_byte(BMP180_SlaveAddress);   //发送设备地址+写信号   BMP180_SlaveAddress  0xee	气压传感器器件地址
	cnt = 0;
	while(iic_wait_ack())								//这边需要增加超时退出，避免卡死
	{
		cnt++;
		delay_ms(1); 
		if(cnt>=100)return 1;
	} 
	iic_send_byte(0xF4);	          // write register address
	cnt = 0;
	while(iic_wait_ack())								//这边需要增加超时退出，避免卡死
	{
		cnt++;
		delay_ms(1);
		if(cnt>=100)return 1;
	} 
	iic_send_byte(0x2E);       	// write register data for temp
	cnt = 0;
	while(iic_wait_ack())								//这边需要增加超时退出，避免卡死
	{
		cnt++;
		delay_ms(1);
		if(cnt>=100)return 1;
	} 
	iic_stop();                   //发送停止信号
	delay_ms(10);	// max time is 4.5ms
	return Multiple_read(0xF6);
}
//*************************************************************
uint16_t bmp180ReadPressure(void)
{
	int cnt = 0;
	iic_start();                   //起始信号
	iic_send_byte(BMP180_SlaveAddress);   //发送设备地址+写信号
	cnt = 0;
	while(iic_wait_ack())								//这边需要增加超时退出，避免卡死
	{
		cnt++;
		delay_ms(1);
		if(cnt>=100)return 1;
	} 
	iic_send_byte(0xF4);	          // write register address
	cnt = 0;
	while(iic_wait_ack())								//这边需要增加超时退出，避免卡死
	{
		cnt++;
		delay_ms(1);
		if(cnt>=100)return 1;
	} 
	iic_send_byte(0x34);       	  // write register data for pressure
	cnt = 0;
	while(iic_wait_ack())								//这边需要增加超时退出，避免卡死
	{
		cnt++;
		delay_ms(1);
		if(cnt>=100)return 1;
	} 
	iic_stop();                    //发送停止信号
	delay_ms(20);    	                  // max time is 4.5ms
	return Multiple_read(0xF6);//pressure;	
}
 
 
 
//初始化BMP180，根据需要请参考pdf进行修改**************
void Init_BMP180()
{
	char str[100] = "";
	AC1 = Multiple_read(0xAA);
	AC2 = Multiple_read(0xAC);
	AC3 = Multiple_read(0xAE);
	AC4 = Multiple_read(0xB0);
	AC5 = Multiple_read(0xB2);
	AC6 = Multiple_read(0xB4);
	B1 =  Multiple_read(0xB6);
	B2 =  Multiple_read(0xB8);
	MB =  Multiple_read(0xBA);
	MC =  Multiple_read(0xBC);
	MD =  Multiple_read(0xBE);
	
	sprintf(str,"ac1=%d ac2=%d ac3=%d ac4=%d ac5=%d ac6=%d b1=%d b2=%d mb=%d mc=%d md=%d\r\n",(int)AC1,(int)AC2,(int)AC3,(int)AC4,(int)AC5,(int)AC6,(int)B1,(int)B2,(int)MB,(int)MC,(int)MD);
  HAL_UART_Transmit(&huart1, (uint8_t *)str, (uint8_t)strlen(str), 5000);
}
//***********************************************************************
void bmp180Convert()
{
	unsigned int ut;
	unsigned long up;
	long x1, x2, b5, b6, x3, b3, p;
	unsigned long b4, b7;
 
	ut = bmp180ReadTemp();	   // 读取温度
	up = bmp180ReadPressure();  // 读取压强    return pressure;	
	//*************
	x1 = (((long)ut - (long)AC6)*(long)AC5) >> 15;
	x2 = ((long) MC << 11) / (x1 + MD);
	b5 = x1 + x2;
	result_UT = ((b5 + 8) >> 4);
	//*************		
	b6 = b5 - 4000;
	                         // Calculate B3
	x1 = (B2 * (b6 * b6)>>12)>>11;
	x2 = (AC2 * b6)>>11;
	x3 = x1 + x2;
	b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;	
	                         // Calculate B4
	x1 = (AC3 * b6)>>13;
	x2 = (B1 * ((b6 * b6)>>12))>>16;
	x3 = ((x1 + x2) + 2)>>2;
	b4 = (AC4 * (unsigned long)(x3 + 32768))>>15;
	
	b7 = ((unsigned long)(up - b3) * (50000>>OSS));
	if (b7 < 0x80000000)
	p = (b7<<1)/b4;
	else
	p = (b7/b4)<<1;
	
	x1 = (p>>8) * (p>>8);
	x1 = (x1 * 3038)>>16;
	x2 = (-7357 * p)>>16;
	result_UP = p+((x1 + x2 + 3791)>>4);
}
 
 
/****************************************************************
　　　函数名称：BMP085_Get_Altitude			    
　　　函数功能：获取海拔高度值
　　　入口参数：无
　　　出口参数：altitude //int型  2字节，当前海拔高度值
　　　备 注：	 返回的高度值单位为厘米，调用时再换算成带小数的以米为单位的高度值
*****************************************************************/
//double BMP085_Get_Altitude()
//{
//	double altitude;
//	//pressure=(float)pressure;				//获取气压值
//	altitude=44330.0*(1-pow((double)pressure/101325,1/5.255));  	//根据芯片手册提供的公式计算海拔高度
//	//altitude*=100;	 		//转换成厘米单位的高度值，调用时再换算成带小数的高度值，提高精度
//	return altitude;
//}