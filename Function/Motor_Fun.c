/***************************************************************************************************
*FileName:
*Description:
*Author: xsx_kair
*Data:
***************************************************************************************************/

/**************************************************************************************************/
/******************************************Header List*********************************************/
/**************************************************************************************************/
#include	"Motor_Fun.h"

#include	"Motor1_Fun.h"
#include	"Motor2_Fun.h"
#include	"Motor4_Fun.h"

#include	"Motor_Data.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
/**************************************************************************************************/
/******************************************Static Variables****************************************/
/**************************************************************************************************/
static xSemaphoreHandle xMotorMutex;
static MotorAction S_MotorAction;																//电机指令保存
/**************************************************************************************************/
/******************************************Static Methods******************************************/
/**************************************************************************************************/
static void MotorMoveToWaitCardPutIn(unsigned char num);
static void MotorMoveToStartTestLocation(unsigned char num);
static void PutCardOutOfDevice(unsigned char num);
static void PutCardOutOfDeviceAfterTest(void);
static void MotorMoveToOriginLocation(unsigned char num, bool reset);
static void motorMoveToPutDownCardInPlace(void);
static void motorMoveToPutDownCardInTestPlace(void);
/**************************************************************************************************/
/**************************************************************************************************/
/**************************************************************************************************/
/****************************************File Start************************************************/
/**************************************************************************************************/
/**************************************************************************************************/
void MotorActionInit(void)
{
	vSemaphoreCreateBinary(xMotorMutex);
	while(pdPASS == xSemaphoreTake(xMotorMutex, 0))
		;
}

void MotorActionFunction(void)
{
	if(S_MotorAction.motorActionEnum != MotorActionNone)
	{
		switch(S_MotorAction.motorActionEnum)
		{
			case Motor1MoveDef :			motor1MoveToNum(S_MotorAction.motorParm, true);	break;
				
			case Motor2MoveDef :			motor2MoveTo(1, 2, S_MotorAction.motorParm, true);	break;
				
			case Motor4MoveDef :			motor4MoveTo(S_MotorAction.motorParm, true);	break;
			
			case WaitCardPutInDef :			MotorMoveToWaitCardPutIn(S_MotorAction.motorParm);	break;
				
			case StartTestDef :				MotorMoveToStartTestLocation(S_MotorAction.motorParm);	break;
				
			case PutCardOutOfDeviceDef :	PutCardOutOfDevice(S_MotorAction.motorParm);	break;
			
			case PutCardOutOfDeviceAfterTestDef :	PutCardOutOfDeviceAfterTest();	break;
			
			case OriginLocationDef :		MotorMoveToOriginLocation(S_MotorAction.motorParm, true);	break;
											
			case PutDownCardInPlaceDef :	motorMoveToPutDownCardInPlace();	break;
			
			case PutDownCardInTestPlaceDef :	motorMoveToPutDownCardInTestPlace();	break;
				
			default: 						break;
		}
		
		S_MotorAction.motorActionEnum = MotorActionNone;
		
		xSemaphoreGive(xMotorMutex);
	}
	
	vTaskDelay(100 / portTICK_RATE_MS);
}

MyRes StartMotorAction(MotorAction * motorAction, bool waitActionDone)
{
	if(motorAction == NULL)
		return My_Fail;
	
	if(S_MotorAction.motorActionEnum != MotorActionNone)
		return My_Fail;
	
	while(pdPASS == xSemaphoreTake(xMotorMutex, 0))
		;
	
	S_MotorAction.motorActionEnum = motorAction->motorActionEnum;
	S_MotorAction.motorParm = motorAction->motorParm;
		
	if(waitActionDone)
	{
		xSemaphoreTake(xMotorMutex, portMAX_DELAY);
	}
	
	return My_Pass;
}

MyRes FormatParmAndStartMotorAction(MotorAction * motorAction, MotorActionEnum motorActionEnum, unsigned int motorParm, bool waitActionDone)
{
	if(motorAction)
	{
		motorAction->motorActionEnum = motorActionEnum;
		motorAction->motorParm = motorParm;
		
		return StartMotorAction(motorAction, waitActionDone);
	}
	else
		return My_Fail;
}

bool isMotorMoveEnd(portTickType waitBlockTime)
{
	if(pdPASS == xSemaphoreTake(xMotorMutex, waitBlockTime))
		return true;
	else
		return false;
}


static void MotorMoveToWaitCardPutIn(unsigned char num)
{
	MotorMoveToOriginLocation(num, true);
	
	motor2MoveTo(1, 2, Motor2_WaitCardLocation, true);
}


static void MotorMoveToStartTestLocation(unsigned char num)
{
	MotorMoveToOriginLocation(num, true);
	
	motor2MoveTo(1, 2, Motor2_CatchCardLocation, true);
	
	motor4MoveTo(Motor4_CardLocation, true);
	
	motor2MoveTo(1, 2, Motor2_StartTestLocation, true);
}


static void PutCardOutOfDevice(unsigned char num)
{
	MotorMoveToOriginLocation(num, true);
	
	motor4MoveTo(Motor4_OpenLocation, true);
	
	motor2MoveTo(1, 2, Motor2_StartTestLocation, true);

	motor4MoveTo(Motor4_CardLocation, true);
	
	motor2MoveTo(1, 2, Motor2_PutCardOutLocation, true);

	MotorMoveToOriginLocation(0, false);
}

/*
*测试完后，夹爪处于夹住卡的状态，直接推出即可
*/
static void PutCardOutOfDeviceAfterTest(void)
{
	motor2MoveTo(1, 2, Motor2_PutCardOutLocation, true);

	MotorMoveToOriginLocation(0, false);
}


static void MotorMoveToOriginLocation(unsigned char num, bool reset)
{
	motor4MoveTo(Motor4_OpenLocation, true);
	
	motor2MoveTo(1, 2, Motor2_MidLocation, true);

    //如果为0，则只是将爪子移动到转盘中心，转盘不移动
    if(num != 0)
        motor1MoveToNum(num, true);
    
    if(reset)
    {
        //移动到转盘中心后，爪子需要复位
        #if(Motor4Type == Motor4UsartMotor)
            motor4Reset();
        #endif
    }
    
    
    motor4MoveTo(Motor4_OpenLocation, true);
}


static void motorMoveToPutDownCardInPlace(void)
{
	motor4MoveTo(Motor4_OpenLocation, true);

	motor2MoveTo(1, 2, Motor2_WaitCardLocation, true);
	
	motor4MoveTo(Motor4_CardLocation, true);
	
	motor2MoveTo(1, 2, Motor2_PutDownCardLocation, true);
	
	MotorMoveToOriginLocation(0, false);
}

static void motorMoveToPutDownCardInTestPlace(void)
{
	motor4MoveTo(Motor4_OpenLocation, true);
	
	motor2MoveTo(1, 2, Motor2_StartTestLocation, true);

	motor4MoveTo(Motor4_CardLocation, true);
	
	motor2MoveTo(1, 2, Motor2_PutDownCardLocation2, true);
	
	MotorMoveToOriginLocation(0, false);
}

/****************************************end of file***********************************************/
