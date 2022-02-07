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



#define UpdateDownloadTimeoutCompare 30000     //设置程序下载超时时间 Ms
//#define UpdateDownloadTimeoutCompare 10000
#define MainTimeoutCompare 300000              //整个程序强制超时时间,超时重启
//#define MainTimeoutCompare 60000

#define UpdateReceiveBuffLen 20    //接收程序数据缓存数组大小
typedef struct {
	char  Buff[UpdateReceiveBuffLen];//接收程序数据缓存数组
	char  ReadDat[2];         //从环形队列读取数据使用
	char  PutDataFlage;       //控制是不是可以向环形队列写入数据
	char  Overflow;           //环形队列是否溢出
	char  ReadDataEndFlage;   //确实接收完了程序
	char  UpdateFlage;        //更新标志
	char  FlashWriteErrFlage; //写入Flash出错标志
	char  DataCheckErrFlage;  //数据校验错误
	u16   UpdateStatusValue;  //获取更新的状态
	u16   ReadDate;           //16位数据
	u32   BackupAPPAddress;   //备份程序的地址
	u32   BackupAPPAddressCnt;//备份程序的地址累加
	u32   RunAPPAddress;      //运行的程序地址
	u32   RunAPPAddressCnt;   //运行的程序地址累加
	u32   DownloadTimeout;    //程序下载超时累加变量
	u32   MainTimeout;        //整体运行超时累加变量
	u32   BackupCnt;					//备份拷贝使用
	u32   BackupCount;				//备份拷贝使用
	u32   UserDataSize;       //IAP程序文件大小
	char* Str;                //通用变量
	long  Len;                //通用变量
}IAPStruct;

IAP_Ex_ IAPStruct IAPStructValue;


//设置设备的版本
void IAPSetHardwareVersion(u16 data);
//获取设备的版本
u16 IAPGetHardwareVersion(void);

void IAPInit(void);
void IAP(void);
void IAPWriteData(void);
void IAPVersionDispose(char *data);
void IAPDownloadTimeoutFunction(void);     //程序下载超时,尝试加载用户程序
void IAPResetDownloadTimeoutFunction(void);//重置程序下载超时
void IAPMainTimeoutFunction(void);         //整体运行超时,超时重启;
void IAPResetIAPMainTimeoutFunction(void); //重置整体运行超时
void IAPLoadAPPProgram(void);              //没有更新的状态下加载用户程序
void IAPReceiveProgramDataToFlash(void);   //读取环形队列里面的程序,写入Flash,设置写入的状态,重启
char IAPCheckRamFlashAddress(u32 Address); //检测接收程序某些位置的Flash的高位地址是不是0x08,RAM的高位地址是不是0x20
void IAPSetUpdateRunProgram(u16 ReadDat);  //设置运行哪一份程序  1:运行第一份程序  2:运行第二份程序
u16  IAPGetUpdateRunProgram(void);         //获取正在运行的哪一份程序  0:没有运行的程序  1:运行第一份程序  2:运行第二份程序
void IAPSetUpdateChangeProgram(void);     //切换运行程序地址
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

