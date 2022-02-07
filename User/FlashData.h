#ifndef __FLASHDATA_H__
#define __FLASHDATA_H__
#include <stm32f10x.h>

typedef enum {/*����/��ȡ����ֵ����*/
	safe_type_temperature = 0,
  safe_type_humidity = 1,
  safe_type_illumination = 2,
	safe_type_fertility = 3,
	safe_type_air_humidity = 4,
}safe_type_typedef;

void FlashSetBotanyType(char* ch);
void FlashGetBotanyType(char* ch);

void FlashSetModeAuto(void);
void FlashSetModeManual(void);
u16 FlashGetModeWatering(void);

void FlashSetSafe(uint16_t L,uint16_t H,safe_type_typedef safe_type);
void FlashGetSafe(uint16_t* L,uint16_t* H,safe_type_typedef safe_type);

//��ˮ����:����
void FlashSetWaterCount(uint16_t data,char mode);
//��ˮ����:��ȡ
u16 FlashGetWaterCount(char mode);
//������ֲʱ��
void FlashSetDataPlant(long long data);
//��ȡ��ֲʱ��
long long FlashGetDataPlant(void);
//����ֵ:����
void FlashSetLoveValue(uint16_t data,char mode);
//����ֵ:��ȡ
u16 FlashGetLoveValue(char mode);
//�ۼ��ջ�:����
void FlashSetHarvestCount(uint16_t data);
//�ۼ��ջ�:��ȡ
u16 FlashGetHarvestCount(void);
//������ֲʱ��(�״�)
void FlashSetOneDataPlant(long long data);
//��ȡ��ֲʱ��(�״�)
long long FlashGetOneDataPlant(void);

//���Ļ���ʱ��
void FlashSetMatrixData(long long data,char mode);
//��ȡ���Ļ���ʱ��
long long FlashGetMatrixData(char mode);


//��������
void FlashSetNurtureDay(uint16_t data,char mode);
//��������
u16 FlashGetNurtureDay(char mode);


//���Ĺ���ͳ��
//data ͳ�ƵĹ���ǿ��
//ͳ�ƵĴ���
void FlashSetIlluminationStatistics(u32 data,u16 count,long long time);

//��ȡ����ͳ��
//count ���ص�ͳ�ƴ���
//return ͳ�ƵĹ���ǿ��
u32 FlashGetIlluminationStatistics(u16* count,long long *time);


//���ù���ǿ�ȱ���ֵ
void FlashSetLightAlarm(int value);
//��ȡ����ǿ�ȱ���ֵ
int FlashGetLightAlarm(void);


//���ù���ǿ��ͳ���ϱ�״̬(���)
void FlashClearIlluminationState(void);

//���ù���ǿ��ͳ���ϱ�״̬(����)
void FlashSetIlluminationState(void);

//��ȡ����ǿ��ͳ���ϱ�״̬
u16 FlashGetIlluminationState(void);

/*��������Ϣʱ����Ƿ񱨾�*///����0:������, 1:����
void FlashSetReportEvent(uint16_t data);
/*��������Ϣʱ����Ƿ񱨾�*///��ȡ 0:������, 1:����
u16 FlashGetReportEvent(void);

//����ƴ�ģʽ 0:�Զ�  1:�ֶ�
void FlashSetFillLightMode(uint16_t data);
//����ƴ�ģʽ 0:�Զ�  1:�ֶ�
u16 FlashGetFillLightMode(void);

#endif

