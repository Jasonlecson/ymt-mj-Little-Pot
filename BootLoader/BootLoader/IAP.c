/**
  ******************************************************************************
  * @file    IAP.c
  * @author  feng wu yang
  * @version V1.0.0
  * @date    2019/10/12
  * @brief   IAP升级
  ******************************************************************************
	一,把以下程序放入 1Ms累加定时器
	//正在接收程序时,下载超时开始累加
	if(IAPStructValue.PutDataFlage && IAPStructValue.UpdateFlage)IAPStructValue.DownloadTimeout++;
	else IAPStructValue.DownloadTimeout=0;
	
	IAPStructValue.MainTimeout++;
	
	
	
	首先需要明确:整体上就是把数据写入环形队列,然后从环形队列里面读出数据,然后写入Flash
	所以:无论用什么方式,只有一个目的,把数据写入环形队列

	写入数据的地方这样写.  
	if(IAPStructValue.PutDataFlage && (IAPStructValue.PutDataFlage^IAPStructValue.Overflow))//可以往环形队列里面写数据,同时没有溢出
	{
		if(PutData(&rb_tIAP,NULL,&Res,1) == -1) //&Res :为数据地址  1:写一个数据,可以写多个
		{
			IAPStructValue.Overflow = 1;//环形队列溢出
		}
	}
	
	
	在判定接收完数据的地方写上
	if(IAPStructValue.PutDataFlage)//写入环形队列的标志位置位了
	{
		IAPStructValue.ReadDataEndFlage=1;//接收完了程序
	}
	
  ******************************************************************************
  */
#define IAP_C_
#include <stdio.h>
#include "stmflash.h"
#include "iap.h"
#include <string.h>
#include "LoopList.h"
#include "cString.h"


IAPStruct IAPStructValue;

iapfun jump2app; 



/*复位芯片*/
void IAPResetMCU(void)
{
	__disable_fault_irq();   
	NVIC_SystemReset();//重启
}



void IAPInit(void)
{
	//清零所有变量;获取存储的云端和设备版本;获取更新状态标志
	memset(IAPStructValue.Buff,0,sizeof(IAPStructValue.Buff));//接收程序数据缓存数组
	memset(IAPStructValue.ReadDat,0,sizeof(IAPStructValue.ReadDat));//从环形队列读取数据使用
	
	
	IAPStructValue.PutDataFlage=0;     //控制是不是可以向环形队列写入数据
	IAPStructValue.Overflow=0;         //环形队列是否溢出
	IAPStructValue.ReadDataEndFlage=0; //确实接收完了程序
	IAPStructValue.UpdateFlage=0;      //更新标志
	IAPStructValue.FlashWriteErrFlage=0;//写入Flash出错标志
	IAPStructValue.UpdateStatusValue=0;//获取更新的状态
	IAPStructValue.ReadDate=0;         //从环形队列读取数据,组合成16位数据写入Flash
	IAPStructValue.BackupAPPAddress=0; //备份程序的地址
	IAPStructValue.RunAPPAddressCnt=0; //用于写Flash的时候地址累加    
	IAPStructValue.RunAPPAddress=0;    //运行的程序地址
	IAPStructValue.DownloadTimeout=0;  //程序下载超时
	IAPStructValue.MainTimeout=0;      //整体运行超时累加变量
	IAPStructValue.DataCheckErrFlage = 0;
	
	IAPStructValue.BackupAPPAddress = FLASH_APP2_ADDR;//备份程序的地址
	IAPStructValue.RunAPPAddress = FLASH_APP1_ADDR;//运行的地址
	
	rbCreate(&rb_tIAP,IAPStructValue.Buff,UpdateReceiveBuffLen);//创建环形队列管理数组
	
}

/**
* @brief  初始化和处理更新状态
* @warn   
* @param  
* @param  None
* @param  None
* @retval None
* @example
**/
void IAP(void)
{
	/*************获取更新标志位************/
	IAPStructValue.UpdateFlage = IAPGetUpdateFlage();//获取更新标志位
	IAPClearUpdateFlage();//清除更新标志位
	
	if(IAPStructValue.UpdateFlage==1){//有更新标志,备份源程序
		
		if(IAPCheckRamFlashAddress(IAPStructValue.RunAPPAddress))//有程序
		{
//			printf("\r\nRunAPP >>> BackupAPP\r\n");
			IAPStructValue.UserDataSize = FLASH_USER_SIZE*1024/2;//APP用户程序大小(按照16字节)
			IAPStructValue.RunAPPAddressCnt = IAPStructValue.RunAPPAddress;//运行的地址
			IAPStructValue.BackupAPPAddressCnt = IAPStructValue.BackupAPPAddress;//备份的地址
			
			if(FlashErasePage(IAPStructValue.BackupAPPAddress,FLASH_USER_SIZE)!=4)//擦除备份程序的地址
			{
				IAPSetUpdateStatus(UpdateStatus_EraseErrBack);//擦除备份区失败
				printf("\r\nErr:FlashEraseErr\r\n");
				IAPResetMCU();//重启
			}
			/*拷贝数据*/
			for(IAPStructValue.BackupCnt=0;IAPStructValue.BackupCnt<IAPStructValue.UserDataSize;IAPStructValue.BackupCnt++){
				
				STMFLASH_Read(IAPStructValue.RunAPPAddressCnt,&IAPStructValue.ReadDate,1);//读取源地址数据
				
				if(WriteFlashHalfWord(IAPStructValue.BackupAPPAddressCnt,IAPStructValue.ReadDate) == 0){//写入缓存
					IAPStructValue.RunAPPAddressCnt = IAPStructValue.RunAPPAddressCnt+2;
					IAPStructValue.BackupAPPAddressCnt = IAPStructValue.BackupAPPAddressCnt+2;
				}
				else{//写入失败,再尝试写一次
					if(WriteFlashHalfWord(IAPStructValue.BackupAPPAddressCnt,IAPStructValue.ReadDate) == 0){//写入缓存
						IAPStructValue.RunAPPAddressCnt = IAPStructValue.RunAPPAddressCnt+2;
						IAPStructValue.BackupAPPAddressCnt = IAPStructValue.BackupAPPAddressCnt+2;
					}
					else{//写入失败,再尝试写一次
						if(WriteFlashHalfWord(IAPStructValue.BackupAPPAddressCnt,IAPStructValue.ReadDate) == 0){//写入缓存
							IAPStructValue.RunAPPAddressCnt = IAPStructValue.RunAPPAddressCnt+2;
							IAPStructValue.BackupAPPAddressCnt = IAPStructValue.BackupAPPAddressCnt+2;
						}
						else{
							IAPSetUpdateStatus(UpdateStatus_BackupErr);//运行区的程序拷贝到备份区失败
							printf("\r\nErr:FlashBackupErr\r\n");
							IAPResetMCU();//重启
						}
					}
				}
			}
		}
		
		IAPSetUpdateStatus(UpdateStatus_EraseErrRunUpdata);//擦除运行区失败(更新时)
		if(FlashErasePage(IAPStructValue.RunAPPAddress,FLASH_USER_SIZE)!=4)//擦除运行程序地址
		{
			//Flash 擦除失败
			IAPSetUpdateStatus(UpdateStatus_EraseErrRunUpdata);//擦除运行区失败(更新时)
			printf("\r\nErr:FlashEraseErrRun\r\n");
			IAPResetMCU();//重启
		}
	}
	else
	{
		IAPStructValue.UpdateStatusValue = IAPGetUpdateStatus();//获取更新状态
		
		if( 
			IAPStructValue.UpdateStatusValue == UpdateStatus_EraseErrRunUpdata || //擦除运行区失败(更新时)
			IAPStructValue.UpdateStatusValue == UpdateStatus_EraseErrRunBack || //擦除运行区失败(备份区拷贝到运行区时)
			IAPStructValue.UpdateStatusValue == UpdateStatus_BackupToRunErr || //备份区的程序拷贝运行区错误
			IAPStructValue.UpdateStatusValue == UpdateStatus_DownloadTimeout || //程序下载超时
			IAPStructValue.UpdateStatusValue == UpdateStatus_MainTimeout ||
			IAPStructValue.UpdateStatusValue == UpdateStatus_FlashWriteErr || //Flash 写错误
			IAPStructValue.UpdateStatusValue == UpdateStatus_DataOverflow || //缓存接收溢出
			IAPStructValue.UpdateStatusValue == UpdateStatus_MissingData || //数据接收不完整
			IAPStructValue.UpdateStatusValue == UpdateStatus_CheckErr || //校验错误
			IAPStructValue.UpdateStatusValue == UpdateStatus_VersionLenErr || //版本号错误
			IAPStructValue.UpdateStatusValue == UpdateStatus_DataAddressError || //Flash数据的高位地址不是0x08 或者 RAM的高位地址不是0x20
			IAPStructValue.UpdateStatusValue == UpdateStatus_SetRollBack ||//手动控制程序回滚
			IAPStructValue.UpdateStatusValue == UpdateStatus_WriteAppOkFlage
			)
		{
			if(IAPCheckRamFlashAddress(IAPStructValue.BackupAPPAddress))//有备份的程序
			{
//				printf("\r\nBackupAPP >>> RunAPP\r\n");
				//把备份的程序拷贝到运行地址
				IAPStructValue.UserDataSize = FLASH_USER_SIZE*1024/2;//APP用户程序大小(按照16字节)
				IAPStructValue.RunAPPAddressCnt = IAPStructValue.RunAPPAddress;//运行的地址
				IAPStructValue.BackupAPPAddressCnt = IAPStructValue.BackupAPPAddress;//备份的地址
				
				/**如果拷贝不成功则一直执行拷贝,因为确实没有办法**/
				if(FlashErasePage(IAPStructValue.RunAPPAddress,FLASH_USER_SIZE)!=4)//擦除源程序的地址
				{
					IAPSetUpdateStatus(UpdateStatus_EraseErrRunUpdata);//擦除运行区失败(备份区拷贝到运行区时)
					printf("\r\nErr:FlashEraseErr\r\n");
					IAPResetMCU();//重启
				}
				
				for(IAPStructValue.BackupCnt=0;IAPStructValue.BackupCnt<IAPStructValue.UserDataSize;IAPStructValue.BackupCnt++)
				{
					STMFLASH_Read(IAPStructValue.BackupAPPAddressCnt,&IAPStructValue.ReadDate,1);//读取备份的地址
					
					if(WriteFlashHalfWord(IAPStructValue.RunAPPAddressCnt,IAPStructValue.ReadDate) == 0){//写入缓存
						IAPStructValue.RunAPPAddressCnt = IAPStructValue.RunAPPAddressCnt+2;
						IAPStructValue.BackupAPPAddressCnt = IAPStructValue.BackupAPPAddressCnt+2;
					}
					else{//写入失败,再尝试写一次
						if(WriteFlashHalfWord(IAPStructValue.RunAPPAddressCnt,IAPStructValue.ReadDate) == 0){//写入缓存
							IAPStructValue.RunAPPAddressCnt = IAPStructValue.RunAPPAddressCnt+2;
							IAPStructValue.BackupAPPAddressCnt = IAPStructValue.BackupAPPAddressCnt+2;
						}
						else{//写入失败,再尝试写一次
							if(WriteFlashHalfWord(IAPStructValue.RunAPPAddressCnt,IAPStructValue.ReadDate) == 0){//写入缓存
								IAPStructValue.RunAPPAddressCnt = IAPStructValue.RunAPPAddressCnt+2;
								IAPStructValue.BackupAPPAddressCnt = IAPStructValue.BackupAPPAddressCnt+2;
							}
							else{
								IAPSetUpdateStatus(UpdateStatus_BackupToRunErr);//备份区的程序拷贝运行区错误
								printf("\r\nErr:FlashBackupErr\r\n");
								IAPResetMCU();//重启
							}
						}
					}
				}
				IAPSetUpdateStatus(UpdateStatus_BackupToRunOK);//备份区的程序拷贝到运行区成功
			}
			else
			{
				IAPSetUpdateStatus(UpdateStatus_NoBackup);//没有备份的程序
			}
		}
		else if(IAPStructValue.UpdateStatusValue == UpdateStatus_WriteAppOk)//上次是升级进去程序
		{
			IAPSetUpdateStatus(UpdateStatus_WriteAppOkFlage);//写入0xFF
		}
	}
	
	
	
	//打印更新的Flash配置信息
//	printf("\r\nuser1ROMStart: 0x%x\r\n",FLASH_APP1_ADDR);
//	printf("user1ROMSize : 0x%x\r\n",FLASH_USER_SIZE*1024);
//	printf("user2ROMStart: 0x%x\r\n",FLASH_APP2_ADDR);
//	printf("user2ROMSize : 0x%x\r\n",FLASH_USER_SIZE*1024);
//	printf("UpdateStatusValue=%d\r\n",IAPStructValue.UpdateStatusValue);//更新的状态变量
}


/**
* @brief  程序下载超时,超时重启;
* @warn   
* @param  
* @param  None
* @param  None
* @retval None
* @example
**/
void IAPDownloadTimeoutFunction(void)
{
	if(IAPStructValue.DownloadTimeout>UpdateDownloadTimeoutCompare) {
		IAPStructValue.DownloadTimeout=0;//程序下载超时:清零
		IAPSetUpdateStatus(UpdateStatus_DownloadTimeout);
		IAPResetMCU();//重启
	}
}

/**
* @brief  重置程序下载超时
* @warn   
* @param  
* @param  None
* @param  None
* @retval None
* @example
**/
void IAPResetDownloadTimeoutFunction(void)
{
	IAPStructValue.DownloadTimeout = 0;
}


/**
* @brief  整体运行超时,超时重启;
* @warn   
* @param  
* @param  None
* @param  None
* @retval None
* @example
**/
void IAPMainTimeoutFunction(void)
{
	if(IAPStructValue.MainTimeout>MainTimeoutCompare) {
		IAPStructValue.MainTimeout=0;//清零
		IAPSetUpdateStatus(UpdateStatus_MainTimeout);
		IAPResetMCU();//重启
	}
}

/**
* @brief  重置整体运行超时
* @warn   
* @param  
* @param  None
* @param  None
* @retval None
* @example
**/
void IAPResetIAPMainTimeoutFunction(void)
{
	IAPStructValue.MainTimeout = 0;
}


/**
* @brief  没有更新的状态下加载用户程序
* @warn   
* @param  
* @param  None
* @param  None
* @retval None
* @example
**/
void IAPLoadAPPProgram(void)
{
	//没有更新标志                 且     没有操作升级
	if(!IAPStructValue.UpdateFlage && !IAPStructValue.PutDataFlage)
	{
		if(IAPCheckRamFlashAddress(IAPStructValue.RunAPPAddress))//检测某些位置的Flash的高位地址是不是0x08,RAM的高位地址是不是0x20	
		{
			SysTick->CTRL=0;//?????

			USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);//????????
			USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);//????????

			USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);//??????
			USART_ITConfig(USART2, USART_IT_IDLE, DISABLE);//??????

			USART_Cmd(USART1, DISABLE);                    
			USART_Cmd(USART2, DISABLE);        
			
			iap_load_app(IAPStructValue.RunAPPAddress);//运行用户程序
		}
	}
}




//检测数据的某些位置的Flash的高位地址是不是0x08		//RAM的高位地址是不是0x20	
//1: 正确  0:不正确
char IAPCheckRamFlashAddress(u32 Address)
{
	if(((*(vu32*)(Address+4))&0xFF000000)==0x08000000)//Flash的高位地址是不是0x08						
	{
		if(((*(vu32*)Address)&0x2FFE0000)==0x20000000)//RAM的高位地址是不是0x20		
		{
			return 1;
		}
	}
	return 0;
}

//设置运行哪一份程序  1:运行第一份程序  2:运行第二份程序
void IAPSetUpdateRunProgram(u16 ReadDat)
{
	STMFLASH_Write(FLASH_RUNUSER_ADDR,&ReadDat,1);
}

//获取正在运行的哪一份程序  0:没有运行的程序  1:运行第一份程序  2:运行第二份程序
u16 IAPGetUpdateRunProgram(void)
{
	u16 ReadDat = 0;
	
  STMFLASH_Read(FLASH_RUNUSER_ADDR,&ReadDat,1);
	
	return ReadDat == 65535 ? 0 : ReadDat;
}

//切换运行程序地址
void IAPSetUpdateChangeProgram(void)
{
	if(IAPGetUpdateRunProgram() == 1) IAPSetUpdateRunProgram(2);
	else IAPSetUpdateRunProgram(1);
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

