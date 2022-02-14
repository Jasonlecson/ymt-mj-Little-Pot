
/**
  ******************************************************************************
  * @file    timer.c
  * @author  yang feng wu 
  * @version V1.0.0
  * @date    2019/10/13
  * @brief   ��ʱ��
  ******************************************************************************
	1.�Ҹ�λ��д�����³���
	//��ʱ��4
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStructure);
  ******************************************************************************
  */


#define _TIME_C_
#include "include.h"
#include "timer.h"
#include "parse.h"
#include "BufferManage.h"
extern buff_manage_struct buff_manage_struct_t;

int uart_send_delay=0;
u32 key_down_time_ms=0;

int buzzer=0;
int buzzer1=0;
u32 system_start_timer = 0;//ϵͳ����ʱ��
int module_net_state_led_cnt=0;
char RunSmartConfigFlag=0;
char RunNetStateFlag=0;

int timestampcnt = 0;

u32 timer_test=0;

u32 fertility_control_cnt=0;

//void Timer2_Config(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
//	TIM_OCInitTypeDef TIM_OCInitStructure;
//	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				 
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //���
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
//	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��
//	
//	TIM_TimeBaseInitStructure.TIM_Period = 1000-1;
//	TIM_TimeBaseInitStructure.TIM_Prescaler = 72-1;
//	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;
//	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
//	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
////	TIM_ARRPreloadConfig(TIM2, DISABLE);
////	TIM_ClearITPendingBit(TIM2,  TIM_IT_Update);
////	TIM_ITConfig(TIM2,  TIM_IT_Update, ENABLE);
//	
//	
//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
//	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//	TIM_OCInitStructure.TIM_Pulse = 0;
//	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
//	TIM_OC4Init(TIM2,&TIM_OCInitStructure);
//	
//	TIM_OC4PreloadConfig(TIM2,TIM_OCPreload_Enable);
//	TIM_ARRPreloadConfig(TIM2, ENABLE);
//	
//	TIM_Cmd(TIM2,ENABLE);
//}





/**
* @brief  ��ʱ��4����
* @param  None
* @param  None
* @param  None
* @retval None
* @example 
**/

void Timer4_Config(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	/* Resets the TIM4 */
  TIM_DeInit(TIM4);
	//������ʱ�ӷָ
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	// ѡ���˼�����ģʽ��
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//��ֵ
	TIM_TimeBaseStructure.TIM_Period = 10;//��ʱʱ��1ms��һ��
	//������������Ϊ TIMx ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ��72M / 7199+1 = 0.01M
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;
	
	//TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	/* Enables the TIM4 counter */
  TIM_Cmd(TIM4, ENABLE);
	
	/* Enables the TIM4 Capture Compare channel 1 Interrupt source */
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE );
}

/**
* @brief  ��ʱ��4�жϺ���
* @param  None
* @param  None
* @param  None
* @retval None
* @example 
**/
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		
		illumination_statistics_test = illumination_statistics_test+1;
		illumination_statistics_timer_delay=illumination_statistics_timer_delay+1;//�ڶ�ʱ�������ۼ�,����ͳ��ʱ����
		
		
		Usart1IdleLoop(10);
		
		if(EventReportTimeCnt1>0)
		{
			EventReportTimeCnt1--;
		}
		
		if(EventReportTimeCnt2>0)
		{
			EventReportTimeCnt2--;
		}
		
		fertility_control_cnt++;
		
		timer_test++;
		
		InitModelCnt++;
		SendModelCnt++;
		InitBleCnt++;
		SendBleCnt++;
		water_stageCnt++;
		sensor_ligntCnt++;
		DS18B20Cnt++;
		if(uart_send_delay>0)
		{
			uart_send_delay--;
		}
//		ConfigModuleBlockDelay++;
		WateringCirculation++;
		
		send_get_net_cnt++;
		
		send_get_down_cnt++;//���Ʒ���get_down
		
		if(system_start_timer < 60000){
			system_start_timer++;
		}
		
		if(timestamp > 1592113818){
			timestampcnt++;
			if(timestampcnt>=1000){
				timestampcnt = 0;
				timestamp++;
			}
		}
		
		if(water_stage==0){
			water_stage_empty_cnt1++;
			if(water_stage_empty_cnt1>5000){
				water_stage_empty_cnt1=0;
				water_stage_empty_flag = 1;//���Ʒ������ε�
				water_stage_water_flag = 0;
			}
		}
		else{
			water_stage_empty_cnt1=0;
		}
		
		
		if(water_stage!=0){
			water_stage_water_cnt1++;
			if(water_stage_water_cnt1>5000){
				water_stage_water_cnt1=0;
				water_stage_water_flag = 1;
				water_stage_empty_flag = 0;
			}
		}
		else{
			water_stage_water_cnt1=0;
		}
		
		
		
		if(water_stage_water_flagCopy != water_stage_empty_flag){
			water_stage_water_flagCopy = water_stage_empty_flag;
			if(water_stage_empty_flag){//��ˮ
				water_stage_empty_run_flage = 1;
			}
		}
		
		
//		if(water_stage_empty_run_flage==1){
//			buzzer++;
//			if(buzzer>0 && buzzer<500){
//				PAout(4) = 1;
//			}
//			if(buzzer>500 && buzzer<1000){
//				PAout(4) = 0;
//			}
//			if(buzzer>1000 && buzzer<1500){
//				PAout(4) = 1;
//			}
//			if(buzzer>1500){
//				PAout(4) = 0;
//				water_stage_empty_run_flage = 0;
//			}
//		}else{
//			buzzer=0;
//		}
		

		if(water_stage>98){
			water_stage_full_cnt1++;
			if(water_stage_full_cnt1>5000){
				water_stage_full_cnt1=0;
				water_stage_full_flag = 1;//���Ʒ�������.....
				water_stage_nofull_flag = 0;
			}
		}
		else{
			water_stage_full_cnt1=0;
		}
		
		
		if(water_stage<98){
			water_stage_nofull_cnt1++;
			if(water_stage_nofull_cnt1>5000){
				water_stage_nofull_cnt1=0;
				water_stage_nofull_flag=1;
				water_stage_full_flag=0;
			}
		}
		else{
			water_stage_nofull_cnt1=0;
		}
		
		
		if(water_stage_full_flagCopy != water_stage_full_flag){
			water_stage_full_flagCopy = water_stage_full_flag;
			if(water_stage_full_flag){
				water_stage_full_run_flage = 1;
			}
		}
		if(water_stage_full_run_flage==1){
			buzzer1++;
//			printf("11111111\r\n");
			if(buzzer1<3000){
//				if(system_start_timer>10000){//ϵͳ����10S����
//					PAout(4) = 1;
//				}
			}	
			else{
				water_stage_full_run_flage=0;
				PAout(4) = 0;
			}
		}
		else{
			buzzer1=0;
		}
		
		if(EventReportTimeCnt>0){
			EventReportTimeCnt--;
		}
		
		if(key_down_count!=0)
		{
			if(SmartConfigFlage!=1)
			{
				key_down_time_ms++;
				
				if(key_down_time_ms>1000)
				{
					key_down_time_ms=0;
					if(key_down_count==1)
					{
						key_down_count_copy=key_down_count;
					}
					else if(key_down_count==2)
					{
						key_down_count_copy=key_down_count;
					}
					else
					{
						
					}
					key_down_count=0;
				}
			}
			else
			{
				key_down_count=0;
			}
		}
		
		
		get_time_stamp_cnt++;
		testing_time_appear_timer_cnt++;
		
		SmartConfigKey();
		
		if(KeySmartConfig[1])
		{
			KeySmartConfig[5]++;
		}
		else
			KeySmartConfig[5]=0;

		if(KeySmartConfig[2])
		{
			KeySmartConfig[6]++;
		}
		else
			KeySmartConfig[6]=0;
		
		if(SmartConfigFlage==1){
			SmartConfigCnt++;
		}else{
			SmartConfigCnt=0;
		}
		
		
		//module_net_state == net_status_uap || 
		if(SmartConfigFlage == 1)
		{ 
			RunNetStateFlag = 1;
			module_net_state_led_cnt++;
			if(module_net_state_led_cnt<200){
				PAout(5) = 1;
				PAout(6) = 1;
				PAout(2) = 1;
			}
			else if(module_net_state_led_cnt<1000){
				PAout(5) = 0;
				PAout(6) = 0;
				PAout(2) = 0;
			}
			else{
				module_net_state_led_cnt = 0;
			}
		}
		else if(module_net_state == net_status_local)
		{
			RunNetStateFlag = 1;
			SmartConfigFlage = 0;
				PAout(5) = 1;
				PAout(6) = 1;
				PAout(2) = 1;
		}
		else if(module_net_state == net_status_unprov)
		{
			RunNetStateFlag = 1;
				PAout(5) = 0;
				PAout(6) = 0;
				PAout(2) = 0;
		}
		else if(module_net_state == net_status_offline)
		{
			RunNetStateFlag = 1;
			module_net_state_led_cnt++;
			if(module_net_state_led_cnt<100){
				PAout(5) = 1;
				PAout(6) = 1;
				PAout(2) = 1;
			}
			else if(module_net_state_led_cnt<300){
				PAout(5) = 0;
				PAout(6) = 0;
				PAout(2) = 0;
			}
			else{
				module_net_state_led_cnt = 0;
			}
		}
		else if(module_net_state == net_status_cloud)
		{
			if(RunNetStateFlag == 1)
			{
				RunNetStateFlag = 0;
				if(LEDState == -1)//û�п��ƹ�LED
				{
				PAout(5) = 0;
				PAout(6) = 0;
				PAout(2) = 0;
				}
				else if(LEDState==1)//���ƹ�LED
				{
				PAout(5) = 0;
				PAout(6) = 0;
				PAout(2) = 0;
				}
				else if(LEDState==0)//���ƹ�LED
				{
				PAout(5) = 1;
				PAout(6) = 1;
				PAout(2) = 1;
				}
			}
		}
		
		if(WateringFlag){
			WateringDelayTimer++;
		}else{
			WateringDelayTimer=0;
		}
	}
}





