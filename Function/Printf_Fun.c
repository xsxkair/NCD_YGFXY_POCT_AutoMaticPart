/***************************************************************************************************
*FileName:
*Description:
*Author: xsx_kair
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"Printf_Fun.h"
#include 	"Usart3_Driver.h"
#include	"SystemSet_Data.h"
#include	"System_Data.h"
#include	"QueueUnits.h"
#include	"MyMem.h"

#include 	"FreeRTOS.h"
#include 	"queue.h"
#include	"semphr.h"

#include	<string.h>
#include	"stdio.h"
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/****************************************File Start*************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

void PrintfData(TestData * testd2)
{
	char * printfbuf = NULL;
	TestData * tempTestData = NULL;
	DateTime mytime;
	float tempvalue = 0.0;
	
	printfbuf = MyMalloc(100);
	tempTestData = MyMalloc(sizeof(TestData));

	if(printfbuf && testd2 && tempTestData)
	{
		//首先复制数据到自己的缓冲器，以防多任务下，其他任务释放原数据区
		memcpy(tempTestData, testd2, sizeof(TestData));
		
		sprintf(printfbuf, "武汉纽康度生物科技股份有限公司\n\0");
		SendDataToQueue(GetUsart3TXQueue(), NULL, printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 50 / portTICK_RATE_MS, EnableUsart3TXInterrupt);
		
		sprintf(printfbuf, "测试人: %s\n\0", tempTestData->operator.name);
		SendDataToQueue(GetUsart3TXQueue(), NULL, printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 50 / portTICK_RATE_MS, EnableUsart3TXInterrupt);
		
		sprintf(printfbuf, "样品编号: %s\n\0", tempTestData->sampleid);
		SendDataToQueue(GetUsart3TXQueue(), NULL, printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 50 / portTICK_RATE_MS, EnableUsart3TXInterrupt);
		
		sprintf(printfbuf, "测试项目: %s\n\0", tempTestData->qrCode.ItemName);
		SendDataToQueue(GetUsart3TXQueue(), NULL, printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 50 / portTICK_RATE_MS, EnableUsart3TXInterrupt);
		
		tempvalue = tempTestData->testSeries.BasicResult;
		if(tempTestData->testResultDesc != ResultIsOK)
			sprintf(printfbuf, "测试结果: ERROR\n\0");
		else if(IsShowRealValue() == true)
			sprintf(printfbuf, "测试结果: %.*f %-8.8s\n\0", tempTestData->qrCode.itemConstData.pointNum, tempTestData->testSeries.BasicResult, tempTestData->qrCode.itemConstData.itemMeasure);
		else if(tempvalue <= tempTestData->qrCode.itemConstData.lowstResult)
			sprintf(printfbuf, "测试结果: <%.*f %-8.8s\n\0", tempTestData->qrCode.itemConstData.pointNum, tempTestData->qrCode.itemConstData.lowstResult, tempTestData->qrCode.itemConstData.itemMeasure);
		else if(tempvalue >= tempTestData->qrCode.itemConstData.highestResult)
			sprintf(printfbuf, "测试结果: >%.*f %-8.8s\n\0", tempTestData->qrCode.itemConstData.pointNum, tempTestData->qrCode.itemConstData.highestResult, tempTestData->qrCode.itemConstData.itemMeasure);
		else
			sprintf(printfbuf, "测试结果: %.*f %-8.8s\n\0", tempTestData->qrCode.itemConstData.pointNum, tempTestData->testSeries.BasicResult, tempTestData->qrCode.itemConstData.itemMeasure);
		
		SendDataToQueue(GetUsart3TXQueue(), NULL, printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 50 / portTICK_RATE_MS, EnableUsart3TXInterrupt);
		
		sprintf(printfbuf, "参考值: %s\n\0", tempTestData->qrCode.itemConstData.normalResult);
		SendDataToQueue(GetUsart3TXQueue(), NULL, printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 50 / portTICK_RATE_MS, EnableUsart3TXInterrupt);
		
		sprintf(printfbuf, "测试时间: 20%02d-%02d-%02d %02d:%02d:%02d\n\0", tempTestData->testDateTime.year, tempTestData->testDateTime.month, tempTestData->testDateTime.day
			, tempTestData->testDateTime.hour, tempTestData->testDateTime.min, tempTestData->testDateTime.sec);
		SendDataToQueue(GetUsart3TXQueue(), NULL, printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 50 / portTICK_RATE_MS, EnableUsart3TXInterrupt);
		
		memcpy(&mytime, &(getSystemRunTimeData()->systemDateTime), sizeof(DateTime));
		sprintf(printfbuf, "打印时间: 20%02d-%02d-%02d %02d:%02d:%02d\n\0", mytime.year, mytime.month, mytime.day
			, mytime.hour, mytime.min, mytime.sec);
		SendDataToQueue(GetUsart3TXQueue(), NULL, printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 50 / portTICK_RATE_MS, EnableUsart3TXInterrupt);
		
		sprintf(printfbuf, "声明: 本结果仅对本标本负责！\n\n\n\n\n\0");
		SendDataToQueue(GetUsart3TXQueue(), NULL, printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 50 / portTICK_RATE_MS, EnableUsart3TXInterrupt);
	}
	
	MyFree(tempTestData);
	MyFree(printfbuf);
}

/****************************************end of file************************************************/
