#ifndef CONFIG8266_H_
#define CONFIG8266_H_

#ifndef CONFIG8266_C_//���û�ж���  _TIME_C_
#define CONFIG8266_EX_ extern
#else
#define CONFIG8266_EX_
#endif

#include <stm32f10x.h>


#define Rst8266PinOut PBout(2)       //��λ8266������
#define SmartConfigPinOut PCout(13)  //SmartConfigָʾ������

CONFIG8266_EX_ char SmartConfigFlage;//�ǲ���������
CONFIG8266_EX_ char SmartConfigSuccess;//�Ƿ������ɹ�
CONFIG8266_EX_ u32  SmartConfigCnt;//��������·������ʱʹ��
CONFIG8266_EX_ u32  Config8266LedDelay;//����8266ָʾ����ҫ

char AutoConnectTCP(void);//����TCP������
char SmartConfig(void);//����SmartConfig ��WIFI����
void Init8266(void);//����·����
void Rst8266(void);//��λ8266
#endif

