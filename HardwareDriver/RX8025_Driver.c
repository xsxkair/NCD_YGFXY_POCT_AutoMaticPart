/***************************************************************************************************
*FileName:
*Description:
*Author: xsx_kair
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"RX8025_Driver.h"

#include	"MyMem.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"

#include 	"delay.h"
/***************************************************************************************************/
/******************************************Static Variables*****************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/******************************************Static Methods*******************************************/
/***************************************************************************************************/
static void RX_SDA_IN(void);
static void RX_SDA_OUT(void);
static void RX_IIC_Start(void);
static void RX_IIC_Stop(void);
static unsigned char RX8025_Write(unsigned char addr, unsigned char *pdata, unsigned char len);
static unsigned char RX8025_Read(unsigned char addr, unsigned char *pdata, unsigned char len);
static unsigned char BCD2HEX(unsigned char bcd_data);
static unsigned char HEX2BCD(unsigned char hex_data);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/****************************************File Start*************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
void RTC_BSPInit(void)
{
	GPIO_InitTypeDef    GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RX_SCK_Rcc | RX_SDA_Rcc, ENABLE);

	GPIO_InitStructure.GPIO_Pin = RX_SCK_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(RX_SCK_Group, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = RX_SDA_Pin;
    GPIO_Init(RX_SDA_Group, &GPIO_InitStructure);
}

/***************************************************************************************************
*FunctionName：RX_SDA_IN
*Description：时钟芯片数据线更改配置为输出
*Input：None
*Output：None
*Author：xsx
*Data�2016年9月18日10:10:12
***************************************************************************************************/
static void RX_SDA_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = RX_SDA_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(RX_SDA_Group, &GPIO_InitStructure);
}
/***************************************************************************************************
*FunctionName：RX_SDA_OUT
*Description：时钟芯片数据线更改配置为输出
*Input：None
*Output：None
*Author：xsx
*Data：2016年9月18日10:10:16
***************************************************************************************************/
static void RX_SDA_OUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = RX_SDA_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(RX_SDA_Group, &GPIO_InitStructure);
}

static unsigned char BCD2HEX(unsigned char bcd_data)  
{   
    unsigned char temp;   
    temp=(bcd_data>>4)*10 + (bcd_data&0x0f);
    return temp;
}
static unsigned char HEX2BCD(unsigned char hex_data)  
{   
    unsigned char temp; 
    temp=((hex_data/10)<<4) + (hex_data%10);
    return temp; 	
}

/***************************************************************************************************
*FunctionName:RX_IIC_Start 
*Description: IIC总线起始信号
*Input: 
*Output: 
*Author: xsx
*Date: 2016年9月18日10:18:20
***************************************************************************************************/
static void RX_IIC_Start(void)
{
	RX_SDA_OUT();
	
	RX_SDA_H();
	RX_SCK_H();
	delay_us(RX_DelayTime);
	
	RX_SDA_L();
	delay_us(RX_DelayTime);
	
	RX_SCK_L();
	delay_us(RX_DelayTime);
}

/***************************************************************************************************
*FunctionName:RX_IIC_Stop 
*Description: IIC总线结束信号
*Input: 
*Output: 
*Author: xsx
*Date: 2016年9月18日10:18:35
***************************************************************************************************/
static void RX_IIC_Stop(void)
{
	RX_SDA_OUT();
	
	RX_SCK_L();
	RX_SDA_L();
	delay_us(RX_DelayTime);
	
	RX_SCK_H();
	delay_us(RX_DelayTime);
	
	RX_SDA_H();
	delay_us(RX_DelayTime);
}

static unsigned char RX_IIC_Wait_Ack(void)
{
	unsigned char errortime = 0;
	
	RX_SDA_IN();
	RX_SCK_H();
	delay_us(RX_DelayTime);
	
	while(RX_SDA_PIN())
	{
		errortime++;
		
		if(errortime > 250)
		{
			RX_IIC_Stop();
			return 0;
		}
	}
	delay_us(RX_DelayTime);
	RX_SCK_L();
	return 1;
}

//产生ACK应答
void RX_IIC_Ack(void)
{
	RX_SCK_L();
	
	RX_SDA_OUT();
	RX_SDA_L();
	delay_us(RX_DelayTime);
	
	RX_SCK_H();
	delay_us(RX_DelayTime);
	RX_SCK_L();
	delay_us(RX_DelayTime);
}
//不产生ACK应答		    
void RX_IIC_NAck(void)
{
	RX_SCK_L();
	RX_SDA_OUT();
	RX_SDA_H();
	delay_us(RX_DelayTime);
	
	RX_SCK_H();
	delay_us(RX_DelayTime);
	RX_SCK_L();
	delay_us(RX_DelayTime);
}

static unsigned char RX_IIC_WriteByte(unsigned char data)
{
	unsigned char i=0;
	
	RX_SDA_OUT();
	
	RX_SCK_L();
	delay_us(RX_DelayTime);
	for(i=0; i<8; i++)
	{		
		if(data&0x80)
			RX_SDA_H();
		else
			RX_SDA_L();
		delay_us(RX_DelayTime);
		
		RX_SCK_H();
		delay_us(RX_DelayTime);
		
		RX_SCK_L();
		delay_us(RX_DelayTime);
		
		data <<= 1;
	}
	
	return RX_IIC_Wait_Ack();
}

static unsigned char RX_IIC_ReadByte(unsigned char ack)
{
	unsigned char i, data = 0;
	
	RX_SDA_IN();
	
	for(i=0; i<8; i++)
	{
		RX_SCK_L();
		delay_us(RX_DelayTime);
		
		RX_SCK_H();
		
		data <<= 1;
		
		if(RX_SDA_PIN())
			data++;
		delay_us(RX_DelayTime);
	}
	
	if(!ack)
		RX_IIC_NAck();
	else
		RX_IIC_Ack();
	
	return data;
}

static unsigned char RX8025_Write(unsigned char addr, unsigned char *pdata, unsigned char len)
{
	unsigned char i=0;
	
	RX_IIC_Start();
	
	if(RX_IIC_WriteByte(0x64) == 0)
	{
		RX_IIC_Stop();
		return 0;
	}
	
	if(RX_IIC_WriteByte(addr) == 0)
	{
		RX_IIC_Stop();
		return 0;
	}
	
	for(i=0; i<len; i++)
	{
		if(RX_IIC_WriteByte(*pdata) == 0)
		{
			RX_IIC_Stop();
			return 0;
		}
		pdata++;
	}
	
	RX_IIC_Stop();
	
	return 1;
}

static unsigned char RX8025_Read(unsigned char addr, unsigned char *pdata, unsigned char len)
{
	unsigned char i=0;
	
	RX_IIC_Start();
	
	if(RX_IIC_WriteByte(0x64) == 0)
	{
		RX_IIC_Stop();
		return 0;
	}
	
	if(RX_IIC_WriteByte(addr) == 0)
	{
		RX_IIC_Stop();
		return 0;
	}
	
	RX_IIC_Start();
	
	if(RX_IIC_WriteByte(0x65) == 0)
	{
		RX_IIC_Stop();
		return 0;
	}
	
	for(i=0; i<len-1; i++)
	{
		*pdata++ = RX_IIC_ReadByte(1);
	}
	
	*pdata++ = RX_IIC_ReadByte(0);
	
	RX_IIC_Stop();
	
	return 1;
}

/***************************************************************************************************
*FunctionName: RTC_SetTimeData
*Description: 设置rtc时间
*Input: data -- 必须为DateTime结构的指针
*Output: 
*Author: xsx
*Date: 2016年9月18日16:11:52
***************************************************************************************************/
MyRes RTC_SetTimeData(DateTime * data)
{
	unsigned char buf[7];
	
	/*设置时间*/
	buf[0] = HEX2BCD(data->sec);
	buf[1] = HEX2BCD(data->min);
	buf[2] = HEX2BCD(data->hour);
	buf[3] = HEX2BCD(0);
	
	/*设置日期*/
	buf[4] = HEX2BCD(data->day);
	buf[5] = HEX2BCD(data->month);
	buf[6] = HEX2BCD(data->year);
	
	if(RX8025_Write(0, buf, 7) == 0)
		return My_Fail;
	else
		return My_Pass;
}

MyRes RTC_SetTimeData2(char * buf)
{
	unsigned short temp = 0;
	DateTime temptime;
	MyRes status = My_Fail;
	
	char * tempbuf = NULL;
	
	tempbuf = MyMalloc(64);
	
	if(tempbuf && buf)
	{
		memset(tempbuf, 0, 64);
		memcpy(tempbuf, buf, 4);
		temp = strtol(tempbuf, NULL, 10);
		if((temp >= 2016) && (temp <= 2100))
			temptime.year = temp-2000;
		else
			goto END;
		
		memset(tempbuf, 0, 64);
		memcpy(tempbuf, buf+4, 2);
		temp = strtol(tempbuf, NULL, 10);
		if((temp >= 1) && (temp <= 12))
			temptime.month = temp;
		else
			goto END;
			
		memset(tempbuf, 0, 64);
		memcpy(tempbuf, buf+6, 2);
		temp = strtol(tempbuf, NULL, 10);
		if((temp >= 1) && (temp <= 31))
			temptime.day = temp;
		else
			goto END;
			
		memset(tempbuf, 0, 64);
		memcpy(tempbuf, buf+8, 2);
		temp = strtol(tempbuf, NULL, 10);
		if(temp <= 23)
			temptime.hour = temp;
		else
			goto END;
			
		memset(tempbuf, 0, 64);
		memcpy(tempbuf, buf+10, 2);
		temp = strtol(tempbuf, NULL, 10);
		if(temp <= 59)
			temptime.min = temp;
		else
			goto END;
			
		memset(tempbuf, 0, 64);
		memcpy(tempbuf, buf+12, 2);
		temp = strtol(tempbuf, NULL, 10);
		if(temp <= 59)
			temptime.sec = temp+2;
		else
			goto END;
		
		status = RTC_SetTimeData(&temptime);
	}
	
	END:
		MyFree(tempbuf);
		return status;
}

void RTC_GetTimeData(DateTime * time)
{
	unsigned char buf[16];
	unsigned char tempV = 0;
	
	RX8025_Read(0, buf, 16);
	
	tempV = BCD2HEX(buf[6]);
	if((tempV >= 16) && (tempV <= 99))
		time->year = tempV;  
	else
		goto END;
	
	tempV = BCD2HEX(buf[5]);
	if((tempV >= 1) && (tempV <= 12))
		time->month = tempV;
	else
		goto END;
	
	tempV = BCD2HEX(buf[4]);
	if((tempV >= 1) && (tempV <= 31))
		time->day = tempV;
	else
		goto END;
	
	tempV = BCD2HEX(buf[2]);
	if(tempV <= 23)
		time->hour = tempV;
	else
		goto END;
	
	tempV = BCD2HEX(buf[1]);
	if(tempV <= 59)
		time->min = tempV;
	else
		goto END;
	
	tempV = BCD2HEX(buf[0]);
	if(tempV <= 59)
		time->sec = tempV;
	else
		goto END;
	
	return;
	
	END:
		time->year = 0;
		time->month = 1;
		time->day = 1;
		time->hour = 0;
		time->min = 0;
		time->sec = 0;
}

/****************************************end of file************************************************/
