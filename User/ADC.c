#include "include.h"
#include "ADC.h"

/**
* @brief  配置ADC函数
* @param  
* @param  None
* @param  None
* @retval None
* @example 
**/
void ADC_Config(void)
{
  ADC_InitTypeDef ADC_InitStructure; 
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_ADC1	, ENABLE );	  //使能ADC1通道时钟
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);      /*使能SWD 禁用JTAG*/

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M
  

	ADC_DeInit(ADC1);  //复位ADC1 

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 5;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

  
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
}

/**
* @brief  DMA模式下配置ADC函数
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
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_ADC1	, ENABLE );	  //使能ADC1通道时钟
//  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);      /*使能SWD 禁用JTAG*/
//	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M
//	
//	ADC_DeInit(ADC1); //复位 ADC1,将外设 ADC1 的全部寄存器重设为缺省值
//	
//	// ADC1 配置
//  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC1工作在独立模式
//  ADC_InitStructure.ADC_ScanConvMode = ENABLE;//使能扫描
//  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;;//ADC转换工作在连续模式
//  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//由软件控制转换
//  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//转换数据右对齐
//  ADC_InitStructure.ADC_NbrOfChannel = 3;//规定了顺序进行规则转换的 ADC 通道的数目,范围是 1 到 16。
//  ADC_Init(ADC1, &ADC_InitStructure); //初始化ADC 
//	
//	//ADC1选择信道,顺续等级,采样时间239.5个周期
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);    
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5); 
////  ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 3, ADC_SampleTime_239Cycles5); 
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 4, ADC_SampleTime_239Cycles5); 
//	
//	//使能ADC1模块DMA
////  ADC_DMACmd(ADC1, ENABLE); 
//	//打开ADC1
//  ADC_Cmd(ADC1, ENABLE);
//	
//	//重置ADC1校准寄存器 
//  ADC_ResetCalibration(ADC1); 
// //等待ADC1校准重置完成
//  while(ADC_GetResetCalibrationStatus(ADC1));  

//  //开始ADC1校准
//  ADC_StartCalibration(ADC1); 
// //等待ADC1校准完成
//  while(ADC_GetCalibrationStatus(ADC1)); 
}


void AdcGpioConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO, ENABLE);	 //使能PA端口时钟
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//浮空输入引脚
//	GPIO_Init(GPIOA, &GPIO_InitStructure);   //初始化GPIO端口
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_7;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		 //输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		 //输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化
}

u16 Get_Adc(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_28Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ))//等待转换结束
	{
  }

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
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

