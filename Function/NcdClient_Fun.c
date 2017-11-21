/***************************************************************************************************
*FileName:
*Description:
*Author:
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"NcdClient_Fun.h"
#include	"TestDataDao.h"
#include	"DeviceDao.h"
#include	"SIM800_Fun.h"
#include	"StringDefine.h"
#include	"RX8025_Driver.h"
#include	"RemoteSoftDao.h"
#include	"System_Data.h"
#include	"IAP_Fun.h"

#include	"HttpBuf.h"
#include	"MyMem.h"
#include	"CRC16.h"

#include	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"
/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/
static void readServerTime(HttpBuf * httpBuf);
static void UpLoadDeviceInfo(HttpBuf * httpBuf);
static void UpLoadTestData(HttpBuf * httpBuf);
static void readRemoteFirmwareVersion(HttpBuf * httpBuf);
static void DownLoadFirmware(HttpBuf * httpBuf);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

void NcdClientFunction(void)
{
	static HttpBuf * httpBuf = NULL;
	
	while(1)
	{
		//upload data every hour
		if(My_Pass == takeGSMMutex(1000 / portTICK_RATE_MS))
		{
				//vTaskDelay(60000 / portTICK_RATE_MS);
			
			httpBuf = MyMalloc(HttpBufStructSize);
			if(httpBuf)
			{
				if(My_Pass == ConnectServer(httpBuf->tempBuf))
				{
					//readServerTime(httpBuf);
				
					UpLoadDeviceInfo(httpBuf);
					
					UpLoadTestData(httpBuf);
					
					//readRemoteFirmwareVersion(httpBuf);
				
					DownLoadFirmware(httpBuf);
				}
				MyFree(httpBuf);
			}
				
			giveGSMxMutex();
		}

		vTaskDelay(30000 / portTICK_RATE_MS);
	}
}

static void readServerTime(HttpBuf * httpBuf)
{
	//prepare data
	getSystemDeviceId(httpBuf->tempBuf);
		
	sprintf(httpBuf->sendBuf, "POST %s HTTP/1.1\nHost: %d.%d.%d.%d:%d\nConnection: keep-alive\nContent-Length:[##]\nContent-Type:application/x-www-form-urlencoded;charset=GBK\nAccept-Language: zh-CN,zh;q=0.8\n\ndid=%s", 
		NcdServerReadTimeUrlStr, NCD_ServerIp_1, NCD_ServerIp_2, NCD_ServerIp_3, NCD_ServerIp_4, NCD_ServerPort, httpBuf->tempBuf);
		
	httpBuf->tempP = strstr(httpBuf->sendBuf, "zh;q=0.8\n\n");
	httpBuf->sendDataLen = strlen(httpBuf->tempP)-10;	
	httpBuf->tempP = strstr(httpBuf->sendBuf, "[##]");
	sprintf(httpBuf->tempBuf, "%04d", httpBuf->sendDataLen);
	memcpy(httpBuf->tempP, httpBuf->tempBuf, 4);
	httpBuf->sendDataLen = strlen(httpBuf->sendBuf);
	httpBuf->isPost = true;
	
	//send data
	if(My_Pass == CommunicateWithNcdServerInGPRS(httpBuf))
	{
		RTC_SetTimeData2(httpBuf->tempP+7);
	}
}

static void UpLoadDeviceInfo(HttpBuf * httpBuf)
{
	httpBuf->device = (Device *)httpBuf->tempBuf;
	
	if(My_Pass == ReadDeviceFromFile(httpBuf->device))
	{
		sprintf(httpBuf->sendBuf, "POST %s HTTP/1.1\nHost: %d.%d.%d.%d:%d\nConnection: keep-alive\nContent-Length:[##]\nContent-Type:application/x-www-form-urlencoded;charset=GBK\nAccept-Language: zh-CN,zh;q=0.8\n\ndid=%s&dversion=%d&addr=%s&name=%s&age=%s&sex=%s&phone=%s&job=%s&dsc=%s&type=%s&lang=%s", 
			NcdServerUpDeviceUrlStr, NCD_ServerIp_1, NCD_ServerIp_2, NCD_ServerIp_3, NCD_ServerIp_4, NCD_ServerPort, httpBuf->device->deviceId,  
			GB_SoftVersion, httpBuf->device->addr, httpBuf->device->operator.name, 
			httpBuf->device->operator.age, httpBuf->device->operator.sex,	
			httpBuf->device->operator.phone, httpBuf->device->operator.job, 
			httpBuf->device->operator.department, DeviceTypeString, DeviceLanguageString);
			
		httpBuf->tempP = strstr(httpBuf->sendBuf, "zh;q=0.8\n\n");
		httpBuf->sendDataLen = strlen(httpBuf->tempP)-10;	
		httpBuf->tempP = strstr(httpBuf->sendBuf, "[##]");
		sprintf(httpBuf->tempBuf, "%04d", httpBuf->sendDataLen);
		memcpy(httpBuf->tempP, httpBuf->tempBuf, 4);
		httpBuf->sendDataLen = strlen(httpBuf->sendBuf);
		httpBuf->isPost = true;
			
		//send data
		if(My_Pass == CommunicateWithNcdServerInGPRS(httpBuf))
		{
			httpBuf->tempP2 = strtok(httpBuf->tempP, "#");
			if(httpBuf->tempP2 == NULL)
				return;
			
			//timedate
			httpBuf->tempP2 = strtok(NULL, "#");
			if(httpBuf->tempP2)
			{
				RTC_SetTimeData2(httpBuf->tempP2+5);
			}
			else
				return;
			
			//version
			httpBuf->tempP2 = strtok(NULL, "#");
			if(httpBuf->tempP2)
			{
				httpBuf->remoteSoftInfo = (RemoteSoftInfo *)httpBuf->sendBuf;
				memset(httpBuf->remoteSoftInfo, 0, RemoteSoftInfoStructSize);
				httpBuf->remoteSoftInfo->RemoteFirmwareVersion = strtol(httpBuf->tempP2+8, NULL, 10);
					
				//如果读取到的版本，大于当前版本，且大于当前保存的最新远程版本，则此次读取的是最新的
				if((httpBuf->remoteSoftInfo->RemoteFirmwareVersion > GB_SoftVersion) &&
					(httpBuf->remoteSoftInfo->RemoteFirmwareVersion > getGbRemoteFirmwareVersion()))
				{
					//md5
					httpBuf->tempP2 = strtok(NULL, "#");
					if(httpBuf->tempP2)
					{
						memcpy(httpBuf->remoteSoftInfo->md5, httpBuf->tempP2+4, 32);
						if(My_Pass == WriteRemoteSoftInfo(httpBuf->remoteSoftInfo))
						{
							//md5保存成功后，才更新最新版本号，保存最新固件版本
							setGbRemoteFirmwareVersion(httpBuf->remoteSoftInfo->RemoteFirmwareVersion);
							setGbRemoteFirmwareMd5(httpBuf->remoteSoftInfo->md5);
								
							setIsSuccessDownloadFirmware(false);
						}
					}
					else
						return;
				}	
			}
			else
				return;
		}
	}
}

/***************************************************************************************************
*FunctionName: UpLoadTestData
*Description: 上传测试数据（数据和曲线）
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年2月20日16:43:44
***************************************************************************************************/
static void UpLoadTestData(HttpBuf * httpBuf)
{
	for(httpBuf->i=0; httpBuf->i<MaxUpLoadTestDataNum; httpBuf->i++)
	{
		httpBuf->page.content[httpBuf->i] = MyMalloc(TestDataStructSize);
		if(httpBuf->page.content[httpBuf->i] == NULL)
			goto END;
		httpBuf->testDataPoint = httpBuf->page.content[httpBuf->i];
		httpBuf->testDataPoint->crc = 0;
	}

	//读取纽康度数据
	httpBuf->page.isForNCD = true;
	if(My_Pass != readTestDataFromFileByPageRequest(NULL, &httpBuf->deviceRecordHeader, &httpBuf->page))
		goto END;
		
	httpBuf->deviceRecordHeader.ncdUpLoadIndex = 0;
	for(httpBuf->i=0; httpBuf->i<MaxUpLoadTestDataNum; httpBuf->i++)
	{
		httpBuf->testDataPoint = httpBuf->page.content[httpBuf->i];
		//如果crc校验正确，则开始上传
		if(httpBuf->testDataPoint->crc == CalModbusCRC16Fun(httpBuf->testDataPoint, TestDataStructCrcSize, NULL))
		{
			//上传测试数据
			if(httpBuf->testDataPoint->testResultDesc != ResultIsOK)
				sprintf(httpBuf->tempBuf, "false");
			else
				sprintf(httpBuf->tempBuf, "true");
						
			if(httpBuf->testDataPoint->testDateTime.month == 0 || httpBuf->testDataPoint->testDateTime.day == 0)
			{
				httpBuf->testDataPoint->testDateTime.year = 0;
				httpBuf->testDataPoint->testDateTime.month = 1;
				httpBuf->testDataPoint->testDateTime.day = 1;
				httpBuf->testDataPoint->testDateTime.hour = 0;
				httpBuf->testDataPoint->testDateTime.min = 0;
				httpBuf->testDataPoint->testDateTime.sec = 0;
			}
						
			//read device id
			getSystemDeviceId(httpBuf->tempBuf+10);

			sprintf(httpBuf->sendBuf, "POST %s HTTP/1.1\nHost: %d.%d.%d.%d:%d\nConnection: keep-alive\nContent-Length:[##]\nContent-Type:application/x-www-form-urlencoded;charset=GBK\nAccept-Language: zh-CN,zh;q=0.8\n\ncardnum=%s&qrdata.cid=%s&device.did=%s&tester=%s&sampleid=%s&testtime=20%02d-%d-%d %d:%d:%d&overtime=%d&cline=%d&tline=%d&bline=%d&t_c_v=%.3f&testv=%.*f&serialnum=%s-%s&t_isok=%s&cparm=%d",
				NcdServerUpTestDataUrlStr, NCD_ServerIp_1, NCD_ServerIp_2, NCD_ServerIp_3, NCD_ServerIp_4, NCD_ServerPort, 
				httpBuf->testDataPoint->qrCode.piNum, httpBuf->testDataPoint->qrCode.PiHao, httpBuf->tempBuf+10, httpBuf->testDataPoint->operator.name, 
				httpBuf->testDataPoint->sampleid, httpBuf->testDataPoint->testDateTime.year, httpBuf->testDataPoint->testDateTime.month, 
				httpBuf->testDataPoint->testDateTime.day, httpBuf->testDataPoint->testDateTime.hour, httpBuf->testDataPoint->testDateTime.min, 
				httpBuf->testDataPoint->testDateTime.sec, httpBuf->testDataPoint->time, httpBuf->testDataPoint->testSeries.C_Point.x, 
				httpBuf->testDataPoint->testSeries.T_Point.x, httpBuf->testDataPoint->testSeries.B_Point.x, httpBuf->testDataPoint->testSeries.t_c, 
				httpBuf->testDataPoint->qrCode.itemConstData.pointNum, httpBuf->testDataPoint->testSeries.result, httpBuf->testDataPoint->qrCode.PiHao, 
				httpBuf->testDataPoint->qrCode.piNum, httpBuf->tempBuf, httpBuf->testDataPoint->cParm);

			for(httpBuf->j=0; httpBuf->j<100; httpBuf->j++)
			{
				if(httpBuf->j == 0)
					sprintf(httpBuf->tempBuf, "&series=[%d,%d,%d", httpBuf->testDataPoint->testSeries.TestPoint[httpBuf->j*3],
						httpBuf->testDataPoint->testSeries.TestPoint[httpBuf->j*3+1], httpBuf->testDataPoint->testSeries.TestPoint[httpBuf->j*3+2]);
				else
					sprintf(httpBuf->tempBuf, ",%d,%d,%d", httpBuf->testDataPoint->testSeries.TestPoint[httpBuf->j*3],
						httpBuf->testDataPoint->testSeries.TestPoint[httpBuf->j*3+1], httpBuf->testDataPoint->testSeries.TestPoint[httpBuf->j*3+2]);
				strcat(httpBuf->sendBuf, httpBuf->tempBuf);
			}
							
			strcat(httpBuf->sendBuf, "]");
						
			httpBuf->tempP = strstr(httpBuf->sendBuf, "zh;q=0.8\n\n");
			httpBuf->sendDataLen = strlen(httpBuf->tempP)-10;	
			httpBuf->tempP = strstr(httpBuf->sendBuf, "[##]");
			sprintf(httpBuf->tempBuf, "%04d", httpBuf->sendDataLen);
			memcpy(httpBuf->tempP, httpBuf->tempBuf, 4);
			httpBuf->sendDataLen = strlen(httpBuf->sendBuf);
			httpBuf->isPost = true;
						
			if(My_Pass != CommunicateWithNcdServerInGPRS(httpBuf))
				break;
			else
				httpBuf->deviceRecordHeader.ncdUpLoadIndex++;
		}
	}
	
	plusTestDataHeaderUpLoadIndexToFile(0, httpBuf->deviceRecordHeader.ncdUpLoadIndex);
	
	
	END:	
		for(httpBuf->i=0; httpBuf->i<MaxUpLoadTestDataNum; httpBuf->i++)
			MyFree(httpBuf->page.content[httpBuf->i]);
}

/***************************************************************************************************
*FunctionName: 
*Description: 
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 
***************************************************************************************************/
static void readRemoteFirmwareVersion(HttpBuf * httpBuf)
{
	sprintf(httpBuf->sendBuf, "POST %s HTTP/1.1\nHost: %d.%d.%d.%d:%d\nConnection: keep-alive\nContent-Length:[##]\nContent-Type:application/x-www-form-urlencoded;charset=GBK\nAccept-Language: zh-CN,zh;q=0.8\n\nsoftName=%s&lang=%s", 
		NcdServerQuerySoftUrlStr, NCD_ServerIp_1, NCD_ServerIp_2, NCD_ServerIp_3, NCD_ServerIp_4, NCD_ServerPort, DeviceTypeString, DeviceLanguageString);
	
	httpBuf->tempP = strstr(httpBuf->sendBuf, "zh;q=0.8\n\n");
	httpBuf->sendDataLen = strlen(httpBuf->tempP)-10;	
	httpBuf->tempP = strstr(httpBuf->sendBuf, "[##]");
	sprintf(httpBuf->tempBuf, "%04d", httpBuf->sendDataLen);
	memcpy(httpBuf->tempP, httpBuf->tempBuf, 4);
	httpBuf->sendDataLen = strlen(httpBuf->sendBuf);
	httpBuf->isPost = true;
	
	if(My_Pass == CommunicateWithNcdServerInGPRS(httpBuf))
	{
		//解析最新固件版本
		httpBuf->remoteSoftInfo = (RemoteSoftInfo *)httpBuf->sendBuf;
		memset(httpBuf->remoteSoftInfo, 0, RemoteSoftInfoStructSize);
		httpBuf->remoteSoftInfo->RemoteFirmwareVersion = strtol(httpBuf->tempP+16, NULL, 10);
			
		//如果读取到的版本，大于当前版本，且大于当前保存的最新远程版本，则此次读取的是最新的
		if((httpBuf->remoteSoftInfo->RemoteFirmwareVersion > GB_SoftVersion) &&
			(httpBuf->remoteSoftInfo->RemoteFirmwareVersion > getGbRemoteFirmwareVersion()))
		{
			//解析最新固件MD5
			httpBuf->tempP = strtok(httpBuf->recvBuf, "#");
			if(httpBuf->tempP)
			{
				httpBuf->tempP = strtok(NULL, "#");
					
				memcpy(httpBuf->remoteSoftInfo->md5, httpBuf->tempP+4, 32);
					
				if(My_Pass == WriteRemoteSoftInfo(httpBuf->remoteSoftInfo))
				{
					//md5保存成功后，才更新最新版本号，保存最新固件版本
					setGbRemoteFirmwareVersion(httpBuf->remoteSoftInfo->RemoteFirmwareVersion);
					setGbRemoteFirmwareMd5(httpBuf->remoteSoftInfo->md5);
						
					setIsSuccessDownloadFirmware(false);
				}
			}
		}	
	}
}

static void DownLoadFirmware(HttpBuf * httpBuf)
{
	if((getGbRemoteFirmwareVersion() <= GB_SoftVersion) || (true == getIsSuccessDownloadFirmware()))
		return;
	
	//prepare data
	getSystemDeviceId(httpBuf->tempBuf);
	
	sprintf(httpBuf->sendBuf, "GET %s?softName=%s&lang=%s HTTP/1.1\nHost: %d.%d.%d.%d:%d\nConnection: keep-alive\n\n", 
		NcdServerDownSoftUrlStr, httpBuf->tempBuf, DeviceLanguageString, NCD_ServerIp_1, NCD_ServerIp_2, NCD_ServerIp_3, NCD_ServerIp_4, NCD_ServerPort);

	httpBuf->sendDataLen = strlen(httpBuf->sendBuf);
	httpBuf->isPost = false;
	
	CommunicateWithNcdServerInGPRS(httpBuf);
	if(My_Pass == checkNewFirmwareIsSuccessDownload())
		setIsSuccessDownloadFirmware(true);
}

