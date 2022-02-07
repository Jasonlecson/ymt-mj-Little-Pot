#ifndef __AHT10_I2C_H
#define __AHT10_I2C_H
#include <stm32f10x.h>
#include "delay.h"
#include "System.h"

//IO��������	
#define AHT10PORT  GPIOB    //
#define AH_SCL     GPIO_Pin_6
#define AH_SDA     GPIO_Pin_7
#define IIC_SCL    PBout(6) //SCL
#define IIC_SDA    PBout(7) //SDA	 
#define READ_SDA   PBin(7)  //����SDA

#define SDA_IN()  {AHT10PORT->CRL&=0X0FFFFFFF;AHT10PORT->CRL|=(u32)8<<28;}
#define SDA_OUT() {AHT10PORT->CRL&=0X0FFFFFFF;AHT10PORT->CRL|=(u32)3<<28;}


void AHT10_Init(void);                //��ʼ��IIC��IO��	
void  read_AHT10_once(void);          //��ȡ��ʪ�ȣ�����ѭ���У�
extern volatile	int H1;   //ʪ�ȣ�552Ϊ55.2%
extern volatile int T1;   //�¶ȣ�255Ϊ25.5��

/*	printf("\r\n");
		printf("�¶�:%d%d.%d",T1/100,(T1/10)%10,T1%10);
		printf("ʪ��:%d%d.%d",H1/100,(H1/10)%10,H1%10);
		printf("\r\n");
*/
#endif
