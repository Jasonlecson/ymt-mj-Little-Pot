#ifndef __KEY_H_
#define __KEY_H_
#include "stm32f10x.h"
#ifndef __KEY_C_
#define __KEY_C_  extern
#else 
#define __KEY_C_
#endif


#define KeySmartConfigDown 0  //0- �͵�ƽ��Ϊ�ǰ���  1-�ߵ�ƽ��Ϊ�ǰ���
#define SmartConfigKeyPinIn PBin(0)  //���İ�������
__KEY_C_ int KeySmartConfig[7];//�������� ���±�� �ɿ���� ���±�ǿ���  �ɿ���ǿ��� ����ʱ��(Ms) �ɿ�ʱ��(Ms)
void SmartConfigKey(void);

__KEY_C_ char key_down_count;//��ⰴ�����µĴ���
__KEY_C_ char key_down_count_copy;//��ⰴ�����µĴ���
	
#endif