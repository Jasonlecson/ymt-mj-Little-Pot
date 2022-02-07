#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include <stm32f10x.h>


//�û������Լ�����Ҫ����
#define STM32_FLASH_SIZE 128 	 		//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 1        //ʹ��FLASHд��(0��������;1��ʹ��)
#define FLASH_IAP_SIZE 20         //BootLoader���ó����С(KB)
#define FLASH_DATA_SIZE 6         //�洢�û��������ó����С(KB)
//�û������С = (��FLASH���� - �洢�û��������ó����С - IAP���ó����С)/2
#define FLASH_USER_SIZE  ((STM32_FLASH_SIZE - FLASH_DATA_SIZE - FLASH_IAP_SIZE)/2)  //�û������С(KB)

#define STM32_FLASH_BASE 0x8000000 	//STM32 FLASH����ʼ��ַ

//�û�����1��ַ
#define FLASH_APP1_ADDR  (STM32_FLASH_BASE+1024*FLASH_IAP_SIZE)
//�û�����2��ַ
#define FLASH_APP2_ADDR  (STM32_FLASH_BASE+1024*(FLASH_USER_SIZE+FLASH_IAP_SIZE))
//�洢�û����ݵĵ�ַ :Flash���� FLASH_DATA_SIZE KB  ��Ϊ���ݴ洢�ĵ�ַ 
#define FLASH_DATA_ADDR  (STM32_FLASH_BASE + 1024*(STM32_FLASH_SIZE - FLASH_DATA_SIZE) + 10) 


#define FLASH_RUNUSER_ADDR (FLASH_DATA_ADDR+0)       //�洢���е���һ�ݳ�����Ϣ 0:û�п������еĳ���;1:���еĵ�һ�ݳ���;2:���еĵڶ��ݳ���
#define FLASH_UPDATE_FLAGE_ADDR (FLASH_DATA_ADDR+2)  //�洢�ǲ�����Ҫ����  1:����  ����:����Ҫ����
#define FLASH_UPDATE_STATUS_ADDR (FLASH_DATA_ADDR+4) //�洢���µ�״̬
#define FLASH_UPDATE_VERSION1_ADDR (FLASH_DATA_ADDR+6) //�洢�ƶ˷����İ汾��,�汾���20���ַ�
#define FLASH_UPDATE_VERSION2_ADDR (FLASH_DATA_ADDR+26) //�洢�豸��ǰ�İ汾��,�汾���20���ַ�
//���ڵĴ洢���  FLASH_DATA_ADDR+46  ��ʼ

u16 STMFLASH_ReadHalfWord(u32 faddr);		  //��������  
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//ָ����ַ��ʼд��ָ�����ȵ�����
u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);						//ָ����ַ��ʼ��ȡָ����������
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����

char WriteFlashHalfWord(uint32_t WriteAddress,u16 data);
char FlashErasePage(uint32_t EraseAddress,u16 PageCnt);	
#endif

















