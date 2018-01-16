/******************************************File Start**********************************************
*FileName: 
*Description: 
*Author:xsx
*Data: 
****************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List**********************************************/
/***************************************************************************************************/
#include	"Motor_Data.h"
/***************************************************************************************************/
/******************************************Static Variables*****************************************/
/***************************************************************************************************/
//定义4个电机
#if(Motor4Type == Motor4UsartMotor)
static Motor GB_Motors[2] = 
#elif(Motor4Type == Motor4IOMotor)
static Motor GB_Motors[3] = 
#endif
{
	//电机1，转盘
	{
		.highTime = 1,										//高电平时间段，(1-3)*100us
		.lowTime = 2,										//低电平时间段，(4)*100us
		.periodCnt = 0,
		.isFront = true,									//是否前进
		.moveStepNum = 0,									//运动步数
		.motorLocation = 0,									//代表当前电机对准口的编号，0 -- 无效数据， 1-8表示插卡空对应编号
		.motorTargetLocation = 0,							//代表电机目标对准口的编号，0 -- 无效数据， 1-8表示插卡空对应编号
		.motorMaxLocation = 60000,
		.parm1 = 0,											//两次中断的间隔步数
		.parm2 = true,										//由于停止位置在边沿，需要继续走几步到中间，true，继续走，false停止
		.parm3 = 0											//到达位置后，为了补偿居中，继续走的步数
	},
	//电机2，爪子移动
	{
		.highTime = 2,										//高电平时间，*100us
		.lowTime = 3,										//低电平时间，*100us
		.periodCnt = 0,
		.isFront = true,									//是否前进
		.moveStepNum = 0,									//运动步数
		.motorLocation = 1,									//电机当前位置
		.motorTargetLocation = 0,							//电机目标位置
		.motorMaxLocation = 65000,
	},
#if(Motor4Type == Motor4IOMotor)	
	//电机4，爪子捏合
	{
		.highTime = 2,										//高电平时间，*100us
		.lowTime = 4,										//低电平时间，*100us
		.periodCnt = 0,
		.isFront = true,									//是否前进
		.moveStepNum = 0,									//运动步数
		.motorLocation = 60000,								//电机当前位置
		.motorTargetLocation = 0,							//电机目标位置
		.motorMaxLocation = 65000
	}
#endif
};
/***************************************************************************************************/
/******************************************Static Methods*******************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/******************************************Main Body************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
Motor * getMotor(Motorx_Def motor)
{
	return &GB_Motors[motor];
}



/****************************************end of file************************************************/
