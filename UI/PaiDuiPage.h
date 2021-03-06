#ifndef __PAIDUI_P_H__
#define __PAIDUI_P_H__

#include	"Define.h"
#include	"MyTest_Data.h"
#include	"UI_Data.h"
#include	"Motor_Fun.h"

typedef struct PaiDuiPageBuffer_tag {
	unsigned int tempvalue1;					//超时时间
	unsigned int tempvalue2;					//倒计时剩余时间
	char buf[100];
	unsigned short lcdinput[20];
	CreateTestErrorType error;
	PaiduiUnitData * currentTestDataBuffer;							//当前测试数据地址
	PaiduiUnitData * tempPaiduiUnitData;
	unsigned int count;
	unsigned short motor1Location;
	bool isMotorStartted;
	unsigned short index;
	unsigned char lastMotorLocation;
	MotorAction motorAction;
	bool isCreate;
	char tempSampleId[100];
}PaiDuiPageBuffer;


MyRes createPaiDuiActivity(Activity * thizActivity, Intent * pram);


#endif

