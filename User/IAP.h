#ifndef __IAP_H__
#define __IAP_H__
#include <stm32f10x.h>

#ifndef IAP_C_//���û�ж���
#define IAP_Ex_ extern
#else
#define IAP_Ex_
#endif

/*���µ�״̬:*/
#define UpdateStatus_None              0x00    //��
#define UpdateStatus_WriteAppOk        0x01    //�ϴγɹ�д���˸��³���

#define UpdateStatus_EraseErrBack      0x03    //����������ʧ��
#define UpdateStatus_BackupErr         0x04    //�������ĳ��򿽱���������ʧ��
#define UpdateStatus_BackupToRunOK     0x05    //�������ĳ��򿽱����������ɹ�


#define UpdateStatus_EraseErrRunUpdata 0x20    //����������ʧ��(����ʱ)
#define UpdateStatus_EraseErrRunBack   0x21    //����������ʧ��(������������������ʱ)
#define UpdateStatus_BackupToRunErr    0x22    //�������ĳ��򿽱�����������

#define UpdateStatus_DownloadTimeout   0x23    //�������س�ʱ
#define UpdateStatus_FlashWriteErr     0x24    //Flash д����
#define UpdateStatus_DataOverflow      0x25    //����������
#define UpdateStatus_MissingData       0x26    //���ݽ��ղ�����
#define UpdateStatus_CheckErr          0x27    //У�����
#define UpdateStatus_VersionLenErr     0x28    //�汾�Ŵ���
#define UpdateStatus_DataAddressError  0x29    //Flash���ݵĸ�λ��ַ����0x08 ���� RAM�ĸ�λ��ַ����0x20
#define UpdateStatus_MainTimeout       0x2A    //BootLoader���г�ʱ
#define UpdateStatus_NoBackup          0x2B    //û�б��ݵĳ���

#define UpdateStatus_SetRollBack       0x30    //�ֶ����Ƴ���ع�

#define UpdateStatus_WriteAppOkFlage   0xFF    //�����ȡ��״̬�� 0x01 ��д���ֵ


typedef struct {
	char  VersionServer[20];  //�ƶ˰汾
	char  VersionDevice[20];  //�豸��ǰ�汾
	char  UpdateStatusStr[30];//�洢���µ�״̬��Ӧ���ַ���
	u16   UpdateStatusValue;  //��ȡ���µ�״̬
}IAPStruct;

IAP_Ex_ IAPStruct IAPStructValue;


//�����豸�İ汾
void IAPSetHardwareVersion(u16 data);
//��ȡ�豸�İ汾
u16 IAPGetHardwareVersion(void);

void IAPGetUpdateInfo(void);
void IAPUpdateDispose(void);
void IAPSetUpdateFlage(void);             //���ø��±�־
void IAPClearUpdateFlage(void);           //������±�־
u16  IAPGetUpdateFlage(void);             //��ȡ���±�־
void IAPSetUpdateVersionServer(char* ch); //�����ƶ˵İ汾
void IAPGetUpdateVersionServer(char* ch); //��ȡ�ƶ˵İ汾
void IAPSetUpdateVersionDevice(char* ch); //�����豸�İ汾
void IAPGetUpdateVersionDevice(char* ch); //��ȡ�豸�İ汾
void IAPSetUpdateStatus(char Status);     //�����豸����״̬
u16  IAPGetUpdateStatus(void);            //��ȡ�豸����״̬
typedef  void (*iapfun)(void);				    //����һ������ָ������		   
void iap_load_app(u32 appxaddr);			    //��ת��APP����ִ��
#endif

