#ifndef __ADC_H_
#define __ADC_H_
#include <stm32f10x.h>

void ADC_Config(void);
void DMA_ADC_Config(void);
u16 Get_Adc(u8 ch);
u16 Get_Adc_Average(u8 ch,u8 times);

void AdcGpioConfig(void);

#endif
