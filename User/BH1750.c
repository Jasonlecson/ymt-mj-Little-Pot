#include "BH1750.h"
#include "delay.h"
#include <math.h>
#include "main.h"


//��ʼ��IIC
void BH1750_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO, ENABLE);	 //ʹ��PA�˿�ʱ��
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	BH1750_SCL=1;
	BH1750_SDA=1;
}
//����IIC��ʼ�ź�
void BH1750_Start(void)
{
	BH1750_SDA_OUT();     //sda�����
//	IIC_SDA=1;
//	delay_us(1);
//	IIC_SDA=0;
	BH1750_SDA=1;
	delay_us(1);
	BH1750_SCL=1;
	delay_us(5);
 	BH1750_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(5);
	BH1750_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
	delay_us(5);
}	  
//����IICֹͣ�ź�
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
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 BH1750_Wait_Ack(void)
{
	u8 ucErrTime=0;
	BH1750_SDA_IN();      //SDA����Ϊ����  
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
	BH1750_SCL=0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
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
//������ACKӦ��		    
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
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
u8 BH1750_Send_Byte(u8 txd)
{                        
	u8 t;   
	u8 ucErrTime=0;
	BH1750_SDA_OUT(); 	    
	BH1750_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
	
	for(t=0;t<8;t++)
	{              
		BH1750_SDA=(txd&0x80)>>7;
		txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		BH1750_SCL=1;
		delay_us(2); 
		BH1750_SCL=0;	
		delay_us(2);
	}	 
	BH1750_SDA_IN();//SDA����Ϊ����
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
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 BH1750_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	BH1750_SDA_IN();//SDA����Ϊ����
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
			BH1750_NAck();//����nACK
	else
			BH1750_Ack(); //����ACK   
	return receive;
}



int BH1750_read_byte(char addr){
	u8 ack=0;
	u8 data;
	BH1750_Start();
	ack = BH1750_Send_Byte(0x94);//��Ҫд���㶫��
	delay_us(2); 
	//��Ҫ����Ӧ��
	if(!ack) return -1;
	
	ack = BH1750_Send_Byte(addr);//�������ַ
	delay_us(2); 
	//��Ҫ����Ӧ��
	if(!ack) return -1;
	
	BH1750_Start();
	ack = BH1750_Send_Byte(0x95);//��Ҫ��ȡ�ϴθ����ַ�����ֵ
	delay_us(2); 
	//��Ҫ����Ӧ��
	if(!ack) return -1;
	
	data = BH1750_Read_Byte(0);//��ȡһ���ֽ�,����ҪӦ��
	
	BH1750_Stop();
	return(data);
}

//��ȡ����ǿ��
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
			ack = BH1750_Send_Byte(0x46);//��ַ+д
			delay_us(2); 
			//��Ҫ����Ӧ��
			if(!ack) return -1;
			BH1750_Send_Byte(0x01);//����
			BH1750_Stop();

			BH1750_Start();
			ack = BH1750_Send_Byte(0x46);//��ַ+д
			delay_us(2); 
			//��Ҫ����Ӧ��
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
			//��Ҫ����Ӧ��
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

