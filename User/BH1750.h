#ifndef __MYIIC_H
#define __MYIIC_H
#include <stm32f10x.h>
#include "System.h"


//IO��������
#define BH1750_SDA_IN()  {GPIOB->CRL&=0XFFF0FFFF;GPIOB->CRL|=8<<16;}
#define BH1750_SDA_OUT() {GPIOB->CRL&=0XFFF0FFFF;GPIOB->CRL|=3<<16;}

//IO��������	 
#define BH1750_SCL    PBout(3) //SCL
#define BH1750_SDA    PBout(4) //SDA	 
#define BH1750_READ_SDA   PBin(4)  //����SDA 

//IIC���в�������
void BH1750_Init(void);        //��ʼ��IIC��IO��				 
void BH1750_Start(void);				//����IIC��ʼ�ź�
void BH1750_Stop(void);	  			//����IICֹͣ�ź�
u8 BH1750_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 BH1750_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 BH1750_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void BH1750_Ack(void);					//IIC����ACK�ź�
void BH1750_NAck(void);				//IIC������ACK�ź�

void BH1750_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 BH1750_Read_One_Byte(u8 daddr,u8 addr);	  

int BH1750_read_byte(char addr);


//��ȡ����ǿ��
int illumination_get(void);

#endif
















