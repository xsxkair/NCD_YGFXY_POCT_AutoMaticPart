#ifndef __SUSER_P_H__
#define __SUSER_P_H__

#include	"Define.h"
#include	"MyTest_Data.h"
#include	"Timer.h"
#include	"UI_Data.h"
#include	"Operator.h"
#include	"Device.h"
#include	"Motor_Fun.h"

typedef struct UserPage_tag {
	Device device;													//设备信息
	Operator * tempOperator;										//临时指针
	Operator * targetOperator;										//选择操作人后目标缓存
	unsigned short lcdinput[100];									//lcd输入解析
	char tempBuf[20];
	unsigned short tempV1;
	unsigned char pageindex;										//页面索引
	PaiduiUnitData * currenttestdata;
	MotorAction motorAction;
}UserPageBuffer;

MyRes createSelectUserActivity(Activity * thizActivity, Intent * pram);

#endif

