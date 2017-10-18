/***************************************************************************************************
*FileName：SystemUI_Task
*Description：系统界面更新任务
*Author：xsx
*Data：2016年4月30日11:21:29
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/

#include	"SystemUI_Task.h"
#include	"LCD_Driver.h"

#include	"DateTime.h"
#include	"UI_Data.h"
#include	"System_Data.h"

#include 	"FreeRTOS.h"
#include 	"task.h"

#include	<string.h>
#include	"stdio.h"
/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/
#define SystemUITask_PRIORITY			3
const char * SystemUITaskName = "vSystemUITask";

static DateTime S_dateTime;
static char tempBuf[30];
/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/
static void vSystemUITask( void *pvParameters );
static void displaySystemDateTime(void);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName：StartvSystemUITask
*Description：建立系统指示灯任务
*Input：None
*Output：None
*Author：xsx
*Data：2015年8月26日16:58:13
***************************************************************************************************/
char StartvSystemUITask(void)
{
	return xTaskCreate( vSystemUITask, SystemUITaskName, configMINIMAL_STACK_SIZE*2, NULL, SystemUITask_PRIORITY, NULL );
}

/***************************************************************************************************
*FunctionName：vSysLedTask
*Description：系统指示灯闪烁表面程序正常运行
*Input：None
*Output：None
*Author：xsx
*Data：2015年8月26日16:58:46
***************************************************************************************************/
static void vSystemUITask( void *pvParameters )
{
	unsigned short cnt = 0;
	
	while(1)
	{
		activityFreshFunction();
		
		if(cnt % 3 == 0)
			displaySystemDateTime();
		
		cnt++;

		vTaskDelay(100 / portTICK_RATE_MS);
	}
}

static void displaySystemDateTime(void)
{
	getSystemTime(&S_dateTime);
	
	snprintf(tempBuf, 30, "       20%02d-%02d-%02d %02d:%02d:%02d", S_dateTime.year, S_dateTime.month, S_dateTime.day,
		S_dateTime.hour, S_dateTime.min, S_dateTime.sec);
		
	DisText(0x1000, tempBuf, 28);
}
