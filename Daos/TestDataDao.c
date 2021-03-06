/***************************************************************************************************
*FileName: TestDataDao
*Description: 测试数据dao
*Author: xsx_kair
*Data: 2016年12月8日10:43:26
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"TestDataDao.h"
#include	"RecordDataDao.h"
#include	"StringDefine.h"

#include	"CRC16.h"

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

MyRes writeTestDataToFile(TestData * testData)
{
	if(testData)
	{
		testData->crc = CalModbusCRC16Fun(testData, TestDataStructCrcSize, NULL);
		
		return writeRecordDataToFile(TestDataFileName, testData, TestDataStructSize);
	}
	
	return My_Fail;
}

MyRes readTestDataFromFileByPageRequest(PageRequest * pageRequest, DeviceRecordHeader * deviceRecordHeader, Page * page)
{
	return readRecordDataFromFileByPageRequest(TestDataFileName, pageRequest, deviceRecordHeader, page, TestDataStructSize);
}

MyRes plusTestDataHeaderUpLoadIndexToFile(unsigned char userIndexPlusNum, unsigned char ncdIndexPlusNum)
{
	return plusRecordDataHeaderUpLoadIndexToFile(TestDataFileName, userIndexPlusNum, ncdIndexPlusNum);
}

MyRes readTestDataFromFileByUpLoadIndex(bool isNcd, DeviceRecordHeader * deviceRecordHeader, Page * page)
{
	return readRecordDataFromFileByUpLoadIndex(TestDataFileName, isNcd, deviceRecordHeader, page, TestDataStructSize);
}

bool isTestDataToBeUpLoad(bool isNcd, DeviceRecordHeader * deviceRecordHeader)
{
	return isRecordDataToBeUpLoad(TestDataFileName, isNcd, deviceRecordHeader);
}

MyRes deleteTestDataFile(void)
{
	return deleteRecordDataFile(TestDataFileName);
}

/****************************************end of file************************************************/
