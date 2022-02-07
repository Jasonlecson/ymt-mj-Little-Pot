#ifndef __DMA1_H
#define	__DMA1_H
#include "stm32f10x.h"
#ifndef DMA_C_
#define DMA_C_ extern
#else
#define DMA_C_
#endif


//#define  GatherRow  20  //采集行数
//#define  GatherList 2 //采集列数

//DMA_C_ u16  ADCBuff[GatherRow][GatherList];

void DMA_Config(void);

#endif 

