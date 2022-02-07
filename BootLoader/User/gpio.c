#include "include.h"

/**
* @brief  设置用到的GPIO
* @param  None
* @param  None
* @param  None
* @retval None
* @example 
**/
void GpioInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO, ENABLE);	 //使能PA端口时钟
// 	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);      /*使能SWD 禁用JTAG*/
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					     //根据设定参数初始化
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;                             
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;                             
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化
	
	
	//GPIO_SetBits(GPIOA,GPIO_Pin_11);
	PAout(11) = 1;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;                             
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化
	GPIO_SetBits(GPIOA,GPIO_Pin_11);
	PAout(11) = 1;
	
	PAout(5) = 1;
}
