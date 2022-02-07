#ifndef __USART_H
#define __USART_H
#include <stm32f10x.h>
#ifndef USART_C_//如果没有定义
#define USART_Ex_ extern
#else
#define USART_Ex_
#endif

#include "LoopList.h"

/****************************串口1****************************/
//接收环形队列
USART_Ex_ rb_t rb_t_usart1_read;
#define rb_t_usart1_read_buff_len 1024
USART_Ex_ unsigned char rb_t_usart1_read_buff[rb_t_usart1_read_buff_len];
//接收计数
USART_Ex_ int usart1_read_count;


//自定义空闲中断检测
USART_Ex_ int usart1_idle_read_count;//对比接收个数是否改变
USART_Ex_ int usart1_idle_cnt;
USART_Ex_ unsigned char usart1_idle_flag;//标志



USART_Ex_ u32   Usart1SendDataCnt;//串口中断发送数据的个数


/**
* @brief  串口1自定义空闲中断检测(放到1ms定时器)
* @param  IdleCnt: 空闲时间
* @param  None
* @param  None
* @retval None
* @example 
**/
void Usart1IdleLoop(int IdleCnt);

void uart_init(u32 bound);//串口1初始化
void UsartOutStr(unsigned char *c,uint32_t cnt);//串口发送字符串数据
void UsartOutChar(unsigned char c);//串口发送一个字节
void UsartOutStrIT(unsigned char *c,uint32_t cnt);//串口中断发送数据
#endif


