/***************************************************************************************************
*FileName:
*Description:
*Author: xsx_kair
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List**********************************************/
/***************************************************************************************************/
#include	"System_Data.h"

#include	"CRC16.h"

#include	<string.h>
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
static SystemData systemData;
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/****************************************File Start*************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName:  getSystemRunTimeData
*Description:  只读的模式读取系统数据
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017年5月26日 15:11:06
***************************************************************************************************/
const SystemData * getSystemRunTimeData(void)
{
	return &systemData;
}

void setSystemTime(DateTime * dateTime)
{
	if(dateTime)
		memcpy(&systemData.systemDateTime, dateTime, DateTimeStructSize);
}

DateTime * getSystemTime(DateTime * dateTime)
{
	if(dateTime)
		memcpy(dateTime, &systemData.systemDateTime, DateTimeStructSize);
	
	return &systemData.systemDateTime;
}

void setSystemWireIP(unsigned int ip)
{
	systemData.wireNetInfo.ip.ip_4 = (ip>>24)&0xff;
	systemData.wireNetInfo.ip.ip_3 = (ip>>16)&0xff;
	systemData.wireNetInfo.ip.ip_2 = (ip>>8)&0xff;
	systemData.wireNetInfo.ip.ip_1 = ip&0xff;
}

void setSystemWireMac(unsigned char * mac)
{
	if(mac == NULL)
		return;
	
	systemData.wireNetInfo.LineMAC[0] = mac[0];
	systemData.wireNetInfo.LineMAC[1] = mac[1];
	systemData.wireNetInfo.LineMAC[2] = mac[2];
	systemData.wireNetInfo.LineMAC[3] = mac[3];
	systemData.wireNetInfo.LineMAC[4] = mac[4];
	systemData.wireNetInfo.LineMAC[5] = mac[5];
}

void setSystemWireLinkStatus(LinkStatus linkStatus)
{
	systemData.wireNetInfo.lineStatus = linkStatus;
}

void setSystemDeviceIsNew(bool status)
{
	systemData.deviceIsNew = status;
}

bool systemDeviceIsNew(void)
{
	return systemData.deviceIsNew;
}

void setSystemDeviceId(char * device)
{
	if(device)
		memcpy(systemData.deviceId, device, DeviceIdLen);
}
void getSystemDeviceId(char * device)
{
	if(device)
		memcpy(device, systemData.deviceId, DeviceIdLen);
}
void setSystemDeviceAddr(char * device)
{
	if(device)
		memcpy(systemData.deviceAdd, device, DeviceAddrLen);
}
void getSystemDeviceAddr(char * device)
{
	if(device)
		memcpy(device, systemData.deviceAdd, DeviceAddrLen);
}
void setTestLedValue(unsigned short value)
{
	systemData.testLedValue = value;
}

void setBaseLineValue(unsigned short value)
{
	systemData.baseLineValue = value;
}

void updateSystemWorkStatus(unsigned short workStatus, MyBitAction action)
{
	if(action == ON)
		systemData.systemWorkStatus |= workStatus;
	else
		systemData.systemWorkStatus &= ~workStatus;
}

unsigned short readSystemWorkStatus(void)
{
	return systemData.systemWorkStatus;
}

/****************************************end of file************************************************/
