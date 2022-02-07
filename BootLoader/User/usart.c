/**
  ******************************************************************************
  * @file    usart.c
  * @author  fengwu yang
  * @version V1.0.0
  * @date    2019/10/12
  * @brief   �������ú���
  ******************************************************************************
	һ,ʹ��˵��
		1,�����³������1ms��ʱ���ж���
			
			if(Usart1ReadCnt!=0)//���ڽ��յ�����
			{
				Usart1IdleCnt++;//�����ۼ�(�ñ����ڴ��ڽ���ʱ����)
				if(Usart1IdleCnt>=Usart1IdleTime)//���� Usart1IdleTime Ms ʱ��û�н��յ�����
				{
					Usart1IdleCnt = 0;
					Usart1ReadCntCopy = Usart1ReadCnt;
					Usart1ReadCnt = 0;
					Usart1ReadFlage = 1;
				}
			}
		
		2,��ѭ������ʹ��
			while(1)
			{
				if(Usart1ReadFlage)//���ڽ��յ�һ������
				{
					Usart1ReadFlage=0;
					//��������
					UsartOutStr(Usart1ReadBuff,Usart1ReadCntCopy);//���ؽ��յ�����
				}
			}
				
  ******************************************************************************
  */
#define USART_C_
#include "include.h"
#include "IAP.h"
#include "LoopList.h"


char Usart1ReadBuff[Usart1ReadLen]={0};  //�������ݻ���
u32  Usart1ReadCnt = 0;//����1���յ������ݸ���


unsigned char *Usart1SendData;//�����жϷ������ݵĵ�ַ
u32   Usart1SendDataCnt=0;//�����жϷ������ݵĸ���


u8   Usart1BufferMemory[Usart1BufferMemoryLen];//�������ݻ�����
u8   Usart1BufferMemoryRead[Usart1BufferMemoryReadLen];//��ȡ������������
u32  Usart1BufferMemoryManage[Usart1BufferMemoryManageLen];//����������

u8   Usart1IdelCnt = 0;


char module_net_state = 0;
u8 NETcnt=0;
u8 NETcnt2=0;
u8 NETcnt3=0;
u8 NETcnt4=0;
u8 NETcnt5=0;
u8 BlueReturnCnt=0;

/**
* @brief   ����1��ʼ��
* @param   bound  ������
* @param   None
* @param   None
* @retval  None
* @warning None
* @example 
**/
void uart_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
	
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  
  //USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	
  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
//	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//�����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 
}


/**
* @brief  �����жϷ�������
* @param  c:���ݵ��׵�ַ  cnt:���͵����ݸ���
* @param  None
* @param  None
* @retval None
* @example 
**/
void UsartOutStrIT(unsigned char *c,uint32_t cnt)
{
	Usart1SendData=c;
	Usart1SendDataCnt=cnt;
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}


/**
* @brief  ���ڷ����ַ�������
* @param  *c:���͵�����ָ��  cnt:���ݸ���
* @param  None
* @param  None
* @retval None
* @example 
**/
void UsartOutStr(unsigned char *c,uint32_t cnt)
{
	while(cnt--)
	{
		USART_SendData(USART1, *c++);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
	}
}


/**
* @brief  ���ڷ���һ���ֽ�
* @param  *c:���͵�����ָ��  cnt:���ݸ���
* @param  None
* @param  None
* @retval None
* @example 
**/
void UsartOutChar(unsigned char c)
{
	USART_SendData(USART1,c);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
}


void USART1_IRQHandler(void)//����1�жϷ������
{
	u8 Res;
	int value;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		
		Usart1ReadBuff[Usart1ReadCnt] = Res;	//���յ����ݴ�������
		Usart1ReadCnt++;
		if(Usart1ReadCnt > Usart1ReadLen -10)//��ֹ�������
		{
			Usart1ReadCnt = 0;
		}
		
		Usart1IdelCnt=0;
		
		if(Res=='u'	&& BlueReturnCnt==0)BlueReturnCnt++;
		else if(Res=='c'	&& BlueReturnCnt==1)BlueReturnCnt++;
		else if(Res=='t'	&& BlueReturnCnt==2)BlueReturnCnt++;
		else if(Res==' '	&& BlueReturnCnt==3)BlueReturnCnt++;
		else if(Res=='i'	&& BlueReturnCnt==4)BlueReturnCnt++;
		else if(Res=='d'	&& BlueReturnCnt==5)BlueReturnCnt++;
		else if(Res=='\"'	&& BlueReturnCnt==6)BlueReturnCnt++;
		else if(Res==':'	&& BlueReturnCnt==7)BlueReturnCnt++;
		else if(Res=='2'	&& BlueReturnCnt==8)BlueReturnCnt++;
		else if(Res=='6'	&& BlueReturnCnt==9)BlueReturnCnt++;
		else if(Res=='9'	&& BlueReturnCnt==10)BlueReturnCnt++;
		else if(Res=='2'	&& BlueReturnCnt==11)BlueReturnCnt++;
		else if(Res==','	&& BlueReturnCnt==12)
		{
			BlueReturnCnt=0;
			InitBleFlag = 1;
		}
		else BlueReturnCnt=0;
		
		
		
		//offline ? local? cloud ? updating ? uap ? unprov
		if(Res=='u'	&& NETcnt==0)NETcnt4++;
		else if(Res=='a'	&& NETcnt4==1)NETcnt4++;
		else if(Res=='p'	&& NETcnt4==2){
			NETcnt4=0;
			module_net_state = net_status_uap;
		}
		else NETcnt4=0;
		
		if(Res=='c'	&& NETcnt==0)NETcnt++;
		else if(Res=='l'	&& NETcnt==1)NETcnt++;
		else if(Res=='o'	&& NETcnt==2)NETcnt++;
		else if(Res=='u'	&& NETcnt==3)NETcnt++;
		else if(Res=='d'	&& NETcnt==4){
			NETcnt=0;
			module_net_state = net_status_cloud;
		}
		else NETcnt=0;
		
		if(Res=='l'	&& NETcnt2==0)NETcnt2++;
		else if(Res=='o'	&& NETcnt2==1)NETcnt2++;
		else if(Res=='c'	&& NETcnt2==2)NETcnt2++;
		else if(Res=='a'	&& NETcnt2==3)NETcnt2++;
		else if(Res=='l'	&& NETcnt2==4){
			NETcnt2=0;
			module_net_state = net_status_local;
		}
		else NETcnt2=0;
		
		if(Res=='o'	&& NETcnt3==0)NETcnt3++;
		else if(Res=='f'	&& NETcnt3==1)NETcnt3++;
		else if(Res=='f'	&& NETcnt3==2)NETcnt3++;
		else if(Res=='l'	&& NETcnt3==3)NETcnt3++;
		else if(Res=='i'	&& NETcnt3==4)NETcnt3++;
		else if(Res=='n'	&& NETcnt3==5)NETcnt3++;
		else if(Res=='e'	&& NETcnt3==6)
		{
			NETcnt3=0;
			module_net_state = net_status_offline;
		}
		else NETcnt3=0;
		
		//unprov
		if(Res=='u'	&& NETcnt5==0)NETcnt5++;
		else if(Res=='n'	&& NETcnt5==1)NETcnt5++;
		else if(Res=='p'	&& NETcnt5==2)NETcnt5++;
		else if(Res=='r'	&& NETcnt5==3)NETcnt5++;
		else if(Res=='o'	&& NETcnt5==4)NETcnt5++;
		else if(Res=='v'	&& NETcnt5==5)
		{
			NETcnt5=0;
			module_net_state = net_status_unprov;
		}
		else NETcnt5=0;
		
		
	} 
//	else if(USART_GetITStatus(USART1,USART_IT_IDLE) == SET)//�����ж�
//	{
//		USART1->DR; //���USART_IT_IDLE��־
//		//�����ݴ��뻺��
//	  BufferManageWrite(&buff_manage_struct1,Usart1ReadBuff,Usart1ReadCnt,&value);
//		Usart1ReadCnt=0;
//  }  
	
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
  {
    if(Usart1SendDataCnt>0)//�����������ݸ�������0
    {
			USART_SendData(USART1, *Usart1SendData);//??
			Usart1SendData++;
		  Usart1SendDataCnt--;
    }
    else
    {
      //�����ֽڽ���
			Usart1SendDataCnt = 0;
			
      USART_ClearITPendingBit(USART1,USART_IT_TXE);
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
      USART_ITConfig(USART1, USART_IT_TC, ENABLE);
    }
  }
	//�������
  if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
  {
    USART_ClearITPendingBit(USART1,USART_IT_TC);
    USART_ITConfig(USART1, USART_IT_TC, DISABLE);
  }
	
} 

/**
* @brief  ʹ��microLib�ķ���,ʹ��printf
* @warningg ��ѡ microLib
* @param  None
* @param  None
* @param  None
* @retval None
* @example 
**/

int fputc(int ch, FILE *f)
{
	static char flage=0;
	uint8_t data = ch;
	if(!flage)
	{
	  flage=1;
		USART_ClearITPendingBit(USART1,USART_IT_TC);//���printf��ʧ��һ���ֽڵ�����
	}
	USART_SendData(USART1, (uint8_t) ch);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
  return ch;
}
