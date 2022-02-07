#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include <stm32f10x.h>


//用户根据自己的需要设置
#define STM32_FLASH_SIZE 128 	 		//所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN 1        //使能FLASH写入(0，不是能;1，使能)
#define FLASH_IAP_SIZE 20         //BootLoader所用程序大小(KB)
#define FLASH_DATA_SIZE 6         //存储用户数据所用程序大小(KB)
//用户程序大小 = (总FLASH容量 - 存储用户数据所用程序大小 - IAP所用程序大小)/2
#define FLASH_USER_SIZE  ((STM32_FLASH_SIZE - FLASH_DATA_SIZE - FLASH_IAP_SIZE)/2)  //用户程序大小(KB)

#define STM32_FLASH_BASE 0x8000000 	//STM32 FLASH的起始地址

//用户程序1地址
#define FLASH_APP1_ADDR  (STM32_FLASH_BASE+1024*FLASH_IAP_SIZE)
//用户程序2地址
#define FLASH_APP2_ADDR  (STM32_FLASH_BASE+1024*(FLASH_USER_SIZE+FLASH_IAP_SIZE))
//存储用户数据的地址 :Flash最后的 FLASH_DATA_SIZE KB  作为数据存储的地址 
#define FLASH_DATA_ADDR  (STM32_FLASH_BASE + 1024*(STM32_FLASH_SIZE - FLASH_DATA_SIZE) + 10) 


#define FLASH_RUNUSER_ADDR (FLASH_DATA_ADDR+0)       //存储运行的哪一份程序信息 0:没有可以运行的程序;1:运行的第一份程序;2:运行的第二份程序
#define FLASH_UPDATE_FLAGE_ADDR (FLASH_DATA_ADDR+2)  //存储是不是需要更新  1:更新  其它:不需要更新
#define FLASH_UPDATE_STATUS_ADDR (FLASH_DATA_ADDR+4) //存储更新的状态
#define FLASH_UPDATE_VERSION1_ADDR (FLASH_DATA_ADDR+6) //存储云端发来的版本号,版本号最长20个字符
#define FLASH_UPDATE_VERSION2_ADDR (FLASH_DATA_ADDR+26) //存储设备当前的版本号,版本号最长20个字符
//后期的存储请从  FLASH_DATA_ADDR+46  开始

u16 STMFLASH_ReadHalfWord(u32 faddr);		  //读出半字  
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//指定地址开始写入指定长度的数据
u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);						//指定地址开始读取指定长度数据
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//从指定地址开始读出指定长度的数据

char WriteFlashHalfWord(uint32_t WriteAddress,u16 data);
char FlashErasePage(uint32_t EraseAddress,u16 PageCnt);	
#endif

















