#ifndef __MAIN_H_
#define __MAIN_H_
#include <stm32f10x.h>

#ifndef __MAIN_C_
#define __MAIN_EXT_ extern
#else
#define __MAIN_EXT_
#endif

//��������ʹ��
#define MainBufferLen 50
__MAIN_EXT_ char MainBuffer[MainBufferLen];//��������,ȫ��ͨ��
__MAIN_EXT_ u32  MainLen;      //ȫ��ͨ�ñ���
__MAIN_EXT_ char *MainString;    //ȫ��ͨ�ñ���


__MAIN_EXT_ char IP[20];//IP��ַ
__MAIN_EXT_ char Port[5];//�˿ں�

__MAIN_EXT_ u32  GetVersionInfoCnt;//�������  ����汾 ʹ��

__MAIN_EXT_ char ReadEOTFlag;//���յ������ź�
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
