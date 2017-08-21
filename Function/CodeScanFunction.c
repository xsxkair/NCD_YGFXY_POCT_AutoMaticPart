/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"CodeScanner_Driver.h"
#include	"CodeScanFunction.h"

#include	"MyEncryptTool.h"
#include	"QueueUnits.h"
#include	"ItemConst_Data.h"
#include	"Motor2_Fun.h"
#include	"Motor4_Fun.h"
#include 	"Usart3_Driver.h"
#include	"CardCheck_Driver.h"

#include	"Define.h"
#include	"CRC16.h"
#include	"MyMem.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/

/******************************************************************************************/
/*****************************************局部函数声明*************************************/
static void ReadBasicCodeData(ReadQRCodeBuffer * readQRCodeBuffer);
static void AnalysisCode(ReadQRCodeBuffer * readQRCodeBuffer);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

ScanCodeResult ScanCodeFun(QRCode * cardQR)
{
	ReadQRCodeBuffer * readQRCodeBuffer;
	ScanCodeResult scanResult = CardCodeScanning;
	
	readQRCodeBuffer = MyMalloc(ReadQRCodeBufferStructSize);
	if(readQRCodeBuffer)
	{
		memset(readQRCodeBuffer, 0, ReadQRCodeBufferStructSize);
		
		readQRCodeBuffer->cardQR = cardQR;
		memset(readQRCodeBuffer->cardQR, 0, QRCodeStructSize);
		
		motor4MoveTo(Motor4_CardLocation, 5000);
		motor2MoveTo(Motor2_WaitCardLocation, 10000);
		
		OpenCodeScanner();
	
		while(pdPASS == ReceiveDataFromQueue(GetUsart3RXQueue(), NULL, readQRCodeBuffer->originalcode , MAX_QR_CODE_LENGHT, NULL, 1, 10 / portTICK_RATE_MS, 10 / portTICK_RATE_MS))
			;
		
		//设置定时器
		timer_SetAndStart(&(readQRCodeBuffer->timer), MAX_SCAN_QR_TIME);
		
		readQRCodeBuffer->motorLocation = getMotorxLocation(Motor_2);
	
		while(readQRCodeBuffer->scanResult == CardCodeScanning)
		{
			if(OFF == readCaedCheckStatus())				//卡被拔出
			{
				readQRCodeBuffer->scanResult = CardCodeCardOut;
			}
			else
			{
				readQRCodeBuffer->motorDir++;
				
				if(readQRCodeBuffer->motorDir%2 == 0)
					motor2MoveTo(55000, 1000);
				else
					motor2MoveTo(48000, 1000);
				
				if(readQRCodeBuffer->motorLocation % 50 == 0)
					ReadBasicCodeData(readQRCodeBuffer);

				vTaskDelay(2 / portTICK_RATE_MS);
				
				if(TimerOut == timer_expired(&(readQRCodeBuffer->timer)))
					readQRCodeBuffer->scanResult = CardCodeScanTimeOut;				
			}
		}
	}

	CloseCodeScanner();
	
	scanResult = readQRCodeBuffer->scanResult;
	
	MyFree(readQRCodeBuffer);
	
	return scanResult;
}

/***************************************************************************************************
*FunctionName：ReadBasicCodeData
*Description：读取原始二维码
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月25日14:07:38
***************************************************************************************************/
static void ReadBasicCodeData(ReadQRCodeBuffer * readQRCodeBuffer)
{
	if(readQRCodeBuffer == NULL)
		return;
	
	memset(readQRCodeBuffer->originalcode, 0, MAX_QR_CODE_LENGHT);
	ReceiveDataFromQueue(GetUsart3RXQueue(), NULL, readQRCodeBuffer->originalcode , MAX_QR_CODE_LENGHT, &(readQRCodeBuffer->originalCodeLen), 1, 10 / portTICK_RATE_MS
		, 10 / portTICK_RATE_MS);
	
	if(readQRCodeBuffer->originalCodeLen > 0)
	{
		readQRCodeBuffer->originalCodeLen -= 1;
		AnalysisCode(readQRCodeBuffer);
	}
}

/***************************************************************************************************
*FunctionName：DecryptCode
*Description：解码并解析二维码数据
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月25日14:08:25
***************************************************************************************************/
static void AnalysisCode(ReadQRCodeBuffer * readQRCodeBuffer)
{
	unsigned char i=0;
	
	if(readQRCodeBuffer == NULL)
		return;
	
	/*清空二维码空间*/
	memset(readQRCodeBuffer->decryptcode, 0, MAX_QR_CODE_LENGHT);

	/*数据解密失败*/
	if(pdFAIL == MyDencrypt(readQRCodeBuffer->originalcode, readQRCodeBuffer->decryptcode, readQRCodeBuffer->originalCodeLen))
		goto END;
	
	memcpy(readQRCodeBuffer->originalcode, readQRCodeBuffer->decryptcode, readQRCodeBuffer->originalCodeLen);
	readQRCodeBuffer->pbuf2 = readQRCodeBuffer->originalcode;
	
	/*获取测试项目名称*/
	readQRCodeBuffer->pbuf1 = strtok(readQRCodeBuffer->decryptcode, "#");
	if(readQRCodeBuffer->pbuf1)
	{
		memcpy(readQRCodeBuffer->cardQR->ItemName, readQRCodeBuffer->pbuf1 ,strlen(readQRCodeBuffer->pbuf1));
		if(getItemConstData(&(readQRCodeBuffer->cardQR->itemConstData), readQRCodeBuffer->cardQR->ItemName) == My_Fail)
		{
			readQRCodeBuffer->scanResult = CardUnsupported;
			goto END;
		}
	}
	else
		goto END;
		
	//读取检测卡上的检测通道
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		readQRCodeBuffer->cardQR->ChannelNum = strtol(readQRCodeBuffer->pbuf1, NULL, 10);
	else
		goto END;
	
	/*读取检测卡T线位置*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		readQRCodeBuffer->cardQR->ItemLocation = strtol(readQRCodeBuffer->pbuf1 , NULL, 10);
	else
		goto END;
		
	/*读取检测卡标准曲线临界浓度1*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		readQRCodeBuffer->cardQR->ItemFenDuan[0] = strtod(readQRCodeBuffer->pbuf1 , NULL);
	else
		goto END;
	
	/*读取检测卡标准曲线临界浓度2*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		readQRCodeBuffer->cardQR->ItemFenDuan[1] = strtod(readQRCodeBuffer->pbuf1 , NULL);
	else
		goto END;

	/*标准曲线1*/
	for(i=0; i<3; i++)
	{
		readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
		if(readQRCodeBuffer->pbuf1)
			readQRCodeBuffer->cardQR->ItemBiaoQu[0][i] = strtod(readQRCodeBuffer->pbuf1 , NULL);
		else
			goto END;
	}
	
	/*标准曲线2*/
	if(readQRCodeBuffer->cardQR->ItemFenDuan[0] > 0)
	{
		for(i=0; i<3; i++)
		{
			readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
			if(readQRCodeBuffer->pbuf1)
				readQRCodeBuffer->cardQR->ItemBiaoQu[1][i] = strtod(readQRCodeBuffer->pbuf1 , NULL);
			else
				goto END;
		}
	}
	
	/*标准曲线3*/
	if(readQRCodeBuffer->cardQR->ItemFenDuan[1] > 0)
	{
		for(i=0; i<3; i++)
		{
			readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
			if(readQRCodeBuffer->pbuf1)
				readQRCodeBuffer->cardQR->ItemBiaoQu[2][i] = strtod(readQRCodeBuffer->pbuf1 , NULL);
			else
				goto END;
		}
	}
		
	/*读取检测卡反应时间*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		readQRCodeBuffer->cardQR->CardWaitTime = strtol(readQRCodeBuffer->pbuf1 , NULL , 10);
	else
		goto END;
		
	/*读取检测卡C线位置*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		readQRCodeBuffer->cardQR->CLineLocation = strtol(readQRCodeBuffer->pbuf1 , NULL , 10);
	else
		goto END;


	/*读取检测卡批号*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		memcpy(readQRCodeBuffer->cardQR->PiHao, readQRCodeBuffer->pbuf1, strlen(readQRCodeBuffer->pbuf1));
	else
		goto END;
	
	
	/*读取检测卡批号*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		memcpy(readQRCodeBuffer->cardQR->piNum, readQRCodeBuffer->pbuf1, strlen(readQRCodeBuffer->pbuf1));
	else
		goto END;

	/*读取检测卡保质期*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
	{
		static char year[10] ,month[10],day[10];
		memcpy(year, readQRCodeBuffer->pbuf1, 2);
		readQRCodeBuffer->cardQR->CardBaoZhiQi.year = strtol(year , NULL , 10);

		memcpy(month, &readQRCodeBuffer->pbuf1[2], 2);
		readQRCodeBuffer->cardQR->CardBaoZhiQi.month = (unsigned char)strtod(month , NULL );

		memcpy(day, &readQRCodeBuffer->pbuf1[4], 2);
		readQRCodeBuffer->cardQR->CardBaoZhiQi.day = (unsigned char)strtod(day , NULL );
	}
	else
		goto END;

	/*读取二维码CRC*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
	{
		readQRCodeBuffer->cardQR->CRC16 = strtol(readQRCodeBuffer->pbuf1 , NULL , 10);
			
		readQRCodeBuffer->tempV1 = strlen(readQRCodeBuffer->pbuf1);
		goto END;
	}
	else
		goto END;
	
	END:
		if(readQRCodeBuffer->scanResult != CardCodeScanning)
			return;
		else if(readQRCodeBuffer->cardQR->CRC16 != CalModbusCRC16Fun(readQRCodeBuffer->pbuf2 , readQRCodeBuffer->originalCodeLen - 
			readQRCodeBuffer->tempV1, NULL))
			readQRCodeBuffer->scanResult = CardCodeCRCError;		
		//else if(My_Fail == CheckCardIsTimeOut(readQRCodeBuffer))
		//	readQRCodeBuffer->scanResult = CardCodeTimeOut;
		else
			readQRCodeBuffer->scanResult = CardCodeScanOK;
}

