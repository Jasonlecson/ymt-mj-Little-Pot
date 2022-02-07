/**
  ******************************************************************************
  * @author  yang feng wu 
  * @version V1.0.0
  * @date    2020/1/28
  * @brief   
  ******************************************************************************
	ʹ��˵��:https://www.cnblogs.com/yangfengwu/p/12228402.html
  ******************************************************************************
  */

#define BUFFMANAGE_C_
#include "BufferManage.h"
#include "LoopList.h"
#include <stdio.h>
#include "stm32f10x.h"


buff_manage_struct buff_manage_struct_t;//�������ݻ������
//buff_manage_struct buff_manage_struct_t1;//���ڽ������ݻ������
buff_manage_struct buff_manage_struct_t2;//���ڽ������ݻ������

/**
* @brief   �������ݻ������
* @param   bms			     ��������ṹ�����
* @param   buff          ���ڻ������ݵ�����
* @param   BuffLen       ���ڻ������ݵ�����ĳ���
* @param   ManageBuff    ���ڼ�¼ÿ�λ�������ֽڵ�����
* @param   ManageBuffLen ���ڼ�¼ÿ�λ�������ֽڵ����鳤��
* @retval  None
* @warning None
* @example 
**/
int32_t BufferManageCreate(buff_manage_struct *bms,void *buff,uint32_t BuffLen,void *ManageBuff,uint32_t ManageBuffLen)
{
	__disable_irq();
	rbCreate(&(bms->Buff),buff,BuffLen);
	rbCreate(&(bms->ManageBuff),ManageBuff,ManageBuffLen);
	
	bms->Count=0;
	bms->Cnt=0;
	bms->ReadFlage=0;
	bms->ReadLen=0;
	bms->SendFlage=0;
	bms->SendLen=0;
	__enable_irq();
	return 0;
}


/**
* @brief   д�뻺������
* @param   bms			��������ṹ�����
* @param   buff     д�������
* @param   BuffLen  д������ݸ���
* @retval  0 Success
* @warning None
* @example 
**/
int value1;
int32_t BufferManageWrite(buff_manage_struct *bms,void *buff,uint32_t BuffLen)
{
	__disable_irq();
	if(rbCanWrite(&(bms->Buff))>BuffLen)//����д������
	{
		if(rbCanWrite(&(bms->ManageBuff))>4)//���Լ�¼���ݸ���
		{			
			PutData(&(bms->Buff) ,buff, BuffLen);
			PutData(&(bms->ManageBuff) ,&BuffLen, 4);
			value1 = 0;
		}
		else{value1 = -2;}
	}
	else {value1 = -3;}
	__enable_irq();
	
	return value1;
}


/**
* @brief   �ӻ����ж�ȡ����
* @param   bms			��������ṹ�����
* @param   buff     ���ص����ݵ�ַ
* @param   None
* @retval  ȡ�������ݸ���
* @warning None
* @example 
**/
int32_t BufferManageRead(buff_manage_struct *bms,void *buff)
{
	int32_t value=0;
	__disable_irq();
	if(rbCanRead(&(bms->ManageBuff))>=4)
	{
		rbRead(&(bms->ManageBuff), &(bms->Len) , 4);//��������������ݸ���
		if(bms->Len>0)
		{
			value = rbRead(&(bms->Buff),buff, bms->Len); 
		}
	}
	__enable_irq();
	return value;
}







