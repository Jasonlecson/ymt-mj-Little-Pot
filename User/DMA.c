#define DMA_C_
#include "DMA.h"

//u16  ADCBuff[GatherRow][GatherList] ={0};

void DMA_Config(void)
{
//	  //��ʼ���ṹ��
//    DMA_InitTypeDef DMA_InitStructure;
//    
//	  //����DMAʱ��
//	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	  			

//    DMA_DeInit(DMA1_Channel1);//��λDMA1�ĵ�һͨ�����ر�״̬����   
//    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1 -> DR;//����DMAԴ����ַ	  
//    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADCBuff;//*�ڴ��ַ(Ҫ����ı�����ָ��)
//    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//������Ϊ���ݴ������Դ	
//    DMA_InitStructure.DMA_BufferSize = GatherRow*GatherList;//ָ�� DMA ͨ���� DMA ����Ĵ�С����λΪ���ݵ�λ�� 
//    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//*�����ַ����	   
//    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//*�ڴ��ַ����
//    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//*�������ݵ�λ���ݿ��Ϊ 16 λ
//    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//*�ڴ����ݵ�λ���ݿ��Ϊ 16 λ	 
//	  
//	  //*DMAģʽ��һ�δ���/ѭ��
//// 		DMA_Mode_Circular ������ѭ������ģʽ
////    DMA_Mode_Normal ��������������ģʽ
////     DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;	
//    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		
//    DMA_InitStructure.DMA_Priority = DMA_Priority_High;//*���ȼ�����  
//    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//*��ֹ�ڴ浽�ڴ�Ĵ���	
//	  //*����DMA1��1ͨ��		   
//    DMA_Init(DMA1_Channel1, &DMA_InitStructure); 
//		
//	  //ʹ��DMA
//	  DMA_Cmd (DMA1_Channel1,ENABLE);
//		
//		//ʹ��ADC1�����ʼת��
//    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}



