#include "include.h"

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	
	/*串口--1*/
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	//定时器2
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStructure);
}
/*复位芯片*/
void Reset_MCU(void)
{
  __disable_fault_irq();   
	NVIC_SystemReset();
}


/*获取系统复位重启的类型*/
/*1:断电上电复位*/
/*0:软复位*/
char GetSystemResetType(void)
{
	if(RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
	{
		return 1;
	}
	else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
	{
		return 0;
	}
	else if (RCC_GetFlagStatus(RCC_FLAG_SFTRST)!= RESET)
	{
		return 0;
	}
	RCC_ClearFlag();
}
