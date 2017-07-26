/***************************************************************************************************
*FileName:
*Description:
*Author: xsx_kair
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"Motor4_Fun.h"

#include	"Motor4_Driver.h"
#include	"Timer3_Driver.h"

#include	"Delay.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"
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

void motor4ResetToOriginLocation(void)
{
	setMotor4DirGPIO(OFF);
	delay_ms(5);
	setMotorxDir(Motor_4, false);
	
	setMotorxLocation(Motor_4, getMotorxMaxLocation(Motor_4));
	setMotorxMoveStepNum(Motor_4, 20000);
	setMotorxPeriodCnt(Motor_4, 0);
	
	while(getMotorxMoveStepNum(Motor_4) > 0)
		vTaskDelay(1 / portTICK_RATE_MS);
}

void motor4CatchCard(void)
{
	motor4ResetToOriginLocation();
	
	setMotor4DirGPIO(ON);
	delay_ms(5);
	setMotorxDir(Motor_4, true);
	setMotorxMoveStepNum(Motor_4, 430);
	setMotorxPeriodCnt(Motor_4, 0);
}

void motor4Open(void)
{
	setMotor4DirGPIO(OFF);
	delay_ms(5);
	setMotorxDir(Motor_4, false);

	setMotorxMoveStepNum(Motor_4, 500);
	setMotorxPeriodCnt(Motor_4, 0);
}
void motor4MoveTo(unsigned short value)
{
	unsigned short location = getMotorxLocation(Motor_4);
	if(location < value)
	{
		setMotor4DirGPIO(ON);
		setMotorxDir(Motor_4, true);
		
		location = value - location;
	}
	else
	{
		setMotor4DirGPIO(OFF);
		setMotorxDir(Motor_4, false);
		
		location = location - value;
	}
	delay_ms(5);
	
	setMotorxMoveStepNum(Motor_4, location);
	setMotorxPeriodCnt(Motor_4, 0);
}

void testMotor4(void)
{
	motor4Open();
	while(0)
	{
		motor4Close();
		vTaskDelay(2000 / portTICK_RATE_MS);
		
		motor4Open();
		vTaskDelay(2000 / portTICK_RATE_MS);

	}
}
/****************************************end of file************************************************/

