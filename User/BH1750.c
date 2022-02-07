#include "BH1750.h"
#include "delay.h"
#include <math.h>
#include "main.h"


//初始化IIC
void BH1750_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO, ENABLE);	 //使能PA端口时钟
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	BH1750_SCL=1;
	BH1750_SDA=1;
}
//产生IIC起始信号
void BH1750_Start(void)
{
	BH1750_SDA_OUT();     //sda线输出
//	IIC_SDA=1;
//	delay_us(1);
//	IIC_SDA=0;
	BH1750_SDA=1;
	delay_us(1);
	BH1750_SCL=1;
	delay_us(5);
 	BH1750_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(5);
	BH1750_SCL=0;//钳住I2C总线，准备发送或接收数据 
	delay_us(5);
}	  
//产生IIC停止信号
void BH1750_Stop(void)
{			
	BH1750_SDA_OUT();
	BH1750_SDA=0;
	delay_us(1);
	BH1750_SCL=1;
	delay_us(5);
	BH1750_SDA=1;
	delay_us(5);
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 BH1750_Wait_Ack(void)
{
	u8 ucErrTime=0;
	BH1750_SDA_IN();      //SDA设置为输入  
	BH1750_SDA=1;delay_us(1);	   
	BH1750_SCL=1;delay_us(1);	 
	while(BH1750_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			BH1750_Stop();
			return 1;
		}
	}
	BH1750_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void BH1750_Ack(void)
{
	BH1750_SCL=0;
	BH1750_SDA_OUT();
	BH1750_SDA=0;
	delay_us(2);
	BH1750_SCL=1;
	delay_us(2);
	BH1750_SCL=0;
}
//不产生ACK应答		    
void BH1750_NAck(void)
{
	BH1750_SCL=0;
	BH1750_SDA_OUT();
	BH1750_SDA=1;
	delay_us(2);
	BH1750_SCL=1;
	delay_us(2);
	BH1750_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
u8 BH1750_Send_Byte(u8 txd)
{                        
	u8 t;   
	u8 ucErrTime=0;
	BH1750_SDA_OUT(); 	    
	BH1750_SCL=0;//拉低时钟开始数据传输
	
	for(t=0;t<8;t++)
	{              
		BH1750_SDA=(txd&0x80)>>7;
		txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		BH1750_SCL=1;
		delay_us(2); 
		BH1750_SCL=0;	
		delay_us(2);
	}	 
	BH1750_SDA_IN();//SDA设置为输入
	delay_us(10);
	BH1750_SCL=1;
	
	while(BH1750_READ_SDA==1 && ucErrTime<200){
		ucErrTime++;
	}
	if(BH1750_READ_SDA==0 && ucErrTime<200){
		BH1750_SCL=0;
		delay_us(2);
		return 1;
	}else{
		BH1750_SCL=0;
		delay_us(2);
		return 0;
	}
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 BH1750_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	BH1750_SDA_IN();//SDA设置为输入
	for(i=0;i<8;i++ )
	{
		BH1750_SCL=0; 
		delay_us(2);
		BH1750_SCL=1;
		receive<<=1;
		if(BH1750_READ_SDA)receive++;   
		delay_us(1); 
	}					 
	if (!ack)
			BH1750_NAck();//发送nACK
	else
			BH1750_Ack(); //发送ACK   
	return receive;
}



int BH1750_read_byte(char addr){
	u8 ack=0;
	u8 data;
	BH1750_Start();
	ack = BH1750_Send_Byte(0x94);//我要写给你东西
	delay_us(2); 
	//需要器件应答
	if(!ack) return -1;
	
	ack = BH1750_Send_Byte(addr);//给你个地址
	delay_us(2); 
	//需要器件应答
	if(!ack) return -1;
	
	BH1750_Start();
	ack = BH1750_Send_Byte(0x95);//我要读取上次给你地址里面的值
	delay_us(2); 
	//需要器件应答
	if(!ack) return -1;
	
	data = BH1750_Read_Byte(0);//读取一个字节,不需要应答
	
	BH1750_Stop();
	return(data);
}

//获取光照强度
int illumination_get(void)
{
	int value=0;
	char temp[2];
	float sensor_lignt_exponent,sensor_lignt_mantissa;
	u8 ack=0;
	int data=0;
	static int delay=0;
	
		if(delay==0){
			BH1750_Start();
			ack = BH1750_Send_Byte(0x46);//地址+写
			delay_us(2); 
			//需要器件应答
			if(!ack) return -1;
			BH1750_Send_Byte(0x01);//启动
			BH1750_Stop();

			BH1750_Start();
			ack = BH1750_Send_Byte(0x46);//地址+写
			delay_us(2); 
			//需要器件应答
			if(!ack) return -2;
			BH1750_Send_Byte(0x10);
			BH1750_Stop();
		}
		
		delay = delay+1;
		
		if(delay>=2){
			delay=0;
			BH1750_Start();
			ack = BH1750_Send_Byte(0x47);
			delay_us(2); 
			//需要器件应答
			if(!ack) return -3;

			data = BH1750_Read_Byte(1);
			data = data<<8;
			data +=0x00ff&BH1750_Read_Byte(0);

			BH1750_Stop();                        //?????? 
		}else{
			data=-4;
		}
		return data; 
}

