#ifndef __USART_H
#define __USART_H
#include <stm32f10x.h>
#ifndef USART_C_//如果没有定义
#define USART_Ex_ extern
#else
#define USART_Ex_
#endif

#include "BufferManage.h"


#define	net_none  0
#define	net_status_offline  1
#define	net_status_local  2
#define	net_status_cloud  3
#define net_status_updating  4
#define	net_status_uap 5
#define	net_status_unprov 6

USART_Ex_ char module_net_state;

USART_Ex_ buff_manage_struct buff_manage_struct1;
USART_Ex_ buff_manage_struct buff_manage_struct_t;

USART_Ex_ buff_manage_struct buff_manage_struct_net;


#define Usart1ReadLen 1460  //串口1接收数据数组大小

#define Usart1BufferMemoryLen 1460 //串口接收缓存区大小
#define Usart1BufferMemoryReadLen 500 //提取缓存区的数据的数组大小
#define Usart1BufferMemoryManageLen 10 //串口接收缓存区管理

USART_Ex_ char Usart1ReadBuff[Usart1ReadLen];  //接收数据缓存
USART_Ex_ u32  Usart1ReadCnt;//串口1接收到的数据个数

USART_Ex_ u32   Usart1SendDataCnt;//串口中断发送数据的个数

USART_Ex_ u8   Usart1BufferMemory[Usart1BufferMemoryLen];//接收数据缓存区
USART_Ex_ u8   Usart1BufferMemoryRead[Usart1BufferMemoryReadLen];//提取缓存区的数据的数组
USART_Ex_ u32  Usart1BufferMemoryManage[Usart1BufferMemoryManageLen];//管理缓存区

USART_Ex_ u8   Usart1IdelCnt;

void uart_init(u32 bound);//串口1初始化
void UsartOutStr(unsigned char *c,uint32_t cnt);//串口发送字符串数据
void UsartOutChar(unsigned char c);//串口发送一个字节

/**
* @brief  串口中断发送数据
* @param  c:数据的首地址  cnt:发送的数据个数
* @param  None
* @param  None
* @retval None
* @example 
**/
void UsartOutStrIT(unsigned char *c,uint32_t cnt);
#endif


