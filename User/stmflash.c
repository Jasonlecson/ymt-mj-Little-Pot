#include "stmflash.h"

FLASH_Status FlashStatus;


//读取指定地址的半字(16位数据)
//faddr:读地址(此地址必须为2的倍数!!)
//返回值:对应数据.
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}

#if STM32_FLASH_WREN	//如果使能了写   
//不检查的写入
//WriteAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数   
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	for(i=0;i<NumToWrite;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
		WriteAddr+=2;//地址增加2.
	}  
} 
//从指定地址开始写入指定长度的数据
//WriteAddr:起始地址(此地址必须为2的倍数!!)
//pBuffer:数据指针
//NumToWrite:半字(16位)数(就是要写入的16位数据的个数.)
#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //字节
#else 
#define STM_SECTOR_SIZE	2048
#endif		 
u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//最多是2K字节
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //扇区地址
	u16 secoff;	   //扇区内偏移地址(16位字计算)
	u16 secremain; //扇区内剩余地址(16位字计算)	   
 	u16 i;    
	u32 offaddr;   //去掉0X08000000后的地址
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return;//非法地址
	FLASH_Unlock();						//解锁
	offaddr=WriteAddr-STM32_FLASH_BASE;		//实际偏移地址.
	secpos=offaddr/STM_SECTOR_SIZE;			//扇区地址  0~127 for STM32F103RBT6
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//在扇区内的偏移(2个字节为基本单位.)
	secremain=STM_SECTOR_SIZE/2-secoff;		//扇区剩余空间大小   
	if(NumToWrite<=secremain)secremain=NumToWrite;//不大于该扇区范围
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//擦除这个扇区
			
			for(i=0;i<STM_SECTOR_SIZE;i+=2)//再次判断是否有擦除失败
			{
			  if(STMFLASH_ReadHalfWord(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE+i)!=0XFFFF)
				{
				  FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//擦除这个扇区
					break;
				}
			}
			
			for(i=0;i<secremain;i++)//复制
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//写入整个扇区  
		}
		else 
		{
			 STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//写已经擦除了的,直接写入扇区剩余区间. 
		}
				
		if(NumToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;				//扇区地址增1
			secoff=0;				//偏移位置为0 	 
			pBuffer+=secremain;  	//指针偏移
			WriteAddr+=secremain;	//写地址偏移	   
			NumToWrite-=secremain;	//字节(16位)数递减
			if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//下一个扇区还是写不完
			else secremain=NumToWrite;//下一个扇区可以写完了
		}	 
	};	
	FLASH_Lock();//上锁
}
#endif




void WriteFlashHalfWords(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	
	FlashStatus = FLASH_BUSY;//设置为忙
	if(FLASH_GetStatus() == FLASH_COMPLETE)//可以操作Flash
	{
		FLASH_Unlock();		
		for(i=0;i<NumToWrite;i++)
		{
			FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
			WriteAddr+=2;//地址增加2.
		}  
		FLASH_Lock();//上锁
	}
} 


/**
* @brief  指定地址写入一个16位数据
* @waing  写入的地址请先擦除
* @param  WriteAddress  写入的地址
* @param  data          写入的数据
* @param  None
* @retval 0:成功
* @example 
**/
char WriteFlashHalfWord(uint32_t WriteAddress,u16 data)
{
	FlashStatus = FLASH_BUSY;//设置为忙
	if(FLASH_GetStatus() == FLASH_COMPLETE)//可以操作Flash
	{
		FLASH_Unlock();		
		FlashStatus = FLASH_ProgramHalfWord(WriteAddress,data);//写入数据
		
		if(FlashStatus == FLASH_COMPLETE)//操作完成
		{
			if(STMFLASH_ReadHalfWord(WriteAddress) == data)//读出的和写入的一致
				FlashStatus = 0;//读出和写入的一致
			else
				FlashStatus = 5;
		}
		FLASH_Lock();//上锁
	}
	return FlashStatus;
} 


//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr+=2;//偏移2个字节.	
	}
}


/**
* @brief  擦除Flash
* @waing  
* @param  EraseAddress  擦除的地址
* @param  PageCnt       擦除连续的几页
* @param  None
* @retval 4:成功
* @example 
**/
char FlashErasePage(uint32_t EraseAddress,u16 PageCnt)
{
	u32 i=0;
	u32 secpos;	   //扇区地址
	if(EraseAddress<STM32_FLASH_BASE||(EraseAddress>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return 0;//非法地址
	secpos = EraseAddress-STM32_FLASH_BASE;//实际地址
	secpos = secpos/STM_SECTOR_SIZE;//扇区地址 
	if(STM_SECTOR_SIZE==2048){PageCnt=PageCnt/2;}
	if(FLASH_GetStatus() == FLASH_COMPLETE)//可以操作Flash
	{
		FLASH_Unlock();
		for(i=0;i<PageCnt;i++)
		{
			FlashStatus = FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//擦除这个扇区
			if(FlashStatus != FLASH_COMPLETE) break;
			secpos++;
		}
		FLASH_Lock();//上锁
	}	
	return FlashStatus;
} 

