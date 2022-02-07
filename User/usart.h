#ifndef __USART_H
#define __USART_H
#include <stm32f10x.h>
#ifndef USART_C_//���û�ж���
#define USART_Ex_ extern
#else
#define USART_Ex_
#endif

#include "LoopList.h"

/****************************����1****************************/
//���ջ��ζ���
USART_Ex_ rb_t rb_t_usart1_read;
#define rb_t_usart1_read_buff_len 1024
USART_Ex_ unsigned char rb_t_usart1_read_buff[rb_t_usart1_read_buff_len];
//���ռ���
USART_Ex_ int usart1_read_count;


//�Զ�������жϼ��
USART_Ex_ int usart1_idle_read_count;//�ԱȽ��ո����Ƿ�ı�
USART_Ex_ int usart1_idle_cnt;
USART_Ex_ unsigned char usart1_idle_flag;//��־



USART_Ex_ u32   Usart1SendDataCnt;//�����жϷ������ݵĸ���


/**
* @brief  ����1�Զ�������жϼ��(�ŵ�1ms��ʱ��)
* @param  IdleCnt: ����ʱ��
* @param  None
* @param  None
* @retval None
* @example 
**/
void Usart1IdleLoop(int IdleCnt);

void uart_init(u32 bound);//����1��ʼ��
void UsartOutStr(unsigned char *c,uint32_t cnt);//���ڷ����ַ�������
void UsartOutChar(unsigned char c);//���ڷ���һ���ֽ�
void UsartOutStrIT(unsigned char *c,uint32_t cnt);//�����жϷ�������
#endif


