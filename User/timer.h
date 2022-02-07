#ifndef __TIMER_H
#define __TIMER_H

#ifndef _TIME_C_//如果没有定义  _TIME_C_
#define _TIME_Ex_ extern
#else
#define _TIME_Ex_
#endif

#include "stm32f10x.h"


_TIME_Ex_ u32 timer_test;

#define event_20minute (3000*60*20)

//#define event_20minute (3000*10)

typedef struct{
	u32 cnt_timer_Ltemperature;
	u32 cnt_timer_temperatureH;
	
	u32 cnt_timer_Lhumidity;
	u32 cnt_timer_humidityH;

	u32 cnt_timer_Lillumination;
	u32 cnt_timer_illuminationH;
	
	u32 cnt_timer_Lfertility;
	u32 cnt_timer_fertilityH;
}event_typedef;

_TIME_Ex_ event_typedef event_t;

_TIME_Ex_ u32 fertility_control_cnt;

_TIME_Ex_ void Timer4_Config(void);

_TIME_Ex_ void Timer2_Config(void);

_TIME_Ex_ int uart_send_delay;

_TIME_Ex_ u32 key_down_time_ms;


#endif

