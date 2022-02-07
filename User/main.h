#ifndef __MAIN_H_
#define __MAIN_H_
#include <stm32f10x.h>

#ifndef __MAIN_C_
#define __MAIN_EXT_ extern
#else
#define __MAIN_EXT_
#endif


__MAIN_EXT_ u16 HardwareVersion;

//��������ʹ��
#define MainBufferLen 200
__MAIN_EXT_ char MainBuffer[MainBufferLen];//��������,ȫ��ͨ��
__MAIN_EXT_ u32  MainLen;      //ȫ��ͨ�ñ���
__MAIN_EXT_ char *MainString;    //ȫ��ͨ�ñ���

/*************BufferManager*****************/
//__MAIN_EXT_ u8  BFbuff[1500];
//__MAIN_EXT_ u32 BFManagebuff[40];
//__MAIN_EXT_ u8  BFSendBuff[1500];
/*************BufferManager*****************/

__MAIN_EXT_ u16 send_get_down_cnt;//���Ʒ���get_down
__MAIN_EXT_ u32  get_time_stamp_cnt; //���ƶ�ʱ��ȡʱ���
__MAIN_EXT_ int  testing_time_appear_timer_cnt;//�����¶�,ʪ��,����,��������ϱ�ʱ��

__MAIN_EXT_ u16 InitModelCnt;
__MAIN_EXT_ u16 SendModelCnt;

__MAIN_EXT_ u16 InitBleCnt;
__MAIN_EXT_ u16 SendBleCnt;

__MAIN_EXT_ int DS18B20Cnt;
__MAIN_EXT_ int sensor_ligntCnt;
__MAIN_EXT_ int  water_stageCnt;

__MAIN_EXT_ u32  SmartConfigCnt;
__MAIN_EXT_ char SmartConfigFlage;
__MAIN_EXT_ char WateringFlag;//�ڽ�ˮ:1
__MAIN_EXT_ u16  WateringDelayTimer;//��ˮ��ʱʱ��

__MAIN_EXT_ u32  WateringCirculation;//ÿ��1��Сʱ��һ��ˮ


__MAIN_EXT_ int EventReportTimeCnt;
__MAIN_EXT_ u32 EventReportTimeCnt1;
__MAIN_EXT_ u32 EventReportTimeCnt2;

__MAIN_EXT_ char module_net_state;



/**********2020/02/23***********/
//ָʾ��Ĭ�Ͽ�,ˮλ������Ԥ��
__MAIN_EXT_ int  water_stage,water_stageCopy;//ˮλ 0-100


__MAIN_EXT_ char water_stage_empty_flag; //ȱˮ
__MAIN_EXT_ char water_stage_empty_flagCopy; //ȱˮ
__MAIN_EXT_ u16  water_stage_empty_cnt1;//ȱˮ
__MAIN_EXT_ char water_stage_empty_run_flage;

__MAIN_EXT_ char water_stage_water_flag; //��ˮ
__MAIN_EXT_ char water_stage_water_flagCopy; //��ˮ
__MAIN_EXT_ u16  water_stage_water_cnt1;//��ˮ

__MAIN_EXT_ char water_stage_full_flag; //ˮ��
__MAIN_EXT_ char water_stage_full_flagCopy; //ˮ��
__MAIN_EXT_ u16  water_stage_full_cnt1;//ˮ��
__MAIN_EXT_ char water_stage_full_run_flage;

__MAIN_EXT_ char water_stage_nofull_flag;
__MAIN_EXT_ char water_stage_nofull_flagCopy;
__MAIN_EXT_ u16  water_stage_nofull_cnt1;
/*********************/


__MAIN_EXT_ u16 send_get_net_cnt;//���Ʒ���get_net��ʱ��

__MAIN_EXT_ int LEDState;

__MAIN_EXT_ long long timestamp;//��ǰʱ���

__MAIN_EXT_ u8  InitBleFlag;


__MAIN_EXT_ int  illumination_statistics_test;//����ʹ��
__MAIN_EXT_ int illumination_statistics_timer_delay;//�ڶ�ʱ�������ۼ�,����ͳ��ʱ����

#endif
