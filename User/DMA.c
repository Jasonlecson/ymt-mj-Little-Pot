#define DMA_C_
#include "DMA.h"

//u16  ADCBuff[GatherRow][GatherList] ={0};

void DMA_Config(void)
{
//	  //初始化结构体
//    DMA_InitTypeDef DMA_InitStructure;
//    
//	  //开启DMA时钟
//	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	  			

//    DMA_DeInit(DMA1_Channel1);//复位DMA1的第一通道，关闭状态设置   
//    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1 -> DR;//设置DMA源：地址	  
//    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADCBuff;//*内存地址(要传输的变量的指针)
//    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//外设作为数据传输的来源	
//    DMA_InitStructure.DMA_BufferSize = GatherRow*GatherList;//指定 DMA 通道的 DMA 缓存的大小，单位为数据单位。 
//    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//*外设地址不增	   
//    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//*内存地址增加
//    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//*外设数据单位数据宽度为 16 位
//    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//*内存数据单位数据宽度为 16 位	 
//	  
//	  //*DMA模式：一次传输/循环
//// 		DMA_Mode_Circular 工作在循环缓存模式
////    DMA_Mode_Normal 工作在正常缓存模式
////     DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;	
//    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		
//    DMA_InitStructure.DMA_Priority = DMA_Priority_High;//*优先级：高  
//    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//*禁止内存到内存的传输	
//	  //*配置DMA1的1通道		   
//    DMA_Init(DMA1_Channel1, &DMA_InitStructure); 
//		
//	  //使能DMA
//	  DMA_Cmd (DMA1_Channel1,ENABLE);
//		
//		//使能ADC1软件开始转换
//    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}



