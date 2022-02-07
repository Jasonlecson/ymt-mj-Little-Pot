/**
  ******************************************************************************
  * @author  yang feng wu 
  * @version V1.0.0
  * @date    2019/10/12
  * @brief   
  ******************************************************************************
	  
  ******************************************************************************
  */
#define __MAIN_C_
#include "include.h"

#include "IAP.h"
#include "BufferManage.h"

#include "stmflash.h"

#define SOH 0x01
#define STX 0x02
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15
#define CAN 0x18

/*硬件版本*/


/*版本0*/
//#define HardwareVersion 0 //基础版本

/*版本1
1.增加PB1控制肥力传感器工作
2.湿度传感器值*1.4
*/
u16  HardwareVersion = 3;


char Bits1=0x01;
char Bits2=0xFE;


//缓存数据使用
char MainBuffer[MainBufferLen];//缓存数据,全局通用
u32  MainLen=0;      //全局通用变量
char *MainString;    //全局通用变量
int  Len=0;

u32  GetVersionInfoCnt=999999;//间隔发送  程序版本 使用


/*************BufferManager发送数据缓存管理*****************/
u8  BFbuff[1024];
u32 BFManagebuff[10];
u8  BFSendBuff[1024];
/*************BufferManager*****************/

/*************BufferManager发送数据缓存管理*****************/
u8  BFbuffNet[1024];
u32 BFManagebuffNet[50];
u8  BFSendBuffNet[1024];
/*************BufferManager*****************/

char UpdataState=0;

char ReadEOTFlag=0;//接收到接收信号
u32 ReadEOTCnt=0;//

char crc16_check_err_cnt=0;//校验CRC错误次数

char KeyDownLong = 0;/*按下时间大于20S*/

//回退版本累加计数
u32  BackspaceFileCnt=0;
int  BackspaceFileLed=0;


//u8 mcu_version[20]="0000\r\n";//版本
u8 get_model[20]="ymt.flowerpot.v3\r\n";//产品型号
u8 get_down[10]="get_down\r\n";//获取服务器下发的信息
u8 get_time_posix[15]="time posix\r\n";//获取时间戳
u8 get_net[6]="net\r\n";//获取网络状态


u8 set_ble[25]="ble_config set 2692";//蓝牙
u8 dump_ble[25]="[\"product id\":2692";


u32  GetNetTimerCnt=0;
u32  GetBleTimerCnt=0;
u8   InitBleFlag=0;
u32  InitBleTimerCnt=0;
char SmartConfigFlage=0;
u32  SmartConfigCnt=0;
char RunSmartConfigOne=0;
u32  module_net_state_led_cnt=0;
char RuningUpdataFlag = 0;//
u32  RuningUpdataDelay=0;

unsigned short crc16_ccitt( const char *buf, int len);
int check(const char *buf, int sz);

//#define KeyDownTimer1 10000
//#define KeyDownTimer2 20000

#define KeyDownTimer1 5000
#define KeyDownTimer2 10000
void BackspaceFile(void)
{
	while(SmartConfigKeyPinIn == KeySmartConfigDown)
	{
		delay_ms(1);
		BackspaceFileCnt++;
		if(BackspaceFileCnt>KeyDownTimer1)
		{
			if(BackspaceFileCnt<KeyDownTimer2)//快闪
			{
				delay_ms(1);
				BackspaceFileLed++;
				if(BackspaceFileLed>900)
				{
					BackspaceFileLed=0;
					PAout(5) = ~PAin(5);
				}
				IWDG_Feed();//喂狗
			}
			else //常亮
			{
				delay_ms(1);
				PAout(5) = 1;
				IWDG_Feed();//喂狗
				KeyDownLong = 1;
				/**复位模块**/
				PAout(11) = 0;
				delay_ms(100);
				PAout(11) = 1;
				delay_ms(3000);
				break;
			}
		}
		IWDG_Feed();//喂狗
	}
	BackspaceFileLed=0;
	
	if(BackspaceFileCnt>=KeyDownTimer1 && BackspaceFileCnt < KeyDownTimer2)
	{
		IAPSetUpdateStatus(UpdateStatus_SetRollBack);
		printf("SetRollBack\r\n");
		Reset_MCU();
	}
	BackspaceFileCnt=0;
}


void SmartConfig(void);
void InitBleFun(void);
int main(void)
{
	/*串口接收缓存*/
	BufferManageCreate(&buff_manage_struct_net,BFbuffNet, 1024, BFManagebuffNet, 50*4);
	BufferManageCreate(&buff_manage_struct_t,  BFbuff, 1024, BFManagebuff, 10*4);
	BufferManageCreate(&buff_manage_struct1,Usart1BufferMemory,Usart1BufferMemoryLen,Usart1BufferMemoryManage,Usart1BufferMemoryManageLen*4);
  NVIC_Configuration();
	uart_init(115200);	 //串口初始化为115200
	
	IAPInit();
	
	if(IAPGetHardwareVersion() != HardwareVersion)
	{
		IAPSetHardwareVersion(HardwareVersion);
	}
	

	GpioInit();
	
	DelayInit();
	
	if(GetSystemResetType()==1){
		BackspaceFile();
	}
	
	if(!KeyDownLong){
		IAP();
	}
	IWDG_Init(IWDG_Prescaler_256,156*5); //时钟40K,分频数为256,156.25HZ,6.4ms;喂狗最小间隔5S
	
//	while(1);
	
	buff_manage_struct_t.SendLen=0;
	Usart1SendDataCnt = 0;
	
	buff_manage_struct_net.SendLen = 0;
	buff_manage_struct_net.Cnt=0;
	SmartConfigCnt=0;
	SmartConfigFlage=0;
//	IAPStructValue.UpdateFlage = 1;
	while(1)
	{
		
		IWDG_Feed();//喂狗
		
		if(KeyDownLong)/*配网*/
		{
			if(buff_manage_struct_net.Cnt<=0 && Usart1SendDataCnt<=0)
			{
				BufferManageRead(&buff_manage_struct_net,BFSendBuffNet,&buff_manage_struct_net.SendLen);
				if(buff_manage_struct_net.SendLen>0){
					UsartOutStrIT(BFSendBuffNet,buff_manage_struct_net.SendLen);
					buff_manage_struct_net.SendLen=0;
					buff_manage_struct_net.Cnt = 150;
				}
			}
			
			//读取串口接收的数据
			memset(Usart1BufferMemoryRead,0,Usart1BufferMemoryReadLen);
			BufferManageRead(&buff_manage_struct1,Usart1BufferMemoryRead,&buff_manage_struct1.ReadLen);
			if(buff_manage_struct1.ReadLen>0){//有数据
				
			}
			
			/*检测配网状态,超时检测*/
			if(SmartConfigFlage){
				if(SmartConfigCnt>5000){
					SmartConfigCnt = 5001;
					
					if(module_net_state == net_status_cloud){
						SmartConfigFlage=0;
						
						//版本0000
						MainLen = sprintf(MainBuffer,"mcu_version 0000\r\n");
						BufferManageWrite(&buff_manage_struct_net,MainBuffer,MainLen,&Len);
						BufferManageWrite(&buff_manage_struct_net,MainBuffer,MainLen,&Len);
						
						
						if(FlashErasePage(IAPStructValue.RunAPPAddress,FLASH_USER_SIZE)!=4)//擦除运行程序地址
						{
							//Flash 擦除失败
							IAPSetUpdateStatus(UpdateStatus_EraseErrRunUpdata);//擦除运行区失败(更新时)
							printf("\r\nErr:FlashEraseErrRun\r\n");
							Reset_MCU();//重启
						}
						
						RuningUpdataFlag = 1;
						RuningUpdataDelay = 0;
						Usart1SendDataCnt=0;
					}
				}
			}
		
			if(RuningUpdataFlag){
				if(RuningUpdataDelay>=2000){
					RuningUpdataDelay=0;
					RuningUpdataFlag=0;
					KeyDownLong = 0;
					Usart1SendDataCnt=0;
				}
			}
			
			/*每隔1S获取一次网络状态*/
			if(GetNetTimerCnt>=1000 && SmartConfigFlage)
			{
				GetNetTimerCnt = 0;
				BufferManageWrite(&buff_manage_struct_net,"net\r\n",5,&Len);
				BufferManageWrite(&buff_manage_struct_net,"get_down\r",9,&Len);
			}
			
			
			SmartConfig();//配网
			
			if(SmartConfigFlage){
				InitBleFun();
			}
		}
		else
		{	
			IAPLoadAPPProgram();//没有更新的状态下加载用户程序
			IAPDownloadTimeoutFunction();//下载超时
			IAPMainTimeoutFunction();//整体运行超时
			
			
			/*有数据需要发送*/
			if(buff_manage_struct_t.SendLen==0 && Usart1SendDataCnt==0){
				BufferManageRead(&buff_manage_struct_t,BFSendBuff,&buff_manage_struct_t.SendLen);
				if(buff_manage_struct_t.SendLen>0){
					UsartOutStrIT(BFSendBuff,buff_manage_struct_t.SendLen);
					buff_manage_struct_t.SendLen=0;
				}
			}
			
			
			if(UpdataState==0){
				if(GetVersionInfoCnt > 500)
				{
					BufferManageWrite(&buff_manage_struct_t,"get_down\r",9,&Len);
					GetVersionInfoCnt=0;
				}
			}
			
			
			if(ReadEOTCnt>=2000){
				if(IAPStructValue.FlashWriteErrFlage)
				{
					IAPSetUpdateStatus(UpdateStatus_FlashWriteErr);
				}
				else if(!IAPCheckRamFlashAddress(IAPStructValue.RunAPPAddress))
				{
					IAPSetUpdateStatus(UpdateStatus_DataAddressError);
				}
				else if(IAPStructValue.DataCheckErrFlage == 1)
				{
					IAPSetUpdateStatus(UpdateStatus_CheckErr);
				}
				else
				{
					IAPSetUpdateStatus(UpdateStatus_WriteAppOk);//设置标志位为 0x01
				}
				delay_ms(100);
				
				
				ReadEOTCnt=0;
				Reset_MCU();
			}
			
			//读取串口接收的数据
			memset(Usart1BufferMemoryRead,0,Usart1BufferMemoryReadLen);
			BufferManageRead(&buff_manage_struct1,Usart1BufferMemoryRead,&buff_manage_struct1.ReadLen);
			if(buff_manage_struct1.ReadLen>0){//有数据
				
				if(memcmp(Usart1BufferMemoryRead,"down update_fw",14) ==0 ){
					UpdataState = 1;
					IAPStructValue.PutDataFlage = 1;
					IAPStructValue.UpdateFlage=1;
					BufferManageWrite(&buff_manage_struct_t,"result \"ready\"\r",15,&Len);
				}
				else if(UpdataState==1 && memcmp(Usart1BufferMemoryRead,"ok",2) ==0){
					BufferManageWrite(&buff_manage_struct_t,"C",1,&Len);
					UpdataState = 2;
					
					IAPStructValue.RunAPPAddressCnt = IAPStructValue.RunAPPAddress;//运行的地址
					IAPStructValue.BackupAPPAddressCnt = IAPStructValue.BackupAPPAddress;//备份的地址
				}
				else if(UpdataState==2){//正常接收数据
					
					IAPResetDownloadTimeoutFunction();
					
					if(buff_manage_struct1.ReadLen==133){//程序文件
						
						//校验程序文件,写入flash
						if(Usart1BufferMemoryRead[0]==SOH &&  Usart1BufferMemoryRead[1]==Bits1 && Bits2==Usart1BufferMemoryRead[2]){
							
							if(check(&Usart1BufferMemoryRead[3],128) ==1){//检测CRC是否正确
								crc16_check_err_cnt = 0;
								MainBuffer[0] = ACK;
								BufferManageWrite(&buff_manage_struct_t,MainBuffer,1,&Len);
								
								Bits1++;
								Bits2--;
								
								if(IAPStructValue.RunAPPAddressCnt< IAPStructValue.BackupAPPAddress)//不能超过 备份的地址
								{
									if(!IAPStructValue.FlashWriteErrFlage)//写Flash没有错误
									{
										for(IAPStructValue.BackupCnt=0;IAPStructValue.BackupCnt<64;IAPStructValue.BackupCnt++){
											
											IAPStructValue.ReadDate = Usart1BufferMemoryRead[IAPStructValue.BackupCnt*2+4];
											IAPStructValue.ReadDate = IAPStructValue.ReadDate<<8;
											IAPStructValue.ReadDate = IAPStructValue.ReadDate|Usart1BufferMemoryRead[IAPStructValue.BackupCnt*2+3];//拼接数据
											
											
											if(WriteFlashHalfWord(IAPStructValue.RunAPPAddressCnt,IAPStructValue.ReadDate) != 0)//写Flash
											{
												IAPStructValue.FlashWriteErrFlage = 1;//写Flash错误
											}
											IAPStructValue.RunAPPAddressCnt+=2;//地址增加
										}
									}
								}
							}
							else{
								crc16_check_err_cnt++;
								
								if(crc16_check_err_cnt>6){
									crc16_check_err_cnt=0;
									
									IAPStructValue.DataCheckErrFlage = 1;
									
									MainBuffer[0] = CAN;//结束传输
									BufferManageWrite(&buff_manage_struct_t,MainBuffer,1,&Len);
									ReadEOTFlag=1;
								}
							}
						}
						else {//写入flash耽误了时间,过来了上一次的数据(或者说根本不匹配数据以后默认一直读取完数据)
							MainBuffer[0] = ACK;
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,1,&Len);
						}
					}
					else if(buff_manage_struct1.ReadLen==1 && Usart1BufferMemoryRead[0] == EOT){//结束信号
						MainBuffer[0] = ACK;
						BufferManageWrite(&buff_manage_struct_t,MainBuffer,1,&Len);
						
						ReadEOTFlag=1;					
					}
					else
					{
//						printf("llll=%d\r\n",buff_manage_struct1.ReadLen);
//						UpdataState = 0;
//						IAPStructValue.PutDataFlage = 0;
//						Bits1=0x01;
//						Bits2=0xFE;
					}
				}
			}
		}
	}
}


/**
* @brief   SmartConfig
* @param   None
* @param   None
* @param   None
* @retval  None
* @warning None
* @example 
**/
void SmartConfig(void){
	if(!SmartConfigFlage &&!RunSmartConfigOne)
	{
		RunSmartConfigOne = 1;
		SmartConfigFlage = 1;
		SmartConfigCnt=0;
		InitBleFlag = 0;
		/*model*/
		MainLen = sprintf(MainBuffer,"model %s",get_model);
		BufferManageWrite(&buff_manage_struct_net,MainBuffer,MainLen,&Len);
		BufferManageWrite(&buff_manage_struct_net,MainBuffer,MainLen,&Len);
		//版本0000
		MainLen = sprintf(MainBuffer,"mcu_version 0000\r\n");
		BufferManageWrite(&buff_manage_struct_net,MainBuffer,MainLen,&Len);
		BufferManageWrite(&buff_manage_struct_net,MainBuffer,MainLen,&Len);
		
		//重新配置网络
		MainLen = sprintf(MainBuffer,"%s\r\n","restore");
		BufferManageWrite(&buff_manage_struct_net,MainBuffer,MainLen,&Len);
		BufferManageWrite(&buff_manage_struct_net,MainBuffer,MainLen,&Len);
		BufferManageWrite(&buff_manage_struct_net,MainBuffer,MainLen,&Len);
	}
}


void InitBleFun(void)
{
	if(!InitBleFlag)
	{
		if(InitBleTimerCnt>1000)
		{
			InitBleTimerCnt = 0;
			MainLen = sprintf(MainBuffer,"%s 0000\r\n",set_ble);
			BufferManageWrite(&buff_manage_struct_net,MainBuffer,MainLen,&Len);
		}
		if(GetBleTimerCnt>500)
		{
			GetBleTimerCnt=0;
			MainLen = sprintf(MainBuffer,"ble_config dump\r\n");
			BufferManageWrite(&buff_manage_struct_net,MainBuffer,MainLen,&Len);
		}
	}
}


unsigned short crc16_ccitt( const char *buf, int len)
{
	unsigned short crc = 0;
	while( len-- ) {
		int i;
		crc ^= *buf++ << 8;
		
		for( i = 0; i < 8; ++i ) {
			if( crc & 0x8000 )
				crc = (crc << 1) ^ 0x1021;
			else
				crc = crc << 1;
		}
	}
	return crc;
}


int check(const char *buf, int sz)
{
	unsigned short crc = crc16_ccitt(buf, sz);
	unsigned short tcrc = (buf[sz]<<8)+buf[sz+1];
	if (crc == tcrc)
		return 1;
	
	return 0;
}



