/***************************************************************************************************
*FileName: Motor3_Fun
*Description: 控制爪子整体移动
*Author: xsx_kair
*Data: 2017年7月20日 15:29:33
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"Motor3_Fun.h"

#include	"Motor3_Driver.h"

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
/***************************************************************************************************
*FunctionName:  motor3StartMove
*Description:   电机3开始移动
*Input:  	isFront -- 方向
*			stepNum -- 移动步数
*Output:  
*Return:  
*Author:  xsx
*Date:  2017年7月20日 15:36:32
***************************************************************************************************/
void motor3StartMove(bool isFront, unsigned short stepNum)
{
	if(isFront)
		setMotor3DirGPIO(ON);
	else
		setMotor3DirGPIO(OFF);
	delay_ms(5);
	setMotorxDir(Motor_3, isFront);
	
	setMotorxMoveStepNum(Motor_3, stepNum);
	setMotorxPeriodCnt(Motor_3, 0);
	
	while(1)
	{
		if(getMotorxMoveStepNum(Motor_3) == 0)
			break;
		
		vTaskDelay(1 / portTICK_RATE_MS);
	}
}

/***************************************************************************************************
*FunctionName:  resetToOriginLocation
*Description:   回原点
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date:  2017年7月20日 15:59:48
***************************************************************************************************/
void resetToOriginLocation(bool isWait)
{
	setMotorxLocation(Motor_3, getMotorxMaxLocation(Motor_3));
	
	motor3StartMove(false, getMotorxMaxLocation(Motor_3));
	
	while(isWait)
	{
		if(getMotorxMoveStepNum(Motor_3) == 0)
			break;
		
		vTaskDelay(1 / portTICK_RATE_MS);
	}

}
/****************************************end of file************************************************/

