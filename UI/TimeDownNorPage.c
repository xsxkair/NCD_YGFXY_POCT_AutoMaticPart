/******************************************************************************************/
/*****************************************ͷ�ļ�*******************************************/

#include	"TimeDownNorPage.h"
#include	"Define.h"
#include	"LCD_Driver.h"

#include	"TestPage.h"
#include	"SystemSetPage.h"
#include	"LunchPage.h"
#include	"PaiDuiPage.h"
#include	"MyMem.h"
#include	"MyTest_Data.h"
#include	"Motor1_Fun.h"
#include	"Motor2_Fun.h"
#include	"Motor4_Fun.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************�ֲ���������*************************************/
static TimeDownPageData *S_TimeDownPageData = NULL;

/******************************************************************************************/
/*****************************************�ֲ���������*************************************/
static void activityStart(void);
static void activityInput(unsigned char *pbuf , unsigned short len);
static void activityFresh(void);
static void activityHide(void);
static void activityResume(void);
static void activityDestroy(void);
static MyRes activityBufferMalloc(void);
static void activityBufferFree(void);

static void RefreshTimeText(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

/***************************************************************************************************
*FunctionName: createSelectUserActivity
*Description: ����ѡ������˽���
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��21��09:00:09
***************************************************************************************************/
MyRes createTimeDownActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "TimeDownActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
		return My_Pass;
	}
	
	return My_Fail;
}

/***************************************************************************************************
*FunctionName: activityStart
*Description: ��ʾ������
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��21��09:00:32
***************************************************************************************************/
static void activityStart(void)
{
	S_TimeDownPageData->currenttestdata = GetCurrentTestItem();
	
	S_TimeDownPageData->S_Timer = &(S_TimeDownPageData->currenttestdata->timeDown_timer);
	
	SelectPage(95);

	S_TimeDownPageData->step = 1;
	motor4MoveTo(Motor4_OpenLocation, 1);
			
			
			
				
			motor2MoveTo(Motor2_StartTestLocation, 10000);
			motor4MoveTo(Motor4_CardLocation, 5000);
}

/***************************************************************************************************
*FunctionName: activityInput
*Description: ��������
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��21��09:00:59
***************************************************************************************************/
static void activityInput(unsigned char *pbuf , unsigned short len)
{

}

/***************************************************************************************************
*FunctionName: activityFresh
*Description: ����ˢ��
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��21��09:01:16
***************************************************************************************************/
static void activityFresh(void)
{
	if(S_TimeDownPageData->count % 5 == 0)
	{
		RefreshTimeText();
		if(TimerOut == timer_expired(S_TimeDownPageData->S_Timer))
		{
			if(S_TimeDownPageData->step == 0)
				startActivity(createTestActivity, NULL, NULL);
		}
	}
	
	S_TimeDownPageData->count++;
	
	if((S_TimeDownPageData->step == 1) && (Motor4_OpenLocation == getMotorxLocation(Motor_4)))
	{
		motor2MoveTo(Motor2_MidLocation, 1);
		S_TimeDownPageData->step = 2;
	}
	
	if((S_TimeDownPageData->step == 2) && (Motor2_MidLocation == getMotorxLocation(Motor_2)))
	{
		S_TimeDownPageData->cardNum = S_TimeDownPageData->currenttestdata->testlocation;
		S_TimeDownPageData->cardNum += 4;
		if(S_TimeDownPageData->cardNum > 8)
			S_TimeDownPageData->cardNum -= 8;
		motor1MoveToNum(S_TimeDownPageData->cardNum, 1);
			
		S_TimeDownPageData->step = 3;
	}

	if((S_TimeDownPageData->step == 3) && (S_TimeDownPageData->cardNum == getMotorxLocation(Motor_1)))
	{
		motor2MoveTo(Motor2_StartTestLocation, 1);
		S_TimeDownPageData->step = 4;
	}
	
	if((S_TimeDownPageData->step == 4) && (Motor2_StartTestLocation == getMotorxLocation(Motor_2)))
	{
		motor4MoveTo(Motor4_CardLocation, 1);
		S_TimeDownPageData->step = 0;
	}
}

/***************************************************************************************************
*FunctionName: activityHide
*Description: ���ؽ���ʱҪ������
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��21��09:01:40
***************************************************************************************************/
static void activityHide(void)
{

}

/***************************************************************************************************
*FunctionName: activityResume
*Description: ����ָ���ʾʱҪ������
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��21��09:01:58
***************************************************************************************************/
static void activityResume(void)
{

}

/***************************************************************************************************
*FunctionName: activityDestroy
*Description: ��������
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��21��09:02:15
***************************************************************************************************/
static void activityDestroy(void)
{
	activityBufferFree();
}

/***************************************************************************************************
*FunctionName: activityBufferMalloc
*Description: ���������ڴ�����
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 
***************************************************************************************************/
static MyRes activityBufferMalloc(void)
{
	if(NULL == S_TimeDownPageData)
	{
		S_TimeDownPageData = MyMalloc(sizeof(TimeDownPageData));
		
		if(S_TimeDownPageData)
		{
			memset(S_TimeDownPageData, 0, sizeof(TimeDownPageData));
	
			return My_Pass;
		}
		else
			return My_Fail;
	}
	else
		return My_Pass;
}

/***************************************************************************************************
*FunctionName: activityBufferFree
*Description: �����ڴ��ͷ�
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��21��09:03:10
***************************************************************************************************/
static void activityBufferFree(void)
{
	MyFree(S_TimeDownPageData);
	S_TimeDownPageData = NULL;
}



static void RefreshTimeText(void)
{
	S_TimeDownPageData->time = timer_surplus(S_TimeDownPageData->S_Timer);
	DspNum(0x1700 , S_TimeDownPageData->time, 2);
}



