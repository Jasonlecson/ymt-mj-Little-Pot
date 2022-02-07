#ifndef __KEY_H_
#define __KEY_H_
#include "stm32f10x.h"
#ifndef __KEY_C_
#define __KEY_Ex_  extern
#else 
#define __KEY_Ex_
#endif


#define KeySmartConfigDown 0  //0- 低电平认为是按下  1-高电平认为是按下
#define SmartConfigKeyPinIn PBin(0)  //检测的按键引脚
__KEY_Ex_ int KeySmartConfig[7];//消抖变量 按下标记 松开标记 按下标记拷贝  松开标记拷贝 按下时间(Ms) 松开时间(Ms)
void SmartConfigKey(void);
	
#endif
