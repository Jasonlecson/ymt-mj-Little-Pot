#ifndef __USART_H
#define __USART_H
#include <stm32f10x.h>
#ifndef USART_C_//���û�ж���
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


#define Usart1ReadLen 1460  //����1�������������С

#define Usart1BufferMemoryLen 1460 //���ڽ��ջ�������С
#define Usart1BufferMemoryReadLen 500 //��ȡ�����������ݵ������С
#define Usart1BufferMemoryManageLen 10 //���ڽ��ջ���������

USART_Ex_ char Usart1ReadBuff[Usart1ReadLen];  //�������ݻ���
USART_Ex_ u32  Usart1ReadCnt;//����1���յ������ݸ���

USART_Ex_ u32   Usart1SendDataCnt;//�����жϷ������ݵĸ���

USART_Ex_ u8   Usart1BufferMemory[Usart1BufferMemoryLen];//�������ݻ�����
USART_Ex_ u8   Usart1BufferMemoryRead[Usart1BufferMemoryReadLen];//��ȡ�����������ݵ�����
USART_Ex_ u32  Usart1BufferMemoryManage[Usart1BufferMemoryManageLen];//��������

USART_Ex_ u8   Usart1IdelCnt;

void uart_init(u32 bound);//����1��ʼ��
void UsartOutStr(unsigned char *c,uint32_t cnt);//���ڷ����ַ�������
void UsartOutChar(unsigned char c);//���ڷ���һ���ֽ�

/**
* @brief  �����жϷ�������
* @param  c:���ݵ��׵�ַ  cnt:���͵����ݸ���
* @param  None
* @param  None
* @retval None
* @example 
**/
void UsartOutStrIT(unsigned char *c,uint32_t cnt);
#endif


