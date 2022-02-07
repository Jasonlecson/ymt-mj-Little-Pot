/**
  ******************************************************************************
  * @file    usart.c
  * @author  fengwu yang
  * @version V1.0.0
  * @date    2019/10/12
  * @brief   串口配置函数
  ******************************************************************************
	一,使用说明
		1,把以下程序放在1ms定时器中断中
			
			if(Usart1ReadCnt!=0)//串口接收到数据
			{
				Usart1IdleCnt++;//变量累加(该变量在串口接收时清零)
				if(Usart1IdleCnt>=Usart1IdleTime)//串口 Usart1IdleTime Ms 时间没有接收到数据
				{
					Usart1IdleCnt = 0;
					Usart1ReadCntCopy = Usart1ReadCnt;
					Usart1ReadCnt = 0;
					Usart1ReadFlage = 1;
				}
			}
		
		2,主循环调用使用
			while(1)
			{
				if(Usart1ReadFlage)//串口接收到一条数据
				{
					Usart1ReadFlage=0;
					//处理数据
					UsartOutStr(Usart1ReadBuff,Usart1ReadCntCopy);//返回接收的数据
				}
			}
				
  ******************************************************************************
  */
#define USART_C_
#include "include.h"
#include "IAP.h"
#include "LoopList.h"


char Usart1ReadBuff[Usart1ReadLen]={0};  //接收数据缓存
u32  Usart1ReadCnt = 0;//串口1接收到的数据个数


unsigned char *Usart1SendData;//串口中断发送数据的地址
u32   Usart1SendDataCnt=0;//串口中断发送数据的个数


u8   Usart1BufferMemory[Usart1BufferMemoryLen];//接收数据缓存区
u8   Usart1BufferMemoryRead[Usart1BufferMemoryReadLen];//提取缓存区的数据
u32  Usart1BufferMemoryManage[Usart1BufferMemoryManageLen];//管理缓存区

u8   Usart1IdelCnt = 0;


char module_net_state = 0;
u8 NETcnt=0;
u8 NETcnt2=0;
u8 NETcnt3=0;
u8 NETcnt4=0;
u8 NETcnt5=0;
u8 BlueReturnCnt=0;

/**
* @brief   串口1初始化
* @param   bound  波特率
* @param   None
* @param   None
* @retval  None
* @warning None
* @example 
**/
void uart_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
	
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  
  //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	
  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
//	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//空闲中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 
}


/**
* @brief  串口中断发送数据
* @param  c:数据的首地址  cnt:发送的数据个数
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
* @brief  串口发送字符串数据
* @param  *c:发送的数据指针  cnt:数据个数
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
* @brief  串口发送一个字节
* @param  *c:发送的数据指针  cnt:数据个数
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


void USART1_IRQHandler(void)//串口1中断服务程序
{
	u8 Res;
	int value;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		
		Usart1ReadBuff[Usart1ReadCnt] = Res;	//接收的数据存入数组
		Usart1ReadCnt++;
		if(Usart1ReadCnt > Usart1ReadLen -10)//防止数组溢出
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
//	else if(USART_GetITStatus(USART1,USART_IT_IDLE) == SET)//空闲中断
//	{
//		USART1->DR; //清除USART_IT_IDLE标志
//		//把数据存入缓存
//	  BufferManageWrite(&buff_manage_struct1,Usart1ReadBuff,Usart1ReadCnt,&value);
//		Usart1ReadCnt=0;
//  }  
	
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
  {
    if(Usart1SendDataCnt>0)//如果里面的数据个数大于0
    {
			USART_SendData(USART1, *Usart1SendData);//??
			Usart1SendData++;
		  Usart1SendDataCnt--;
    }
    else
    {
      //发送字节结束
			Usart1SendDataCnt = 0;
			
      USART_ClearITPendingBit(USART1,USART_IT_TXE);
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
      USART_ITConfig(USART1, USART_IT_TC, ENABLE);
    }
  }
	//发送完成
  if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
  {
    USART_ClearITPendingBit(USART1,USART_IT_TC);
    USART_ITConfig(USART1, USART_IT_TC, DISABLE);
  }
	
} 

/**
* @brief  使用microLib的方法,使用printf
* @warningg 勾选 microLib
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
		USART_ClearITPendingBit(USART1,USART_IT_TC);//解决printf丢失第一个字节的问题
	}
	USART_SendData(USART1, (uint8_t) ch);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
  return ch;
}

