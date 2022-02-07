/**
  ******************************************************************************
  * @file    usart.c
  * @author  fengwu yang
  * @version V1.0.0
  * @date    2019/10/12
  * @brief   IAP升级
  ******************************************************************************
  ******************************************************************************
  */
#define IAP_C_
#include <stdio.h>
#include "stmflash.h"
#include "IAP.h"
#include <string.h>

IAPStruct IAPStructValue;

iapfun jump2app; 




/**
* @brief  获取更新的信息
* @param  
* @param  None
* @param  None
* @retval None
* @example 
**/
void IAPGetUpdateInfo(void)
{
//	IAPGetUpdateVersionServer(IAPStructValue.VersionServer);//获取云端版本 
//	IAPGetUpdateVersionDevice(IAPStructValue.VersionDevice);//获取设备版本
	IAPStructValue.UpdateStatusValue = IAPGetUpdateStatus();//获取更新状态
}


/**
* @brief  处理更新
* @param  
* @param  None
* @param  None
* @retval None
* @example 
**/
void IAPUpdateDispose(void)
{
	//处理更新
	if(IAPStructValue.UpdateStatusValue!=0)//升级过程序
	{
		IAPSetUpdateStatus(UpdateStatus_None);//清零升级状态
//		if(IAPStructValue.UpdateStatusValue == UpdateStatus_WriteAppOkFlage)//是运行的新程序
//		{
//			IAPGetUpdateVersionServer(IAPStructValue.VersionServer);//获取云端版本 
//			IAPSetUpdateVersionDevice(IAPStructValue.VersionServer);//切换程序版本
//			IAPGetUpdateVersionDevice(IAPStructValue.VersionDevice);//获取设备版本
//		}
		IAPStructValue.UpdateStatusValue=0;
	}			
	else
	{
		memset(IAPStructValue.UpdateStatusStr,0,sizeof(IAPStructValue.UpdateStatusStr));
		sprintf(IAPStructValue.UpdateStatusStr,"%s","NoUpdata");
	}
}


//设置更新标志
void IAPSetUpdateFlage(void)
{
	u16 ReadDat = 1;
  STMFLASH_Write(FLASH_UPDATE_FLAGE_ADDR,&ReadDat,1);
}

//清除更新标志
void IAPClearUpdateFlage(void)
{
	u16 ReadDat = 0;
  STMFLASH_Write(FLASH_UPDATE_FLAGE_ADDR,&ReadDat,1);
}

//获取更新标志
u16 IAPGetUpdateFlage(void)
{
	u16 ReadDat = 0;
  STMFLASH_Read(FLASH_UPDATE_FLAGE_ADDR,&ReadDat,1);
	return ReadDat == 65535 ? 0 : ReadDat;
}


//设置云端的版本
void IAPSetUpdateVersionServer(char* ch)
{
	u16 ReadDat[10];
	
	ReadDat[0] = ch[0]; ReadDat[0] = ReadDat[0]<<8|ch[1];
	ReadDat[1] = ch[2]; ReadDat[1] = ReadDat[1]<<8|ch[3];
	ReadDat[2] = ch[4]; ReadDat[2] = ReadDat[2]<<8|ch[5];
	ReadDat[3] = ch[6]; ReadDat[3] = ReadDat[3]<<8|ch[7];
	ReadDat[4] = ch[8]; ReadDat[4] = ReadDat[4]<<8|ch[9];
	ReadDat[5] = ch[10]; ReadDat[5] = ReadDat[5]<<8|ch[11];
	ReadDat[6] = ch[12]; ReadDat[6] = ReadDat[6]<<8|ch[13];
	ReadDat[7] = ch[14]; ReadDat[7] = ReadDat[7]<<8|ch[15];
	ReadDat[8] = ch[16]; ReadDat[8] = ReadDat[8]<<8|ch[17];
	ReadDat[9] = ch[18]; ReadDat[9] = ReadDat[9]<<8|ch[19];
	
	
  STMFLASH_Write(FLASH_UPDATE_VERSION1_ADDR,&ReadDat[0],10);
}

//获取云端的版本
void IAPGetUpdateVersionServer(char* ch)
{
	u16 ReadDat[10];
  STMFLASH_Read(FLASH_UPDATE_VERSION1_ADDR,ReadDat,10);
	
	if(ReadDat[0] == 65535) ReadDat[0]=0;
	if(ReadDat[1] == 65535) ReadDat[1]=0;
	if(ReadDat[2] == 65535) ReadDat[2]=0;
	if(ReadDat[3] == 65535) ReadDat[3]=0;
	if(ReadDat[4] == 65535) ReadDat[4]=0;
	if(ReadDat[5] == 65535) ReadDat[5]=0;
	if(ReadDat[6] == 65535) ReadDat[6]=0;
	if(ReadDat[7] == 65535) ReadDat[7]=0;
	if(ReadDat[8] == 65535) ReadDat[8]=0;
	if(ReadDat[9] == 65535) ReadDat[9]=0;
	
	ch[0]  = ReadDat[0]>>8;  ch[1] = ReadDat[0]&0x00ff;
	ch[2]  = ReadDat[1]>>8;  ch[3] = ReadDat[1]&0x00ff;
	ch[4]  = ReadDat[2]>>8;  ch[5] = ReadDat[2]&0x00ff;
	ch[6]  = ReadDat[3]>>8;  ch[7] = ReadDat[3]&0x00ff;
	ch[8]  = ReadDat[4]>>8;  ch[9] = ReadDat[4]&0x00ff;
	ch[10] = ReadDat[5]>>8;  ch[11] = ReadDat[5]&0x00ff;
	ch[12] = ReadDat[6]>>8;  ch[13] = ReadDat[6]&0x00ff;
	ch[14] = ReadDat[7]>>8;  ch[15] = ReadDat[7]&0x00ff;
	ch[16] = ReadDat[8]>>8;  ch[17] = ReadDat[8]&0x00ff;
	ch[18] = ReadDat[9]>>8;  ch[19] = ReadDat[9]&0x00ff;
}


//设置设备的版本
void IAPSetHardwareVersion(u16 data)
{
	u16 ReadDat = data;
  STMFLASH_Write(FLASH_UPDATE_VERSION2_ADDR,&ReadDat,1);
//	u16 ReadDat[10];
//	
//	ReadDat[0] = ch[0]; ReadDat[0] = ReadDat[0]<<8|ch[1];
//	ReadDat[1] = ch[2]; ReadDat[1] = ReadDat[1]<<8|ch[3];
//	ReadDat[2] = ch[4]; ReadDat[2] = ReadDat[2]<<8|ch[5];
//	ReadDat[3] = ch[6]; ReadDat[3] = ReadDat[3]<<8|ch[7];
//	ReadDat[4] = ch[8]; ReadDat[4] = ReadDat[4]<<8|ch[9];
//	ReadDat[5] = ch[10]; ReadDat[5] = ReadDat[5]<<8|ch[11];
//	ReadDat[6] = ch[12]; ReadDat[6] = ReadDat[6]<<8|ch[13];
//	ReadDat[7] = ch[14]; ReadDat[7] = ReadDat[7]<<8|ch[15];
//	ReadDat[8] = ch[16]; ReadDat[8] = ReadDat[8]<<8|ch[17];
//	ReadDat[9] = ch[18]; ReadDat[9] = ReadDat[9]<<8|ch[19];
//	
//  STMFLASH_Write(FLASH_UPDATE_VERSION2_ADDR,ReadDat,10);
}


//获取设备的版本
u16 IAPGetHardwareVersion(void)
{
	u16 ReadDat = 0;
  STMFLASH_Read(FLASH_UPDATE_VERSION2_ADDR,&ReadDat,1);
	return ReadDat == 65535 ? 0 : ReadDat;
//	u16 ReadDat[10];
//  STMFLASH_Read(FLASH_UPDATE_VERSION2_ADDR,ReadDat,10);
//	if(ReadDat[0] == 65535) ReadDat[0]=0;
//	if(ReadDat[1] == 65535) ReadDat[1]=0;
//	if(ReadDat[2] == 65535) ReadDat[2]=0;
//	if(ReadDat[3] == 65535) ReadDat[3]=0;
//	if(ReadDat[4] == 65535) ReadDat[4]=0;
//	if(ReadDat[5] == 65535) ReadDat[5]=0;
//	if(ReadDat[6] == 65535) ReadDat[6]=0;
//	if(ReadDat[7] == 65535) ReadDat[7]=0;
//	if(ReadDat[8] == 65535) ReadDat[8]=0;
//	if(ReadDat[9] == 65535) ReadDat[9]=0;
//	
//	ch[0]  = ReadDat[0]>>8;  ch[1] = ReadDat[0]&0x00ff;
//	ch[2]  = ReadDat[1]>>8;  ch[3] = ReadDat[1]&0x00ff;
//	ch[4]  = ReadDat[2]>>8;  ch[5] = ReadDat[2]&0x00ff;
//	ch[6]  = ReadDat[3]>>8;  ch[7] = ReadDat[3]&0x00ff;
//	ch[8]  = ReadDat[4]>>8;  ch[9] = ReadDat[4]&0x00ff;
//	ch[10] = ReadDat[5]>>8;  ch[11] = ReadDat[5]&0x00ff;
//	ch[12] = ReadDat[6]>>8;  ch[13] = ReadDat[6]&0x00ff;
//	ch[14] = ReadDat[7]>>8;  ch[15] = ReadDat[7]&0x00ff;
//	ch[16] = ReadDat[8]>>8;  ch[17] = ReadDat[8]&0x00ff;
//	ch[18] = ReadDat[9]>>8;  ch[19] = ReadDat[9]&0x00ff;
}

//设置设备更新状态
void IAPSetUpdateStatus(char Status)
{
	u16 ReadDat = Status;
  STMFLASH_Write(FLASH_UPDATE_STATUS_ADDR,&ReadDat,1);
}

//获取设备更新状态
u16 IAPGetUpdateStatus(void)
{
  u16 ReadDat = 0;
  STMFLASH_Read(FLASH_UPDATE_STATUS_ADDR,&ReadDat,1);
	return ReadDat == 65535 ? 0 : ReadDat;
}

//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

//跳转到应用程序段
//appxaddr:用户代码起始地址.
void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{ 
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//拷贝APP程序的复位中断函数地址
		MSR_MSP(*(vu32*)appxaddr);					//初始化APP堆栈指针,对APP程序的堆栈进行重构,就是说重新分配RAM
		jump2app();									//执行APP的复位中断函数,最终便会跳转到APP的main函数
	}
}		 

