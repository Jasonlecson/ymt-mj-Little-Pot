#ifndef __MAIN_H_
#define __MAIN_H_
#include <stm32f10x.h>

#ifndef __MAIN_C_
#define __MAIN_EXT_ extern
#else
#define __MAIN_EXT_
#endif

//缓存数据使用
#define MainBufferLen 50
__MAIN_EXT_ char MainBuffer[MainBufferLen];//缓存数据,全局通用
__MAIN_EXT_ u32  MainLen;      //全局通用变量
__MAIN_EXT_ char *MainString;    //全局通用变量


__MAIN_EXT_ char IP[20];//IP地址
__MAIN_EXT_ char Port[5];//端口号

__MAIN_EXT_ u32  GetVersionInfoCnt;//间隔发送  程序版本 使用

__MAIN_EXT_ char ReadEOTFlag;//接收到接收信号
__MAIN_EXT_ u32 ReadEOTCnt;//

__MAIN_EXT_ u32  GetNetTimerCnt;
__MAIN_EXT_ u8  InitBleFlag;
__MAIN_EXT_ u32  InitBleTimerCnt;
__MAIN_EXT_ u32  GetBleTimerCnt;
__MAIN_EXT_ char SmartConfigFlage;
__MAIN_EXT_ u32  SmartConfigCnt;
__MAIN_EXT_ u32  module_net_state_led_cnt;
__MAIN_EXT_ u32  RuningUpdataDelay;

#endif
