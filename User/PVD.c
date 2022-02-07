/**
  ******************************************************************************
  * @file    PVD.c
  * @author  yang feng wu 
  * @version V1.0.0
  * @date    2020/4/24
  * @brief   PVD断电检测
  ******************************************************************************
	1.找个位置写上以下程序
	NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
  ******************************************************************************
  */

#define PVD_C_

#include "PVD.h"
#include "stm32f10x.h"
#include <string.h>
#include <stdio.h>
#include "System.h"

void PVD_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	PWR_PVDLevelConfig(PWR_PVDLevel_2V9);	//?????:2.2-2.9V
	PWR_PVDCmd(ENABLE);
//   EXTI_DeInit();
//   EXTI_StructInit(&EXTI_InitStructure);
//   EXTI_InitStructure.EXTI_Line = EXTI_Line16;	//PVD???????16
//   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//   EXTI_Init(&EXTI_InitStructure);
}

void  PVD_IRQHandler(void)
{
   if(PWR_GetFlagStatus(PWR_FLAG_PVDO))
   {
      PAout(2) = ~ PAin(2);
   }
}

