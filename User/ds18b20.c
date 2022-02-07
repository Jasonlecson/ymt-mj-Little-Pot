#include "ds18b20.h"
#include "delay.h"	
#include "parse.h"  

//��λDS18B20
void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT(); //SET PA0 OUTPUT
	DS18B20_DQ_OUT=0; //����DQ
	delay_us(750);    //����750us
	DS18B20_DQ_OUT=1; //DQ=1 
	delay_us(15);     //15US
}
//�ȴ�DS18B20�Ļ�Ӧ
//����1:δ��⵽DS18B20�Ĵ���
//����0:����
u8 DS18B20_Check(void) 	   
{   
	u8 retry=0;
	DS18B20_IO_IN();//SET PA0 INPUT	 
	while (DS18B20_DQ_IN&&retry<200)
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=200)return 1;
	else retry=0;
	while (!DS18B20_DQ_IN&&retry<240)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)return 1;	    
	return 0;
}
//��DS18B20��ȡһ��λ
//����ֵ��1/0
u8 DS18B20_Read_Bit(void) 			 // read one bit
{
	u8 data;
	DS18B20_IO_OUT();//SET PA0 OUTPUT
	DS18B20_DQ_OUT=0; 
	delay_us(2);
	DS18B20_DQ_OUT=1; 
	DS18B20_IO_IN();//SET PA0 INPUT
	delay_us(12);
	if(DS18B20_DQ_IN)data=1;
		else data=0;	 
	delay_us(50);           
	return data;
}
//��DS18B20��ȡһ���ֽ�
//����ֵ������������
u8 DS18B20_Read_Byte(void)    // read one byte
{        
	u8 i,j,dat;
	dat=0;
	for (i=1;i<=8;i++) 
	{
		j=DS18B20_Read_Bit();
		dat=(j<<7)|(dat>>1);
	}						    
	return dat;
}
//дһ���ֽڵ�DS18B20
//dat��Ҫд����ֽ�
void DS18B20_Write_Byte(u8 dat)     
{             
	u8 j;
	u8 testb;
	DS18B20_IO_OUT();//SET PA0 OUTPUT;
	for (j=1;j<=8;j++) 
	{
		testb=dat&0x01;
		dat=dat>>1;
		if (testb) 
		{
			DS18B20_DQ_OUT=0;// Write 1
			delay_us(2);                            
			DS18B20_DQ_OUT=1;
			delay_us(60);             
		}
		else 
		{
			DS18B20_DQ_OUT=0;// Write 0
			delay_us(60);             
			DS18B20_DQ_OUT=1;
			delay_us(2);                          
		}
	}
}
//��ʼ�¶�ת��
void DS18B20_Start(void)// ds1820 start convert
{   						               
	DS18B20_Rst();	   
	DS18B20_Check();	 
	DS18B20_Write_Byte(0xcc);// skip rom
	DS18B20_Write_Byte(0x44);// convert
} 
//��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
//����1:������
//����0:����    	 
u8 DS18B20_Init(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��PORTA��ʱ�� 
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				//PORTA2 �������
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOA,GPIO_Pin_2);    //���1
	
	DS18B20_Rst();

	return DS18B20_Check();
}  

/********************************************************/  
/*DS18B20?CRC8????*/  
/********************************************************/   
u8 calcrc_1byte(u8 abyte)    
{    
   u8 i,crc_1byte;     
   crc_1byte=0;                //??crc_1byte???0   
   for(i = 0; i < 8; i++)    
    {    
      if(((crc_1byte^abyte)&0x01))    
         {    
           crc_1byte^=0x18;     
           crc_1byte>>=1;    
           crc_1byte|=0x80;    
          }          
       else     
          crc_1byte>>=1;   
       abyte>>=1;          
     }   
     return crc_1byte;   
}   
u8 calcrc_bytes(u8 *p,u8 len)  
{  
 u8 crc=0;  
 while(len--) //len??????????   
  {  
    crc=calcrc_1byte(crc^*p++);  
  }  
 return crc;  //??????crc?0,???????   
}  


//��ds18b20�õ��¶�ֵ
//���ȣ�0.1C
//����ֵ���¶�ֵ ��-55~125�� 
short DS18B20_Get_Temp(void)
{
	u8 ack=0;
	u8 temp;
	u8 data[9];
	
	short tem;
	DS18B20_Start ();                    // ds1820 start convert
	DS18B20_Rst();
	ack = DS18B20_Check();
	if(ack) return 255; 
	DS18B20_Write_Byte(0xcc);// skip rom
	DS18B20_Write_Byte(0xbe);// convert	    
	data[0]=DS18B20_Read_Byte(); // LSB   
	data[1]=DS18B20_Read_Byte(); // MSB  
	    	  
	data[2] = DS18B20_Read_Byte();
	data[3] = DS18B20_Read_Byte();
	data[4] = DS18B20_Read_Byte();
	data[5] = DS18B20_Read_Byte();
	data[6] = DS18B20_Read_Byte();
	data[7] = DS18B20_Read_Byte();
	data[8] = DS18B20_Read_Byte();
	
	if(CRC8_Table(data,8) != data[8])
	{
		return 255;
	}
	
	/*��ҪCRC8У��,���У��ʧ�ܷ��� 255*/
	if(data[1]>7)
	{
		data[1]=~data[1];
		data[0]=~data[0]; 
		temp=0;//�¶�Ϊ��  
	}else temp=1;//�¶�Ϊ��	  	  
	tem=data[1]; //��ø߰�λ
	tem<<=8;    
	tem+=data[0];//��õװ�λ
	tem=(float)tem*0.0625;//ת��     
	if(temp)return tem; //�����¶�ֵ
	else return -tem;    
} 
 
