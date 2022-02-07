#ifndef __IAP_H__
#define __IAP_H__
#include <stm32f10x.h>

#ifndef IAP_C_//如果没有定义
#define IAP_Ex_ extern
#else
#define IAP_Ex_
#endif

/*更新的状态:*/
#define UpdateStatus_None              0x00    //无
#define UpdateStatus_WriteAppOk        0x01    //上次成功写入了更新程序

#define UpdateStatus_EraseErrBack      0x03    //擦除备份区失败
#define UpdateStatus_BackupErr         0x04    //运行区的程序拷贝到备份区失败
#define UpdateStatus_BackupToRunOK     0x05    //备份区的程序拷贝到运行区成功


#define UpdateStatus_EraseErrRunUpdata 0x20    //擦除运行区失败(更新时)
#define UpdateStatus_EraseErrRunBack   0x21    //擦除运行区失败(备份区拷贝到运行区时)
#define UpdateStatus_BackupToRunErr    0x22    //备份区的程序拷贝运行区错误

#define UpdateStatus_DownloadTimeout   0x23    //程序下载超时
#define UpdateStatus_FlashWriteErr     0x24    //Flash 写错误
#define UpdateStatus_DataOverflow      0x25    //缓存接收溢出
#define UpdateStatus_MissingData       0x26    //数据接收不完整
#define UpdateStatus_CheckErr          0x27    //校验错误
#define UpdateStatus_VersionLenErr     0x28    //版本号错误
#define UpdateStatus_DataAddressError  0x29    //Flash数据的高位地址不是0x08 或者 RAM的高位地址不是0x20
#define UpdateStatus_MainTimeout       0x2A    //BootLoader运行超时
#define UpdateStatus_NoBackup          0x2B    //没有备份的程序

#define UpdateStatus_SetRollBack       0x30    //手动控制程序回滚

#define UpdateStatus_WriteAppOkFlage   0xFF    //如果获取的状态是 0x01 则写入该值


typedef struct {
	char  VersionServer[20];  //云端版本
	char  VersionDevice[20];  //设备当前版本
	char  UpdateStatusStr[30];//存储更新的状态对应的字符串
	u16   UpdateStatusValue;  //获取更新的状态
}IAPStruct;

IAP_Ex_ IAPStruct IAPStructValue;


//设置设备的版本
void IAPSetHardwareVersion(u16 data);
//获取设备的版本
u16 IAPGetHardwareVersion(void);

void IAPGetUpdateInfo(void);
void IAPUpdateDispose(void);
void IAPSetUpdateFlage(void);             //设置更新标志
void IAPClearUpdateFlage(void);           //清除更新标志
u16  IAPGetUpdateFlage(void);             //获取更新标志
void IAPSetUpdateVersionServer(char* ch); //设置云端的版本
void IAPGetUpdateVersionServer(char* ch); //获取云端的版本
void IAPSetUpdateVersionDevice(char* ch); //设置设备的版本
void IAPGetUpdateVersionDevice(char* ch); //获取设备的版本
void IAPSetUpdateStatus(char Status);     //设置设备更新状态
u16  IAPGetUpdateStatus(void);            //获取设备更新状态
typedef  void (*iapfun)(void);				    //定义一个函数指针类型		   
void iap_load_app(u32 appxaddr);			    //跳转到APP程序执行
#endif

