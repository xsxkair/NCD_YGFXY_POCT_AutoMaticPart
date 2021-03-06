#ifndef __TEST_P_H__
#define __TEST_P_H__

#include	"MyTest_Data.h"
#include	"SystemSet_Data.h"
#include	"Test_Fun.h"
#include	"UI_Data.h"
#include	"Motor_Fun.h"

typedef struct TestPageBuffer_tag {
	PaiduiUnitData * currenttestdata;
	Basic_ICO myico[3];
	LineInfo line;
	unsigned short lcdinput[20];
	unsigned char testisover;
	char buf[100];
	TestData testDataForPrintf;							//复制一份测试数据，给打印用
	SystemSetData systemSetData;
	bool canExit;
	MotorAction motorAction;
}TestPageBuffer;


MyRes createTestActivity(Activity * thizActivity, Intent * pram);

#endif

