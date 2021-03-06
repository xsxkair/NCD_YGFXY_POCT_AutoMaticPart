/****************************************file start****************************************************/
#ifndef SYSTEMSET_D_H
#define	SYSTEMSET_D_H

#include	"Define.h"
#include	"Device.h"
#include	"NetWork.h"

#pragma pack(1)
typedef struct SystemSetData_Tag
{		
	WireNetSet wireNetSet;												//有线网设置
	ServerSet serverSet;												//服务器设置
	bool isAutoPrint;													//是否自动打印
	unsigned short ledSleepTime;										//休眠时间 s
	unsigned char ledLightIntensity;									//屏幕亮度 0-100
	unsigned short testLedLightIntensity;								//测试时采用的led亮度值，mv
    PrintInfo printInfo;                                                //打印数据信息设置
	unsigned char parm[480];											//预留512字节的参数位置
	unsigned short crc;
}SystemSetData;
#pragma pack()

#define	SystemSetDataStructSize		sizeof(SystemSetData)
#define SystemSetDataStructCrcSize	SystemSetDataStructSize - 2

void setDefaultSystemSetData(SystemSetData * systemSetData);
SystemSetData * getGBSystemSetData(void);
void readGbSystemSetData(SystemSetData * systemSetData);
void upDateSystemSetData(SystemSetData * systemSetData);
unsigned short getSystemTestLedLightIntensity(void);
void setIsShowRealValue(bool isShow);
bool IsShowRealValue(void);
void readSystemServerSerData(ServerSet * server);
void readPrintInfo(PrintInfo * printInfo);
#endif

/****************************************end of file************************************************/
