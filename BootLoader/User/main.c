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

/*Ӳ���汾*/


/*�汾0*/
//#define HardwareVersion 0 //�����汾

/*�汾1
1.����PB1���Ʒ�������������
2.ʪ�ȴ�����ֵ*1.4
*/
u16  HardwareVersion = 3;


char Bits1=0x01;
char Bits2=0xFE;


//��������ʹ��
char MainBuffer[MainBufferLen];//��������,ȫ��ͨ��
u32  MainLen=0;      //ȫ��ͨ�ñ���
char *MainString;    //ȫ��ͨ�ñ���
int  Len=0;

u32  GetVersionInfoCnt=999999;//�������  ����汾 ʹ��


/*************BufferManager�������ݻ������*****************/
u8  BFbuff[1024];
u32 BFManagebuff[10];
u8  BFSendBuff[1024];
/*************BufferManager*****************/

/*************BufferManager�������ݻ������*****************/
u8  BFbuffNet[1024];
u32 BFManagebuffNet[50];
u8  BFSendBuffNet[1024];
/*************BufferManager*****************/

char UpdataState=0;

char ReadEOTFlag=0;//���յ������ź�
u32 ReadEOTCnt=0;//

char crc16_check_err_cnt=0;//У��CRC�������

char KeyDownLong = 0;/*����ʱ�����20S*/

//���˰汾�ۼӼ���
u32  BackspaceFileCnt=0;
int  BackspaceFileLed=0;


//u8 mcu_version[20]="0000\r\n";//�汾
u8 get_model[20]="ymt.flowerpot.v3\r\n";//��Ʒ�ͺ�
u8 get_down[10]="get_down\r\n";//��ȡ�������·�����Ϣ
u8 get_time_posix[15]="time posix\r\n";//��ȡʱ���
u8 get_net[6]="net\r\n";//��ȡ����״̬


u8 set_ble[25]="ble_config set 2692";//����
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
			if(BackspaceFileCnt<KeyDownTimer2)//����
			{
				delay_ms(1);
				BackspaceFileLed++;
				if(BackspaceFileLed>900)
				{
					BackspaceFileLed=0;
					PAout(5) = ~PAin(5);
				}
				IWDG_Feed();//ι��
			}
			else //����
			{
				delay_ms(1);
				PAout(5) = 1;
				IWDG_Feed();//ι��
				KeyDownLong = 1;
				/**��λģ��**/
				PAout(11) = 0;
				delay_ms(100);
				PAout(11) = 1;
				delay_ms(3000);
				break;
			}
		}
		IWDG_Feed();//ι��
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
	/*���ڽ��ջ���*/
	BufferManageCreate(&buff_manage_struct_net,BFbuffNet, 1024, BFManagebuffNet, 50*4);
	BufferManageCreate(&buff_manage_struct_t,  BFbuff, 1024, BFManagebuff, 10*4);
	BufferManageCreate(&buff_manage_struct1,Usart1BufferMemory,Usart1BufferMemoryLen,Usart1BufferMemoryManage,Usart1BufferMemoryManageLen*4);
  NVIC_Configuration();
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
	
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
	IWDG_Init(IWDG_Prescaler_256,156*5); //ʱ��40K,��Ƶ��Ϊ256,156.25HZ,6.4ms;ι����С���5S
	
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
		
		IWDG_Feed();//ι��
		
		if(KeyDownLong)/*����*/
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
			
			//��ȡ���ڽ��յ�����
			memset(Usart1BufferMemoryRead,0,Usart1BufferMemoryReadLen);
			BufferManageRead(&buff_manage_struct1,Usart1BufferMemoryRead,&buff_manage_struct1.ReadLen);
			if(buff_manage_struct1.ReadLen>0){//������
				
			}
			
			/*�������״̬,��ʱ���*/
			if(SmartConfigFlage){
				if(SmartConfigCnt>5000){
					SmartConfigCnt = 5001;
					
					if(module_net_state == net_status_cloud){
						SmartConfigFlage=0;
						
						//�汾0000
						MainLen = sprintf(MainBuffer,"mcu_version 0000\r\n");
						BufferManageWrite(&buff_manage_struct_net,MainBuffer,MainLen,&Len);
						BufferManageWrite(&buff_manage_struct_net,MainBuffer,MainLen,&Len);
						
						
						if(FlashErasePage(IAPStructValue.RunAPPAddress,FLASH_USER_SIZE)!=4)//�������г����ַ
						{
							//Flash ����ʧ��
							IAPSetUpdateStatus(UpdateStatus_EraseErrRunUpdata);//����������ʧ��(����ʱ)
							printf("\r\nErr:FlashEraseErrRun\r\n");
							Reset_MCU();//����
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
			
			/*ÿ��1S��ȡһ������״̬*/
			if(GetNetTimerCnt>=1000 && SmartConfigFlage)
			{
				GetNetTimerCnt = 0;
				BufferManageWrite(&buff_manage_struct_net,"net\r\n",5,&Len);
				BufferManageWrite(&buff_manage_struct_net,"get_down\r",9,&Len);
			}
			
			
			SmartConfig();//����
			
			if(SmartConfigFlage){
				InitBleFun();
			}
		}
		else
		{	
			IAPLoadAPPProgram();//û�и��µ�״̬�¼����û�����
			IAPDownloadTimeoutFunction();//���س�ʱ
			IAPMainTimeoutFunction();//�������г�ʱ
			
			
			/*��������Ҫ����*/
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
					IAPSetUpdateStatus(UpdateStatus_WriteAppOk);//���ñ�־λΪ 0x01
				}
				delay_ms(100);
				
				
				ReadEOTCnt=0;
				Reset_MCU();
			}
			
			//��ȡ���ڽ��յ�����
			memset(Usart1BufferMemoryRead,0,Usart1BufferMemoryReadLen);
			BufferManageRead(&buff_manage_struct1,Usart1BufferMemoryRead,&buff_manage_struct1.ReadLen);
			if(buff_manage_struct1.ReadLen>0){//������
				
				if(memcmp(Usart1BufferMemoryRead,"down update_fw",14) ==0 ){
					UpdataState = 1;
					IAPStructValue.PutDataFlage = 1;
					IAPStructValue.UpdateFlage=1;
					BufferManageWrite(&buff_manage_struct_t,"result \"ready\"\r",15,&Len);
				}
				else if(UpdataState==1 && memcmp(Usart1BufferMemoryRead,"ok",2) ==0){
					BufferManageWrite(&buff_manage_struct_t,"C",1,&Len);
					UpdataState = 2;
					
					IAPStructValue.RunAPPAddressCnt = IAPStructValue.RunAPPAddress;//���еĵ�ַ
					IAPStructValue.BackupAPPAddressCnt = IAPStructValue.BackupAPPAddress;//���ݵĵ�ַ
				}
				else if(UpdataState==2){//������������
					
					IAPResetDownloadTimeoutFunction();
					
					if(buff_manage_struct1.ReadLen==133){//�����ļ�
						
						//У������ļ�,д��flash
						if(Usart1BufferMemoryRead[0]==SOH &&  Usart1BufferMemoryRead[1]==Bits1 && Bits2==Usart1BufferMemoryRead[2]){
							
							if(check(&Usart1BufferMemoryRead[3],128) ==1){//���CRC�Ƿ���ȷ
								crc16_check_err_cnt = 0;
								MainBuffer[0] = ACK;
								BufferManageWrite(&buff_manage_struct_t,MainBuffer,1,&Len);
								
								Bits1++;
								Bits2--;
								
								if(IAPStructValue.RunAPPAddressCnt< IAPStructValue.BackupAPPAddress)//���ܳ��� ���ݵĵ�ַ
								{
									if(!IAPStructValue.FlashWriteErrFlage)//дFlashû�д���
									{
										for(IAPStructValue.BackupCnt=0;IAPStructValue.BackupCnt<64;IAPStructValue.BackupCnt++){
											
											IAPStructValue.ReadDate = Usart1BufferMemoryRead[IAPStructValue.BackupCnt*2+4];
											IAPStructValue.ReadDate = IAPStructValue.ReadDate<<8;
											IAPStructValue.ReadDate = IAPStructValue.ReadDate|Usart1BufferMemoryRead[IAPStructValue.BackupCnt*2+3];//ƴ������
											
											
											if(WriteFlashHalfWord(IAPStructValue.RunAPPAddressCnt,IAPStructValue.ReadDate) != 0)//дFlash
											{
												IAPStructValue.FlashWriteErrFlage = 1;//дFlash����
											}
											IAPStructValue.RunAPPAddressCnt+=2;//��ַ����
										}
									}
								}
							}
							else{
								crc16_check_err_cnt++;
								
								if(crc16_check_err_cnt>6){
									crc16_check_err_cnt=0;
									
									IAPStructValue.DataCheckErrFlage = 1;
									
									MainBuffer[0] = CAN;//��������
									BufferManageWrite(&buff_manage_struct_t,MainBuffer,1,&Len);
									ReadEOTFlag=1;
								}
							}
						}
						else {//д��flash������ʱ��,��������һ�ε�����(����˵������ƥ�������Ժ�Ĭ��һֱ��ȡ������)
							MainBuffer[0] = ACK;
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,1,&Len);
						}
					}
					else if(buff_manage_struct1.ReadLen==1 && Usart1BufferMemoryRead[0] == EOT){//�����ź�
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
		//�汾0000
		MainLen = sprintf(MainBuffer,"mcu_version 0000\r\n");
		BufferManageWrite(&buff_manage_struct_net,MainBuffer,MainLen,&Len);
		BufferManageWrite(&buff_manage_struct_net,MainBuffer,MainLen,&Len);
		
		//������������
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



