/**
  ******************************************************************************
  * @author  yang feng wu 
  * @version V1.0.0
  * @date    2020/1/28
  * @brief   
  ******************************************************************************
	使用说明:https://www.cnblogs.com/yangfengwu/p/12228402.html
  ******************************************************************************
  */

#define BUFFMANAGE_C_
#include "BufferManage.h"
#include "LoopList.h"
#include <stdio.h>
#include "stm32f10x.h"


buff_manage_struct buff_manage_struct_t;//发送数据缓存管理
//buff_manage_struct buff_manage_struct_t1;//串口接收数据缓存管理
buff_manage_struct buff_manage_struct_t2;//串口接收数据缓存管理

/**
* @brief   创建数据缓存管理
* @param   bms			     缓存管理结构体变量
* @param   buff          用于缓存数据的数组
* @param   BuffLen       用于缓存数据的数组的长度
* @param   ManageBuff    用于记录每次缓存多少字节的数组
* @param   ManageBuffLen 用于记录每次缓存多少字节的数组长度
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
* @brief   写入缓存数据
* @param   bms			缓存管理结构体变量
* @param   buff     写入的数据
* @param   BuffLen  写入的数据个数
* @retval  0 Success
* @warning None
* @example 
**/
int value1;
int32_t BufferManageWrite(buff_manage_struct *bms,void *buff,uint32_t BuffLen)
{
	__disable_irq();
	if(rbCanWrite(&(bms->Buff))>BuffLen)//可以写入数据
	{
		if(rbCanWrite(&(bms->ManageBuff))>4)//可以记录数据个数
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
* @brief   从缓存中读取数据
* @param   bms			缓存管理结构体变量
* @param   buff     返回的数据地址
* @param   None
* @retval  取出的数据个数
* @warning None
* @example 
**/
int32_t BufferManageRead(buff_manage_struct *bms,void *buff)
{
	int32_t value=0;
	__disable_irq();
	if(rbCanRead(&(bms->ManageBuff))>=4)
	{
		rbRead(&(bms->ManageBuff), &(bms->Len) , 4);//读出来存入的数据个数
		if(bms->Len>0)
		{
			value = rbRead(&(bms->Buff),buff, bms->Len); 
		}
	}
	__enable_irq();
	return value;
}








