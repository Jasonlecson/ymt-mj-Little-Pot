#ifndef __MYIIC_H
#define __MYIIC_H
#include <stm32f10x.h>
#include "System.h"


//IO方向设置
#define BH1750_SDA_IN()  {GPIOB->CRL&=0XFFF0FFFF;GPIOB->CRL|=8<<16;}
#define BH1750_SDA_OUT() {GPIOB->CRL&=0XFFF0FFFF;GPIOB->CRL|=3<<16;}

//IO操作函数	 
#define BH1750_SCL    PBout(3) //SCL
#define BH1750_SDA    PBout(4) //SDA	 
#define BH1750_READ_SDA   PBin(4)  //输入SDA 

//IIC所有操作函数
void BH1750_Init(void);        //初始化IIC的IO口				 
void BH1750_Start(void);				//发送IIC开始信号
void BH1750_Stop(void);	  			//发送IIC停止信号
u8 BH1750_Send_Byte(u8 txd);			//IIC发送一个字节
u8 BH1750_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 BH1750_Wait_Ack(void); 				//IIC等待ACK信号
void BH1750_Ack(void);					//IIC发送ACK信号
void BH1750_NAck(void);				//IIC不发送ACK信号

void BH1750_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 BH1750_Read_One_Byte(u8 daddr,u8 addr);	  

int BH1750_read_byte(char addr);


//获取光照强度
int illumination_get(void);

#endif
















