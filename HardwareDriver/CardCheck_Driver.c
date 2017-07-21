/***************************************************************************************************
*FileName��CardLimit_Driver
*Description����⿨��λ����
*Author��xsx
*Data��2016��5��3��10:48:53
***************************************************************************************************/

/***************************************************************************************************/
/******************************************ͷ�ļ�***************************************************/
/***************************************************************************************************/

#include	"CardCheck_Driver.h"

/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************����********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName��DRV_Init
*Description�����IO��ʼ��
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��23��11:30:52
***************************************************************************************************/
void CardCheck_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(CardIn_RCC, ENABLE);

  	GPIO_InitStructure.GPIO_Pin = CardIn_Pin; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(CardIn_Group, &GPIO_InitStructure);
}


