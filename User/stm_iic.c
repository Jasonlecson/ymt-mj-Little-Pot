#define STM_IIC_C_
#include "stm_iic.h"


void stm_iic_init(void){
	/*Clock GPIO*/
	GPIO_InitTypeDef  GPIO_InitStructure;
	I2C_InitTypeDef   I2C_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1|RCC_APB1Periph_I2C2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_10;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_11;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化
	
	/*I2C1*/
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;//ACK
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = I2C1_SPEED;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_OwnAddress1 = I2C1_OwnAddress;
	I2C_Init(I2C1,&I2C_InitStructure);
	
	
}


void stm_iic_read_byte(I2C_TypeDef* I2Cx,uint8_t *data,u8 data_len){
	if(!I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)){//NO BUSY
		I2C_GenerateSTART(I2C1, ENABLE);//START
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

		I2C_Send7bitAddress(I2C1, (I2C1_OwnAddress<<1)+1, I2C_Direction_Transmitter);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
		
		
		
		I2C_AcknowledgeConfig(I2C1, DISABLE);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
		*data  = I2C_ReceiveData(I2C1);
		
	}
}
	





