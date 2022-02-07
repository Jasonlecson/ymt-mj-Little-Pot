#ifndef __MAIN_H_
#define __MAIN_H_
#include <stm32f10x.h>

#ifndef __MAIN_C_
#define __MAIN_EXT_ extern
#else
#define __MAIN_EXT_
#endif


__MAIN_EXT_ u16 HardwareVersion;

//缓存数据使用
#define MainBufferLen 200
__MAIN_EXT_ char MainBuffer[MainBufferLen];//缓存数据,全局通用
__MAIN_EXT_ u32  MainLen;      //全局通用变量
__MAIN_EXT_ char *MainString;    //全局通用变量

/*************BufferManager*****************/
//__MAIN_EXT_ u8  BFbuff[1500];
//__MAIN_EXT_ u32 BFManagebuff[40];
//__MAIN_EXT_ u8  BFSendBuff[1500];
/*************BufferManager*****************/

__MAIN_EXT_ u16 send_get_down_cnt;//控制发送get_down
__MAIN_EXT_ u32  get_time_stamp_cnt; //控制定时获取时间戳
__MAIN_EXT_ int  testing_time_appear_timer_cnt;//控制温度,湿度,光照,肥力检测上报时间

__MAIN_EXT_ u16 InitModelCnt;
__MAIN_EXT_ u16 SendModelCnt;

__MAIN_EXT_ u16 InitBleCnt;
__MAIN_EXT_ u16 SendBleCnt;

__MAIN_EXT_ int DS18B20Cnt;
__MAIN_EXT_ int sensor_ligntCnt;
__MAIN_EXT_ int  water_stageCnt;

__MAIN_EXT_ u32  SmartConfigCnt;
__MAIN_EXT_ char SmartConfigFlage;
__MAIN_EXT_ char WateringFlag;//在浇水:1
__MAIN_EXT_ u16  WateringDelayTimer;//浇水延时时间

__MAIN_EXT_ u32  WateringCirculation;//每隔1个小时浇一次水


__MAIN_EXT_ int EventReportTimeCnt;
__MAIN_EXT_ u32 EventReportTimeCnt1;
__MAIN_EXT_ u32 EventReportTimeCnt2;

__MAIN_EXT_ char module_net_state;



/**********2020/02/23***********/
//指示灯默认开,水位蜂鸣器预警
__MAIN_EXT_ int  water_stage,water_stageCopy;//水位 0-100


__MAIN_EXT_ char water_stage_empty_flag; //缺水
__MAIN_EXT_ char water_stage_empty_flagCopy; //缺水
__MAIN_EXT_ u16  water_stage_empty_cnt1;//缺水
__MAIN_EXT_ char water_stage_empty_run_flage;

__MAIN_EXT_ char water_stage_water_flag; //有水
__MAIN_EXT_ char water_stage_water_flagCopy; //有水
__MAIN_EXT_ u16  water_stage_water_cnt1;//有水

__MAIN_EXT_ char water_stage_full_flag; //水满
__MAIN_EXT_ char water_stage_full_flagCopy; //水满
__MAIN_EXT_ u16  water_stage_full_cnt1;//水满
__MAIN_EXT_ char water_stage_full_run_flage;

__MAIN_EXT_ char water_stage_nofull_flag;
__MAIN_EXT_ char water_stage_nofull_flagCopy;
__MAIN_EXT_ u16  water_stage_nofull_cnt1;
/*********************/


__MAIN_EXT_ u16 send_get_net_cnt;//控制发送get_net的时间

__MAIN_EXT_ int LEDState;

__MAIN_EXT_ long long timestamp;//当前时间戳

__MAIN_EXT_ u8  InitBleFlag;


__MAIN_EXT_ int  illumination_statistics_test;//测试使用
__MAIN_EXT_ int illumination_statistics_timer_delay;//在定时器里面累加,设置统计时间间隔

#endif
