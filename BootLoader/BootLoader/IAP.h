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



#define UpdateDownloadTimeoutCompare 30000     //���ó������س�ʱʱ�� Ms
//#define UpdateDownloadTimeoutCompare 10000
#define MainTimeoutCompare 300000              //��������ǿ�Ƴ�ʱʱ��,��ʱ����
//#define MainTimeoutCompare 60000

#define UpdateReceiveBuffLen 20    //���ճ������ݻ��������С
typedef struct {
	char  Buff[UpdateReceiveBuffLen];//���ճ������ݻ�������
	char  ReadDat[2];         //�ӻ��ζ��ж�ȡ����ʹ��
	char  PutDataFlage;       //�����ǲ��ǿ������ζ���д������
	char  Overflow;           //���ζ����Ƿ����
	char  ReadDataEndFlage;   //ȷʵ�������˳���
	char  UpdateFlage;        //���±�־
	char  FlashWriteErrFlage; //д��Flash�����־
	char  DataCheckErrFlage;  //����У�����
	u16   UpdateStatusValue;  //��ȡ���µ�״̬
	u16   ReadDate;           //16λ����
	u32   BackupAPPAddress;   //���ݳ���ĵ�ַ
	u32   BackupAPPAddressCnt;//���ݳ���ĵ�ַ�ۼ�
	u32   RunAPPAddress;      //���еĳ����ַ
	u32   RunAPPAddressCnt;   //���еĳ����ַ�ۼ�
	u32   DownloadTimeout;    //�������س�ʱ�ۼӱ���
	u32   MainTimeout;        //�������г�ʱ�ۼӱ���
	u32   BackupCnt;					//���ݿ���ʹ��
	u32   BackupCount;				//���ݿ���ʹ��
	u32   UserDataSize;       //IAP�����ļ���С
	char* Str;                //ͨ�ñ���
	long  Len;                //ͨ�ñ���
}IAPStruct;

IAP_Ex_ IAPStruct IAPStructValue;


//�����豸�İ汾
void IAPSetHardwareVersion(u16 data);
//��ȡ�豸�İ汾
u16 IAPGetHardwareVersion(void);

void IAPInit(void);
void IAP(void);
void IAPWriteData(void);
void IAPVersionDispose(char *data);
void IAPDownloadTimeoutFunction(void);     //�������س�ʱ,���Լ����û�����
void IAPResetDownloadTimeoutFunction(void);//���ó������س�ʱ
void IAPMainTimeoutFunction(void);         //�������г�ʱ,��ʱ����;
void IAPResetIAPMainTimeoutFunction(void); //�����������г�ʱ
void IAPLoadAPPProgram(void);              //û�и��µ�״̬�¼����û�����
void IAPReceiveProgramDataToFlash(void);   //��ȡ���ζ�������ĳ���,д��Flash,����д���״̬,����
char IAPCheckRamFlashAddress(u32 Address); //�����ճ���ĳЩλ�õ�Flash�ĸ�λ��ַ�ǲ���0x08,RAM�ĸ�λ��ַ�ǲ���0x20
void IAPSetUpdateRunProgram(u16 ReadDat);  //����������һ�ݳ���  1:���е�һ�ݳ���  2:���еڶ��ݳ���
u16  IAPGetUpdateRunProgram(void);         //��ȡ�������е���һ�ݳ���  0:û�����еĳ���  1:���е�һ�ݳ���  2:���еڶ��ݳ���
void IAPSetUpdateChangeProgram(void);     //�л����г����ַ
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

