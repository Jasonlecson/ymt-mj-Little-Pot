#include "include.h"
#include "ADC.h"

/**
* @brief  ����ADC����
* @param  
* @param  None
* @param  None
* @retval None
* @example 
**/
void ADC_Config(void)
{
  ADC_InitTypeDef ADC_InitStructure; 
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_ADC1	, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);      /*ʹ��SWD ����JTAG*/

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M
  

	ADC_DeInit(ADC1);  //��λADC1 

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 5;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

  
	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
	
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
	
	ADC_StartCalibration(ADC1);	 //����ADУ׼
 
	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
}

/**
* @brief  DMAģʽ������ADC����
* @param  
* @param  None
* @param  None
* @retval None
* @example 
**/
void DMA_ADC_Config(void)
{
//	ADC_InitTypeDef ADC_InitStructure;
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_ADC1	, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
//  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);      /*ʹ��SWD ����JTAG*/
//	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M
//	
//	ADC_DeInit(ADC1); //��λ ADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ
//	
//	// ADC1 ����
//  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC1�����ڶ���ģʽ
//  ADC_InitStructure.ADC_ScanConvMode = ENABLE;//ʹ��ɨ��
//  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;;//ADCת������������ģʽ
//  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//���������ת��
//  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ת�������Ҷ���
//  ADC_InitStructure.ADC_NbrOfChannel = 3;//�涨��˳����й���ת���� ADC ͨ������Ŀ,��Χ�� 1 �� 16��
//  ADC_Init(ADC1, &ADC_InitStructure); //��ʼ��ADC 
//	
//	//ADC1ѡ���ŵ�,˳���ȼ�,����ʱ��239.5������
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);    
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5); 
////  ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 3, ADC_SampleTime_239Cycles5); 
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 4, ADC_SampleTime_239Cycles5); 
//	
//	//ʹ��ADC1ģ��DMA
////  ADC_DMACmd(ADC1, ENABLE); 
//	//��ADC1
//  ADC_Cmd(ADC1, ENABLE);
//	
//	//����ADC1У׼�Ĵ��� 
//  ADC_ResetCalibration(ADC1); 
// //�ȴ�ADC1У׼�������
//  while(ADC_GetResetCalibrationStatus(ADC1));  

//  //��ʼADC1У׼
//  ADC_StartCalibration(ADC1); 
// //�ȴ�ADC1У׼���
//  while(ADC_GetCalibrationStatus(ADC1)); 
}


void AdcGpioConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO, ENABLE);	 //ʹ��PA�˿�ʱ��
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//������������
//	GPIO_Init(GPIOA, &GPIO_InitStructure);   //��ʼ��GPIO�˿�
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_7;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		 //���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		 //���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��
}

u16 Get_Adc(u8 ch)   
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_28Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ))//�ȴ�ת������
	{
  }

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}

u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
	}
	return temp_val/times;
} 

