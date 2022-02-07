/**
  ******************************************************************************
  * @file    usart.c
  * @author  fengwu yang
  * @version V1.0.0
  * @date    2019/10/12
  * @brief   IAP����
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
* @brief  ��ȡ���µ���Ϣ
* @param  
* @param  None
* @param  None
* @retval None
* @example 
**/
void IAPGetUpdateInfo(void)
{
//	IAPGetUpdateVersionServer(IAPStructValue.VersionServer);//��ȡ�ƶ˰汾 
//	IAPGetUpdateVersionDevice(IAPStructValue.VersionDevice);//��ȡ�豸�汾
	IAPStructValue.UpdateStatusValue = IAPGetUpdateStatus();//��ȡ����״̬
}


/**
* @brief  �������
* @param  
* @param  None
* @param  None
* @retval None
* @example 
**/
void IAPUpdateDispose(void)
{
	//�������
	if(IAPStructValue.UpdateStatusValue!=0)//����������
	{
		IAPSetUpdateStatus(UpdateStatus_None);//��������״̬
//		if(IAPStructValue.UpdateStatusValue == UpdateStatus_WriteAppOkFlage)//�����е��³���
//		{
//			IAPGetUpdateVersionServer(IAPStructValue.VersionServer);//��ȡ�ƶ˰汾 
//			IAPSetUpdateVersionDevice(IAPStructValue.VersionServer);//�л�����汾
//			IAPGetUpdateVersionDevice(IAPStructValue.VersionDevice);//��ȡ�豸�汾
//		}
		IAPStructValue.UpdateStatusValue=0;
	}			
	else
	{
		memset(IAPStructValue.UpdateStatusStr,0,sizeof(IAPStructValue.UpdateStatusStr));
		sprintf(IAPStructValue.UpdateStatusStr,"%s","NoUpdata");
	}
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

