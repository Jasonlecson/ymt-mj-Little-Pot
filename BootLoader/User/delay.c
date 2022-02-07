#define __DELAY_C_
#include "include.h"
#include "IAP.h"
#include "BufferManage.h"


u32 SysTickCntMs = 0;//延时函数使用
int value;
/**
* @brief  系统定时器初始化 1Ms中断
* @param   
* @param  None
* @param  None
* @retval None
* @example 
**/
void DelayInit(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	SysTick_Config(SystemCoreClock / 1000);//1Ms
}

/**
* @brief  延时ms
* @param  ms 延时ms
* @param  None
* @param  None
* @retval None
* @example 
**/
void delay_ms(u16 ms)
{
  SysTickCntMs = 0;
	while(SysTickCntMs<ms);
}

/**
* @brief  系统定时器中断
* @param  
* @param  None
* @param  None
* @retval None
* @example 
**/
void SysTick_Handler(void)
{	
	SysTickCntMs ++;
	
	GetVersionInfoCnt++;
	
	if(IAPStructValue.PutDataFlage && IAPStructValue.UpdateFlage)IAPStructValue.DownloadTimeout++;
	else IAPStructValue.DownloadTimeout=0;
	
	IAPStructValue.MainTimeout++;
	
//	if(IAPStructValue.MainTimeout == 150000 && IAPStructValue.DownloadTimeout==0){
//		PAout(11) = 0;
//	}
//	if(PAin(11) == 0 && IAPStructValue.MainTimeout == 150500)
//	{
//		PAout(11) = 1;
//	}
//	
//	if(IAPStructValue.MainTimeout == 160500 && IAPStructValue.DownloadTimeout==0)
//	{
//		BufferManageWrite(&buff_manage_struct_t,"update_me\r",10,&buff_manage_struct_t.Len);
//	}
	
	if(ReadEOTFlag){
		ReadEOTCnt++;
	}
	else{
		ReadEOTCnt=0;
	}
	
	if(buff_manage_struct_net.Cnt>0)
	{
		buff_manage_struct_net.Cnt--;
	}
	
	if(SmartConfigFlage == 1)
	{
		module_net_state_led_cnt++;
		if(module_net_state_led_cnt<200){
			PAout(5) = 1;
		}
		else if(module_net_state_led_cnt<1000){
			PAout(5) = 0;
		}
		else{
			module_net_state_led_cnt = 0;
		}
	}
	else if(IAPStructValue.UpdateFlage == 1)
	{
		module_net_state_led_cnt++;
		if(module_net_state_led_cnt<50){
			PAout(5) = 1;
		}
		else if(module_net_state_led_cnt<150){
			PAout(5) = 0;
		}
		else if(module_net_state_led_cnt<200){
			PAout(5) = 1;
		}
		else if(module_net_state_led_cnt<1000){
			PAout(5) = 0;
		}
		else{
			module_net_state_led_cnt = 0;
		}
	}
	
	if(Usart1ReadCnt != 0)
	{
		Usart1IdelCnt++;
		if(Usart1IdelCnt>10)
		{
			Usart1IdelCnt=0;
			BufferManageWrite(&buff_manage_struct1,Usart1ReadBuff,Usart1ReadCnt,&value);
			Usart1ReadCnt=0;
		}
	}
	
	InitBleTimerCnt++;
	GetBleTimerCnt++;
	GetNetTimerCnt++;
	SmartConfigCnt++;
	RuningUpdataDelay++;
	
	if(KeySmartConfig[1])
		KeySmartConfig[5]++;
	else
		KeySmartConfig[5]=0;

	if(KeySmartConfig[2])
		KeySmartConfig[6]++;
	else
		KeySmartConfig[6]=0;
	
}
