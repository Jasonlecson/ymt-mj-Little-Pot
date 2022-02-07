/**
  ******************************************************************************
  * @file    IAP.c
  * @author  feng wu yang
  * @version V1.0.0
  * @date    2019/10/12
  * @brief   IAP����
  ******************************************************************************
	һ,�����³������ 1Ms�ۼӶ�ʱ��
	//���ڽ��ճ���ʱ,���س�ʱ��ʼ�ۼ�
	if(IAPStructValue.PutDataFlage && IAPStructValue.UpdateFlage)IAPStructValue.DownloadTimeout++;
	else IAPStructValue.DownloadTimeout=0;
	
	IAPStructValue.MainTimeout++;
	
	
	
	������Ҫ��ȷ:�����Ͼ��ǰ�����д�뻷�ζ���,Ȼ��ӻ��ζ��������������,Ȼ��д��Flash
	����:������ʲô��ʽ,ֻ��һ��Ŀ��,������д�뻷�ζ���

	д�����ݵĵط�����д.  
	if(IAPStructValue.PutDataFlage && (IAPStructValue.PutDataFlage^IAPStructValue.Overflow))//���������ζ�������д����,ͬʱû�����
	{
		if(PutData(&rb_tIAP,NULL,&Res,1) == -1) //&Res :Ϊ���ݵ�ַ  1:дһ������,����д���
		{
			IAPStructValue.Overflow = 1;//���ζ������
		}
	}
	
	
	���ж����������ݵĵط�д��
	if(IAPStructValue.PutDataFlage)//д�뻷�ζ��еı�־λ��λ��
	{
		IAPStructValue.ReadDataEndFlage=1;//�������˳���
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



/*��λоƬ*/
void IAPResetMCU(void)
{
	__disable_fault_irq();   
	NVIC_SystemReset();//����
}



void IAPInit(void)
{
	//�������б���;��ȡ�洢���ƶ˺��豸�汾;��ȡ����״̬��־
	memset(IAPStructValue.Buff,0,sizeof(IAPStructValue.Buff));//���ճ������ݻ�������
	memset(IAPStructValue.ReadDat,0,sizeof(IAPStructValue.ReadDat));//�ӻ��ζ��ж�ȡ����ʹ��
	
	
	IAPStructValue.PutDataFlage=0;     //�����ǲ��ǿ������ζ���д������
	IAPStructValue.Overflow=0;         //���ζ����Ƿ����
	IAPStructValue.ReadDataEndFlage=0; //ȷʵ�������˳���
	IAPStructValue.UpdateFlage=0;      //���±�־
	IAPStructValue.FlashWriteErrFlage=0;//д��Flash�����־
	IAPStructValue.UpdateStatusValue=0;//��ȡ���µ�״̬
	IAPStructValue.ReadDate=0;         //�ӻ��ζ��ж�ȡ����,��ϳ�16λ����д��Flash
	IAPStructValue.BackupAPPAddress=0; //���ݳ���ĵ�ַ
	IAPStructValue.RunAPPAddressCnt=0; //����дFlash��ʱ���ַ�ۼ�    
	IAPStructValue.RunAPPAddress=0;    //���еĳ����ַ
	IAPStructValue.DownloadTimeout=0;  //�������س�ʱ
	IAPStructValue.MainTimeout=0;      //�������г�ʱ�ۼӱ���
	IAPStructValue.DataCheckErrFlage = 0;
	
	IAPStructValue.BackupAPPAddress = FLASH_APP2_ADDR;//���ݳ���ĵ�ַ
	IAPStructValue.RunAPPAddress = FLASH_APP1_ADDR;//���еĵ�ַ
	
	rbCreate(&rb_tIAP,IAPStructValue.Buff,UpdateReceiveBuffLen);//�������ζ��й�������
	
}

/**
* @brief  ��ʼ���ʹ������״̬
* @warn   
* @param  
* @param  None
* @param  None
* @retval None
* @example
**/
void IAP(void)
{
	/*************��ȡ���±�־λ************/
	IAPStructValue.UpdateFlage = IAPGetUpdateFlage();//��ȡ���±�־λ
	IAPClearUpdateFlage();//������±�־λ
	
	if(IAPStructValue.UpdateFlage==1){//�и��±�־,����Դ����
		
		if(IAPCheckRamFlashAddress(IAPStructValue.RunAPPAddress))//�г���
		{
//			printf("\r\nRunAPP >>> BackupAPP\r\n");
			IAPStructValue.UserDataSize = FLASH_USER_SIZE*1024/2;//APP�û������С(����16�ֽ�)
			IAPStructValue.RunAPPAddressCnt = IAPStructValue.RunAPPAddress;//���еĵ�ַ
			IAPStructValue.BackupAPPAddressCnt = IAPStructValue.BackupAPPAddress;//���ݵĵ�ַ
			
			if(FlashErasePage(IAPStructValue.BackupAPPAddress,FLASH_USER_SIZE)!=4)//�������ݳ���ĵ�ַ
			{
				IAPSetUpdateStatus(UpdateStatus_EraseErrBack);//����������ʧ��
				printf("\r\nErr:FlashEraseErr\r\n");
				IAPResetMCU();//����
			}
			/*��������*/
			for(IAPStructValue.BackupCnt=0;IAPStructValue.BackupCnt<IAPStructValue.UserDataSize;IAPStructValue.BackupCnt++){
				
				STMFLASH_Read(IAPStructValue.RunAPPAddressCnt,&IAPStructValue.ReadDate,1);//��ȡԴ��ַ����
				
				if(WriteFlashHalfWord(IAPStructValue.BackupAPPAddressCnt,IAPStructValue.ReadDate) == 0){//д�뻺��
					IAPStructValue.RunAPPAddressCnt = IAPStructValue.RunAPPAddressCnt+2;
					IAPStructValue.BackupAPPAddressCnt = IAPStructValue.BackupAPPAddressCnt+2;
				}
				else{//д��ʧ��,�ٳ���дһ��
					if(WriteFlashHalfWord(IAPStructValue.BackupAPPAddressCnt,IAPStructValue.ReadDate) == 0){//д�뻺��
						IAPStructValue.RunAPPAddressCnt = IAPStructValue.RunAPPAddressCnt+2;
						IAPStructValue.BackupAPPAddressCnt = IAPStructValue.BackupAPPAddressCnt+2;
					}
					else{//д��ʧ��,�ٳ���дһ��
						if(WriteFlashHalfWord(IAPStructValue.BackupAPPAddressCnt,IAPStructValue.ReadDate) == 0){//д�뻺��
							IAPStructValue.RunAPPAddressCnt = IAPStructValue.RunAPPAddressCnt+2;
							IAPStructValue.BackupAPPAddressCnt = IAPStructValue.BackupAPPAddressCnt+2;
						}
						else{
							IAPSetUpdateStatus(UpdateStatus_BackupErr);//�������ĳ��򿽱���������ʧ��
							printf("\r\nErr:FlashBackupErr\r\n");
							IAPResetMCU();//����
						}
					}
				}
			}
		}
		
		IAPSetUpdateStatus(UpdateStatus_EraseErrRunUpdata);//����������ʧ��(����ʱ)
		if(FlashErasePage(IAPStructValue.RunAPPAddress,FLASH_USER_SIZE)!=4)//�������г����ַ
		{
			//Flash ����ʧ��
			IAPSetUpdateStatus(UpdateStatus_EraseErrRunUpdata);//����������ʧ��(����ʱ)
			printf("\r\nErr:FlashEraseErrRun\r\n");
			IAPResetMCU();//����
		}
	}
	else
	{
		IAPStructValue.UpdateStatusValue = IAPGetUpdateStatus();//��ȡ����״̬
		
		if( 
			IAPStructValue.UpdateStatusValue == UpdateStatus_EraseErrRunUpdata || //����������ʧ��(����ʱ)
			IAPStructValue.UpdateStatusValue == UpdateStatus_EraseErrRunBack || //����������ʧ��(������������������ʱ)
			IAPStructValue.UpdateStatusValue == UpdateStatus_BackupToRunErr || //�������ĳ��򿽱�����������
			IAPStructValue.UpdateStatusValue == UpdateStatus_DownloadTimeout || //�������س�ʱ
			IAPStructValue.UpdateStatusValue == UpdateStatus_MainTimeout ||
			IAPStructValue.UpdateStatusValue == UpdateStatus_FlashWriteErr || //Flash д����
			IAPStructValue.UpdateStatusValue == UpdateStatus_DataOverflow || //����������
			IAPStructValue.UpdateStatusValue == UpdateStatus_MissingData || //���ݽ��ղ�����
			IAPStructValue.UpdateStatusValue == UpdateStatus_CheckErr || //У�����
			IAPStructValue.UpdateStatusValue == UpdateStatus_VersionLenErr || //�汾�Ŵ���
			IAPStructValue.UpdateStatusValue == UpdateStatus_DataAddressError || //Flash���ݵĸ�λ��ַ����0x08 ���� RAM�ĸ�λ��ַ����0x20
			IAPStructValue.UpdateStatusValue == UpdateStatus_SetRollBack ||//�ֶ����Ƴ���ع�
			IAPStructValue.UpdateStatusValue == UpdateStatus_WriteAppOkFlage
			)
		{
			if(IAPCheckRamFlashAddress(IAPStructValue.BackupAPPAddress))//�б��ݵĳ���
			{
//				printf("\r\nBackupAPP >>> RunAPP\r\n");
				//�ѱ��ݵĳ��򿽱������е�ַ
				IAPStructValue.UserDataSize = FLASH_USER_SIZE*1024/2;//APP�û������С(����16�ֽ�)
				IAPStructValue.RunAPPAddressCnt = IAPStructValue.RunAPPAddress;//���еĵ�ַ
				IAPStructValue.BackupAPPAddressCnt = IAPStructValue.BackupAPPAddress;//���ݵĵ�ַ
				
				/**����������ɹ���һֱִ�п���,��Ϊȷʵû�а취**/
				if(FlashErasePage(IAPStructValue.RunAPPAddress,FLASH_USER_SIZE)!=4)//����Դ����ĵ�ַ
				{
					IAPSetUpdateStatus(UpdateStatus_EraseErrRunUpdata);//����������ʧ��(������������������ʱ)
					printf("\r\nErr:FlashEraseErr\r\n");
					IAPResetMCU();//����
				}
				
				for(IAPStructValue.BackupCnt=0;IAPStructValue.BackupCnt<IAPStructValue.UserDataSize;IAPStructValue.BackupCnt++)
				{
					STMFLASH_Read(IAPStructValue.BackupAPPAddressCnt,&IAPStructValue.ReadDate,1);//��ȡ���ݵĵ�ַ
					
					if(WriteFlashHalfWord(IAPStructValue.RunAPPAddressCnt,IAPStructValue.ReadDate) == 0){//д�뻺��
						IAPStructValue.RunAPPAddressCnt = IAPStructValue.RunAPPAddressCnt+2;
						IAPStructValue.BackupAPPAddressCnt = IAPStructValue.BackupAPPAddressCnt+2;
					}
					else{//д��ʧ��,�ٳ���дһ��
						if(WriteFlashHalfWord(IAPStructValue.RunAPPAddressCnt,IAPStructValue.ReadDate) == 0){//д�뻺��
							IAPStructValue.RunAPPAddressCnt = IAPStructValue.RunAPPAddressCnt+2;
							IAPStructValue.BackupAPPAddressCnt = IAPStructValue.BackupAPPAddressCnt+2;
						}
						else{//д��ʧ��,�ٳ���дһ��
							if(WriteFlashHalfWord(IAPStructValue.RunAPPAddressCnt,IAPStructValue.ReadDate) == 0){//д�뻺��
								IAPStructValue.RunAPPAddressCnt = IAPStructValue.RunAPPAddressCnt+2;
								IAPStructValue.BackupAPPAddressCnt = IAPStructValue.BackupAPPAddressCnt+2;
							}
							else{
								IAPSetUpdateStatus(UpdateStatus_BackupToRunErr);//�������ĳ��򿽱�����������
								printf("\r\nErr:FlashBackupErr\r\n");
								IAPResetMCU();//����
							}
						}
					}
				}
				IAPSetUpdateStatus(UpdateStatus_BackupToRunOK);//�������ĳ��򿽱����������ɹ�
			}
			else
			{
				IAPSetUpdateStatus(UpdateStatus_NoBackup);//û�б��ݵĳ���
			}
		}
		else if(IAPStructValue.UpdateStatusValue == UpdateStatus_WriteAppOk)//�ϴ���������ȥ����
		{
			IAPSetUpdateStatus(UpdateStatus_WriteAppOkFlage);//д��0xFF
		}
	}
	
	
	
	//��ӡ���µ�Flash������Ϣ
//	printf("\r\nuser1ROMStart: 0x%x\r\n",FLASH_APP1_ADDR);
//	printf("user1ROMSize : 0x%x\r\n",FLASH_USER_SIZE*1024);
//	printf("user2ROMStart: 0x%x\r\n",FLASH_APP2_ADDR);
//	printf("user2ROMSize : 0x%x\r\n",FLASH_USER_SIZE*1024);
//	printf("UpdateStatusValue=%d\r\n",IAPStructValue.UpdateStatusValue);//���µ�״̬����
}


/**
* @brief  �������س�ʱ,��ʱ����;
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
		IAPStructValue.DownloadTimeout=0;//�������س�ʱ:����
		IAPSetUpdateStatus(UpdateStatus_DownloadTimeout);
		IAPResetMCU();//����
	}
}

/**
* @brief  ���ó������س�ʱ
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
* @brief  �������г�ʱ,��ʱ����;
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
		IAPStructValue.MainTimeout=0;//����
		IAPSetUpdateStatus(UpdateStatus_MainTimeout);
		IAPResetMCU();//����
	}
}

/**
* @brief  �����������г�ʱ
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
* @brief  û�и��µ�״̬�¼����û�����
* @warn   
* @param  
* @param  None
* @param  None
* @retval None
* @example
**/
void IAPLoadAPPProgram(void)
{
	//û�и��±�־                 ��     û�в�������
	if(!IAPStructValue.UpdateFlage && !IAPStructValue.PutDataFlage)
	{
		if(IAPCheckRamFlashAddress(IAPStructValue.RunAPPAddress))//���ĳЩλ�õ�Flash�ĸ�λ��ַ�ǲ���0x08,RAM�ĸ�λ��ַ�ǲ���0x20	
		{
			SysTick->CTRL=0;//?????

			USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);//????????
			USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);//????????

			USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);//??????
			USART_ITConfig(USART2, USART_IT_IDLE, DISABLE);//??????

			USART_Cmd(USART1, DISABLE);                    
			USART_Cmd(USART2, DISABLE);        
			
			iap_load_app(IAPStructValue.RunAPPAddress);//�����û�����
		}
	}
}




//������ݵ�ĳЩλ�õ�Flash�ĸ�λ��ַ�ǲ���0x08		//RAM�ĸ�λ��ַ�ǲ���0x20	
//1: ��ȷ  0:����ȷ
char IAPCheckRamFlashAddress(u32 Address)
{
	if(((*(vu32*)(Address+4))&0xFF000000)==0x08000000)//Flash�ĸ�λ��ַ�ǲ���0x08						
	{
		if(((*(vu32*)Address)&0x2FFE0000)==0x20000000)//RAM�ĸ�λ��ַ�ǲ���0x20		
		{
			return 1;
		}
	}
	return 0;
}

//����������һ�ݳ���  1:���е�һ�ݳ���  2:���еڶ��ݳ���
void IAPSetUpdateRunProgram(u16 ReadDat)
{
	STMFLASH_Write(FLASH_RUNUSER_ADDR,&ReadDat,1);
}

//��ȡ�������е���һ�ݳ���  0:û�����еĳ���  1:���е�һ�ݳ���  2:���еڶ��ݳ���
u16 IAPGetUpdateRunProgram(void)
{
	u16 ReadDat = 0;
	
  STMFLASH_Read(FLASH_RUNUSER_ADDR,&ReadDat,1);
	
	return ReadDat == 65535 ? 0 : ReadDat;
}

//�л����г����ַ
void IAPSetUpdateChangeProgram(void)
{
	if(IAPGetUpdateRunProgram() == 1) IAPSetUpdateRunProgram(2);
	else IAPSetUpdateRunProgram(1);
}


//���ø��±�־
void IAPSetUpdateFlage(void)
{
	u16 ReadDat = 1;
  STMFLASH_Write(FLASH_UPDATE_FLAGE_ADDR,&ReadDat,1);
}

//������±�־
void IAPClearUpdateFlage(void)
{
	u16 ReadDat = 0;
  STMFLASH_Write(FLASH_UPDATE_FLAGE_ADDR,&ReadDat,1);
}

//��ȡ���±�־
u16 IAPGetUpdateFlage(void)
{
	u16 ReadDat = 0;
  STMFLASH_Read(FLASH_UPDATE_FLAGE_ADDR,&ReadDat,1);
	return ReadDat == 65535 ? 0 : ReadDat;
}


//�����ƶ˵İ汾
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

//��ȡ�ƶ˵İ汾
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


//�����豸�İ汾
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


//��ȡ�豸�İ汾
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

//�����豸����״̬
void IAPSetUpdateStatus(char Status)
{
	u16 ReadDat = Status;
  STMFLASH_Write(FLASH_UPDATE_STATUS_ADDR,&ReadDat,1);
}

//��ȡ�豸����״̬
u16 IAPGetUpdateStatus(void)
{
  u16 ReadDat = 0;
  STMFLASH_Read(FLASH_UPDATE_STATUS_ADDR,&ReadDat,1);
	return ReadDat == 65535 ? 0 : ReadDat;
}

//addr:ջ����ַ
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

//��ת��Ӧ�ó����
//appxaddr:�û�������ʼ��ַ.
void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
	{ 
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//����APP����ĸ�λ�жϺ�����ַ
		MSR_MSP(*(vu32*)appxaddr);					//��ʼ��APP��ջָ��,��APP����Ķ�ջ�����ع�,����˵���·���RAM
		jump2app();									//ִ��APP�ĸ�λ�жϺ���,���ձ����ת��APP��main����
	}
}		 

