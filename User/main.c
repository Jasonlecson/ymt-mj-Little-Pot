/**
  ******************************************************************************
  * @author  yang feng wu 
  * @version V1.0.0
  * @date    2019/10/12
  * @brief   
	1.上报事件太频繁
	2.肥力检测传感器腐蚀
  ******************************************************************************
	光照	 PA0,Y
	湿度	 PA1.Y
	
	按键   PB0.Y
	光照	 PB6,PB7.Y
	温度	 PA2.Y
	水位	 PB10,PB11
	继电器 PA3
	蜂鸣器 PA4
	灯     PA5.Y
	WiFi   PA9,PA10.Y
	肥力传感器 PBout(1)
  ******************************************************************************
	0023:
	更改: 光照统计报警; 温湿度,光照,水位传感器实时上报; 报警事件3小时统一上报; 无法种植返回 10001
	BUG:浇水时间判断多了一个小时导致9点也浇水
  */
#include "include.h"
#include "IAP.h"
#include "stmflash.h"
#include "BufferManage.h"
#include "gpio.h"
#include "parse.h"
#include "FlashData.h"
#include "ADC.h"
#include "DMA.h"
#include "usart.h"
#include "AHT10_I2C.h"
#include "TM601.h"
#include "iic2.h"
#include "BH1750.h"
#include <time.h>
//#include "pwm.h"
#include "flash_helper.h"

#define define_mcu_version "0016"

/*
1.硬件版本大于等于2支持补光灯
2.硬件版本大于等于6支持电池
3.大于等于2支持查找花盆
*/
u16 HardwareVersion = 0;

u8 mcu_version[5]="";//版本

//缓存数据使用
char MainBuffer[MainBufferLen];//缓存数据,全局通用
u32  MainLen=0;      //全局通用变量
char *MainString;    //全局通用变量
u32  MainLenCopy=0;  //全局通用变量
int  IntValue=0;     //全局通用变量
int  IntLen=0;       //全局通用变量
long long  Valuell;  //全局通用变量
float FloatValue=0;  //全局通用变量

//#define define_get_model "ymt.flowerpot.v1\r"

//#define define_get_down "get_down\r"
//#define define_get_time_posix "time posix\r"
//#define define_get_net "net\r"

#define soil_ec_en 0

//u8 get_model[20]="ymt.flowerpot.v1\r";//产品型号
//u8 get_down[10]="get_down\r";//获取服务器下发的信息
//u8 get_time_posix[15]="time posix\r";//获取时间戳
//u8 get_net[6]="net\r";//获取网络状态

//#define define_set_ble "ble_config set 2692"
//#define define_dump_ble "[\"product id\":2692"

//u8 set_ble[25]="ble_config set 2692";//蓝牙
//u8 dump_ble[25]="[\"product id\":2692";

/**********2020/02/23***********/
//指示灯默认开,水位蜂鸣器预警


char water_stage_empty_flag=0; //缺水
char water_stage_empty_flagCopy=0; //缺水
u16  water_stage_empty_cnt1=0;//缺水
char water_stage_empty_run_flage=0;

char water_stage_water_flag=0; //有水
char water_stage_water_flagCopy=0; //有水
u16  water_stage_water_cnt1=0;//有水


char water_stage_full_flag=0; //水满
char water_stage_full_flagCopy=0; //水满
u16  water_stage_full_cnt1=0;//水满
char water_stage_full_run_flage=0;

char water_stage_nofull_flag=0;
char water_stage_nofull_flagCopy=0;
u16  water_stage_nofull_cnt1=0;

char fill_state = 0;//Fill light state
/*********************/

/**************************************/
u8 InitModelFlag=0;
u16 InitModelCnt=0;
u16 SendModelCnt=0;

u8  InitBleFlag=0;
u16 InitBleCnt=0;
u16 SendBleCnt=0;

u8  InitVersionFlag = 0;
u16 InitVersionCnt=0;
u16 SendVersionCnt=0;
/*****************************************/

/*********************************/
long long matrix=0,matrixCopy=0;//基质时间
u16 love_value=0,love_valueCopy=0;//爱心值
u16 cumulative_harvest=0,cumulative_harvestCopy=0; //累计收获
u16 WateringMode='0',WateringModeCopy='0';//默认自动模式(浇水模式)
u16 WateringStateCopy = 0;//浇水状态
char WateringFlag=0;//在浇水:1
u16  WateringDelayTimer=0;//浇水延时时间
u32  WateringCirculation = 0;//每隔1个小时浇一次水

char find_flower=0;
int LEDState=-1,LEDStateCopy=-1;//LED状态
char fault = 0,faultcopy=0;//故障
u16 water_count=0,water_count_copy=0;//浇水次数,每浇一次累加一次,上报一次
char botany_type[5]={'-','1',0};//植物类型 0-999
char botany_typeCopy[5]={'-','1',0};//植物类型 0-999
char botany_typeCopyCopy[5]={'-','1',0};//植物类型 0-999

int  water_stage=0,water_stageCopy=0;//水位 0-100
u16  water_stageTempCount=0;//水位 0-100

int  fertility=0,fertilityCopy=0;//肥力 0-2000
int  humidity=0,humidityCopy=0;//湿度

int  battery_level=0,battery_level_copy=0;
int  temperature=0,temperatureCopy=0;//温度
int  humidity_air=0,humidity_air_copy=0;//空气湿度

/*检测传感器故障*/
u8  fertilityErrCnt=0,humidityErrCnt=0,sensor_ligntErrCnt=0;

int  DS18B20Cnt=0;
int  sensor_ligntCnt=0;
int  water_stageCnt=0;

int  illumination=0,illuminationCopy=0;//光照
u16  illuminationTempCount=0;//光照
char sensor_lignt_temp[2];//采集的光照传感器的高低位
float sensor_lignt_exponent,sensor_lignt_mantissa;


int  testing_time_appear_timer_cnt=0;//控制温度,湿度,光照,肥力检测上报时间

uint16_t  safe_air_humidity[2],safe_air_humidityCopy[2];//湿度安全值
uint16_t  safe_humidity[2],safe_humidityCopy[2];//湿度安全值
int16_t   safe_temperature[2],safe_temperatureCopy[2];//温度安全值
uint16_t  safe_illumination[2],safe_illuminationCopy[2];//光照安全值
uint16_t  safe_fertility[2],safe_fertilityCopy[2];//肥力安全值

#define   safe_water_l 5
#define   safe_water_H 95

//uint16_t  safe_water[2]={5,95};//水位安全值
/*********************************/


u16 send_get_down_cnt=0;//控制发送get_down时间
data_parse_struct data_parse_struct_t;//解析数据
int data_parse_len=0;//解析数据后获取的字段长度

u16 send_get_net_cnt = 0;//控制发送get_net的时间

/*******/
long long one_start_date=0,one_start_dateCopy=0;//首次种植植物的开始时间
/*******/

char manual_refresh = 0,manual_refreshCopy=0;//获取刷新数据指令:1,上报:水位,温度,湿度,光照,肥力
char manual_refresh_one=0;
char manual_refresh_value=0;//用于米家测试记录值
/************/
long long timestamp=0;//当前时间戳
long long start_date=0,start_dateCopy=0;//种植植物的开始时间
char get_time_stamp_flag=0;//是否获取了时间戳
u32  get_time_stamp_time = 1000;
u32  get_time_stamp_cnt=0; //控制定时获取时间戳
/************/

char module_net_state=0;
char cloud_flage=0;//设备连接上小米服务器
char mcu_version_appear=0;//连接上服务器上报一次版本
char mcu_version_appear1=0;//升级完成直接上报版本

u16  NurtureDay=0,NurtureDayCopy=0;//培育天数

//事件监测时间
#define EventReportTime (1000*60*20)
#define EventReportTime1 (1000*60*60*3)

//#define EventReportTime1 (1000*60)

#define EventReportTime2 (1000*60*60*5)
/*事件***********************/
int EventReportTimeCnt=EventReportTime;
u32 EventReportTimeCnt1=EventReportTime1;
u32 EventReportTimeCnt2=EventReportTime2;
/************************/


/*************BufferManager发送数据缓存管理*****************/
#define  BFbuffLen 1024
#define  BFManagebuffLen 40
u8  BFbuff[BFbuffLen];
u32 BFManagebuff[BFManagebuffLen];
//u8  BFSendBuff[BFbuffLen];
/*************BufferManager*****************/

/*************BufferManager发送数据缓存管理(传感器数据)*****************/
#define  BFsensorbuffLen 500
#define  BFsensorManagebuffLen 20
u8  BFsensorbuff[BFsensorbuffLen];
u32 BFsensorManagebuff[BFsensorManagebuffLen];
//u8  BFsensorSendBuff[BFsensorbuffLen];
/*************BufferManager*****************/


/*************BufferManager发送数据缓存管理*****************/
#define BMCMDbuffLen 100
#define BMCMDManagebuffLen 20
buff_manage_struct buff_manage_struct_cmd;//发送数据缓存管理
u8  BMCMDbuff[BMCMDbuffLen];
u32 BMCMDManagebuff[BMCMDManagebuffLen];
//u8  BMCMDSendBuff[BMCMDbuffLen];
/*************BufferManager*****************/



/*************BufferManager:串口接收数据缓存管理*****************/
u8  BMUsart1ReadBuff[rb_t_usart1_read_buff_len];
/*************BufferManager*****************/

/*************SmartConfig*****************/
char SmartConfigState=0;
char SmartConfigFlage=0;
u32  SmartConfigCnt=0;
u32  KeyPressTime;//按键按下时间

struct tm *lcTime;
time_t startTime;

//0:不报警浇水  1:报警浇水
char ReportEventFlag=0;//液晶是否报警浇水
/***浇水时间范围***/
char watering_timer_start;//开始时间
char watering_timer_stop;//结束时间
// 在  InitValue(); 函数中修改
/*报警事件事件范围*/
char event_timer_start;//开始时间
char event_timer_stop;//结束时间
// 在  InitValue(); 函数中修改

/****光照强度统计时间****/
int  illumination_statistics_test=0;//测试使用
char illumination_statistics_timer_start;//开始时间
char illumination_statistics_timer_stop;//结束时间
// 在  InitValue(); 函数中修改
int illumination_statistics_timer_delay=0;//在定时器里面累加,设置统计时间间隔
u16 illumination_statistics_temp[10]={0};//存储一个时间段采集的光照强度
int illumination_statistics_cnt = 0;
u32 illumination_statistics_sum = 0;//光照统计总量
u16 illumination_statistics_count = 0;//光照统计次数

char fill_light = 0;//补光灯

/**
* @brief   统计光照强度
**/
void illumination_intensity_statistics(void);

void InitInitData(void);

void InitBlue(void);
void NurtureDayFunction(void);
void WateringCirculationFunction(void);
void InitModel(void);
void SmartConfig(void);
void data_collection(void);
void safe_appear(void);
void Watering(void);

void InitData(void);
void processing_data(void);
void properties_changed(void);
void InitValue(void);

int main(void)
{
	u16 data=0;
	SCB->VTOR = FLASH_BASE | 0x5000;
	SysTick->CTRL=0;//关闭定时器中断
	
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);//????????
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);//????????
	USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);//??????
	USART_ITConfig(USART2, USART_IT_IDLE, DISABLE);//??????
	USART_Cmd(USART1, DISABLE);                    
	USART_Cmd(USART2, DISABLE);   
	uart_init(115200);	 //串口初始化为115200
	FlashHelperInit(FLASH_DATA_ADDR_1,FLASH_DATA_ADDR_2,FLASH_DATA_ADDR_3,FLASH_DATA_ADDR_4);
	InitValue();
	
	HardwareVersion = IAPGetHardwareVersion();
	
	data_parse_struct_t.net_status = net_status_offline;
	
	BufferManageCreate(&buff_manage_struct_cmd,  BMCMDbuff, BMCMDbuffLen, BMCMDManagebuff, BMCMDManagebuffLen*4);
	BufferManageCreate(&buff_manage_struct_t,  BFbuff, BFbuffLen, BFManagebuff, BFManagebuffLen*4);
//	BufferManageCreate(&buff_manage_struct_t1, BMUsart1Buff, BMUsart1BuffLen, BMUsart1ManageBuff, BMUsart1ManageBuffLen*4);
	BufferManageCreate(&buff_manage_struct_t2, BFsensorbuff, BFsensorbuffLen, BFsensorManagebuff, BFsensorManagebuffLen*4);
	
	
  NVIC_Configuration();
//	TIM1_PWM_Init(9990,719);//PWM频率=72000000/(719+1)(999+1)=100hz 
//	TIM_SetCompare1(TIM1,9988);//调节占空比,0-719
	AdcGpioConfig();
	ADC_Config();
	
//	printf("11");
	GpioInit();
	DelayInit();
	
	Timer2_Config();
	Timer4_Config();
	AHT10_Init();
	IIC2_Init();
	BH1750_Init();
	delay_ms(100);
	IAPGetUpdateInfo();//获取更新的信息
	delay_ms(100);
	InitData();
	delay_ms(100);
	/**复位模块**/
	PAout(11) = 0;
	
	delay_ms(100);
	PAout(11) = 1;
	delay_ms(3000);
	IWDG_Init(IWDG_Prescaler_256,156*5);
  IAPUpdateDispose();
	
//	while(1){
//		IWDG_Feed();//喂狗
//		IntValue = iic_read_byte(0);
//		if(IntValue!=-2){
//			printf("IntValue=%d",IntValue);
//		}
//	}
//	InitModelFlag=1;
//	module_net_state = net_status_cloud;
	InitVersionFlag=0;
	while(1)
	{
		IWDG_Feed();//喂狗
		InitModel();
		
		SmartConfig();
		
		if(SmartConfigFlage){
			InitBlue();
		}
		
		if(KeySmartConfig[3] == 0)
		{
			data_collection();//采集数据
		}
		
		illumination_intensity_statistics();//统计光照强度
		
		WateringCirculationFunction();//循环浇水
		Watering();//控制浇水5S
		NurtureDayFunction();//培育1天,爱心值累加10
		
		if(uart_send_delay<=0)//可以发送数据
		{
			buff_manage_struct_t.SendLen = BufferManageRead(&buff_manage_struct_t,BMUsart1ReadBuff);
			if(buff_manage_struct_t.SendLen>0)//有数据需要返回
			{
				uart_send_delay = 110;
				
				UsartOutStrIT(BMUsart1ReadBuff,buff_manage_struct_t.SendLen);
			}
			else
			{
				buff_manage_struct_t2.SendLen = BufferManageRead(&buff_manage_struct_t2,BMUsart1ReadBuff);
				if(buff_manage_struct_t2.SendLen>0){
					uart_send_delay = 110;
					UsartOutStrIT(BMUsart1ReadBuff,buff_manage_struct_t2.SendLen);
				}
				else{
					buff_manage_struct_cmd.SendLen = BufferManageRead(&buff_manage_struct_cmd,BMUsart1ReadBuff);
					if(buff_manage_struct_cmd.SendLen>0)//发送询问指令
					{
						uart_send_delay = 110;
						UsartOutStrIT(BMUsart1ReadBuff,buff_manage_struct_cmd.SendLen);
					}
				}
			}
		}
		
		
		/*每隔一段时间询问命令*/
		if(InitModelFlag && InitVersionFlag && KeySmartConfig[3] == 0 && send_get_down_cnt>200){
			send_get_down_cnt=0;
			BufferManageWrite(&buff_manage_struct_cmd,"get_down\r",9);
		}
		
		if(InitModelFlag && InitVersionFlag && KeySmartConfig[3] == 0 && send_get_net_cnt>1000){
			send_get_net_cnt = 0;
			BufferManageWrite(&buff_manage_struct_cmd,"net\r",4);
		}
		
		if(!SmartConfigFlage && InitVersionFlag && InitModelFlag){//不在配网状态
			if(timestamp==0){//没有获取时间戳
				get_time_stamp_time = 1000;
			}
			else{
				get_time_stamp_time = 3600000/2;
			}
			//连接上小米服务器
			if(module_net_state == net_status_cloud){
				if(!mcu_version_appear){
					mcu_version_appear=1;
					MainLen = sprintf(MainBuffer,"mcu_version %s\r",mcu_version);
					BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
					BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
					
					IAPUpdateDispose();
				}
				properties_changed();//状态改变时上报数据
				safe_appear();//上报事件
			}
			
			if(get_time_stamp_cnt>get_time_stamp_time){//获取时间戳
				get_time_stamp_cnt=0;
				BufferManageWrite(&buff_manage_struct_t,"time posix\r",11);
			}
			processing_data();//处理模块返回的数据
		}
	}
}

/**
* @brief   统计光照强度
* @param   None
* @param   None
* @param   None
* @retval  None
* @warning None
* @example 
**/
void illumination_intensity_statistics(void){
	u32 value=0;
	long long endtime;
	time_t time_t_value;
  if(timestamp > 1592113818){//从模块获取到了时间戳
		startTime = timestamp+28800;//加8小时变为北京时间
		lcTime = localtime (&startTime);//解析时间戳
		
		if(illumination_statistics_test >=1000)
		{
			illumination_statistics_test=0;

		}
		
		if(lcTime->tm_hour == illumination_statistics_timer_stop) //到达统计时间
//		if(lcTime->tm_hour == 11 && lcTime->tm_min == 46) //到达统计时间
		{
			if(FlashGetIlluminationState() == 0)
			{
				FlashSetIlluminationState();//设置已经上报标志
				
				if(FlashGetFillLightMode()==0)//自动打开补光灯
				{
					if(PBin(5)==1){
						PBout(5) = 0;//关闭补光灯
						fill_state = 0;
					}
				}
				
				//获取存储的光照强度, 存储的次数, 最后统计数据时的时间戳
				illumination_statistics_sum = FlashGetIlluminationStatistics(&illumination_statistics_count,&endtime);

				FlashSetIlluminationStatistics(0,0,startTime);//清除数据
				//统计的光照强度小于设置的光照强度
				if(FlashGetLightAlarm()!=65535 && illumination_statistics_sum/illumination_statistics_count < FlashGetLightAlarm())
				{
					MainLen = sprintf(MainBuffer,"%s %s %s\r","event_occured","5","12");
					BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
				}
			}
		}
		else
		{	//统计光照强度 (早晚8点)
			if(lcTime->tm_hour >= illumination_statistics_timer_start && lcTime->tm_hour < illumination_statistics_timer_stop)
			{
				if(illumination_statistics_timer_delay>1000*60)//1分钟
				{
					illumination_statistics_timer_delay = 0;
					
					if(illumination_statistics_cnt<10)//采集 10 个数据
					{
						illumination_statistics_temp[illumination_statistics_cnt] = illuminationCopy;
						illumination_statistics_cnt++;
					}
					else//采集了数据
					{
						illumination_statistics_cnt=0;
						
						if(FlashGetIlluminationState() == 1)//清除上报
						{
							FlashClearIlluminationState();
						}
						
						SortMethod(illumination_statistics_temp,10,0);//排序
						
						value = illumination_statistics_temp[9] + illumination_statistics_temp[8] + illumination_statistics_temp[7];
						value = value/3;
						
						/*****如果低于设置的光照阈值,打开补光灯****/
						if(value <= safe_illumination[0]){
							if(FlashGetFillLightMode() == 0){//自动打开补光灯
								PBout(5) = 1;//打开补光灯
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","7","1","0");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","7","1","true");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							MainLen =0 ;
//								printf("自动打开补光灯\r\n");
							}
							else{
//								printf("非自动打开补光灯\r\n");
							}
						}
						else{
							if(FlashGetFillLightMode()==0)//自动打开补光灯
							{
								if(PBin(5)==1){
									PBout(5) = 0;//关闭补光灯
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","7","1","0");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","7","1","false");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							MainLen =0 ;
								}
							}
//							printf("关闭补光灯\r\n");
						}
						
						//获取存储的光照强度, 存储的次数, 最后统计数据时的时间戳
						illumination_statistics_sum = FlashGetIlluminationStatistics(&illumination_statistics_count,&endtime);

						time_t_value = endtime;
						if(localtime(&time_t_value)->tm_hour > localtime(&startTime)->tm_hour)//统计的时间点大于当前时间点
						{
							FlashSetIlluminationStatistics(0,0,startTime);//清除数据
							illumination_statistics_sum=0;
							illumination_statistics_count=0;
						}
						else if(localtime(&time_t_value)->tm_hour == localtime(&startTime)->tm_hour)//小时相同
						{
							if(localtime(&time_t_value)->tm_min >= localtime(&startTime)->tm_min){//统计的分钟数大于当前分钟数
								FlashSetIlluminationStatistics(0,0,startTime);//清除数据
								illumination_statistics_sum=0;
								illumination_statistics_count=0;
							}
						}
						else
						{
							if(startTime - endtime > 86400)//当前时间比统计的时间超过了24小时
							{
								FlashSetIlluminationStatistics(0,0,startTime);//清除数据
								illumination_statistics_sum=0;
								illumination_statistics_count=0;
							}
						}
						illumination_statistics_sum = illumination_statistics_sum + value;//累加上这次的光照强度
						illumination_statistics_count = illumination_statistics_count + 1;//次数加一
						
						FlashSetIlluminationStatistics(illumination_statistics_sum,illumination_statistics_count,startTime);//存储数据
					}
				}
			}	
		}
	}
	else{//没有获取到时间戳(断网)
		
	}
}


/**
* @brief   Watering,控制浇水8S
* @param   None
* @param   None
* @param   None
* @retval  None
* @warning None
* @example 
**/
void Watering(void){
	if(TIM2->CCR4 == 400 && WateringFlag==0){
		WateringFlag=1;
	}
	if(WateringFlag==1){
		if(WateringDelayTimer>5000){//5S
			WateringDelayTimer=0;
			WateringFlag=0;
			TIM2->CCR4 = 0;
		}
	}
}



/**
* @brief   循环浇水
* @param   None
* @param   None
* @param   None
* @retval  None
* @warning None
* @example 
**/
void WateringCirculationFunction(void)
{
	if(WateringCirculation>3600000){//3600000
		WateringCirculation=0;		
				
		if(timestamp > 1592113818){//从模块获取到了时间戳
			startTime = timestamp+28800;//加8小时变为北京时间
			lcTime = localtime (&startTime);//解析时间戳
		}
		else{
			
		}
		//在早8点到晚8点,或者用户设置不限制时间段
		if((lcTime->tm_hour>=watering_timer_start && lcTime->tm_hour<watering_timer_stop) || ReportEventFlag==1){
			if(humidity<safe_humidity[0] && WateringMode == '0' && water_stage>safe_water_l){
				TIM2->CCR4 = 400;
			}
		}
	}
}


/**
* @brief   培育1天,爱心值累加10
* @param   None
* @param   None
* @param   None
* @retval  None
* @warning None
* @example 
**/
void NurtureDayFunction(void)
{
	if(timestamp!=0 && start_date!=0){//有时间
		if(botany_type[0]!='-'){//正在种植
			
			NurtureDay = FlashGetNurtureDay(1);//已经培育的天数
			NurtureDayCopy = (timestamp - start_date)/(3600*24);
//			NurtureDayCopy = (timestamp - start_date)/(20);
			//又过了一天
			if(NurtureDay != NurtureDayCopy){
				NurtureDay = NurtureDayCopy;
				FlashSetNurtureDay(NurtureDay,1);
				love_value=love_value+10;//爱心值累加
				FlashSetLoveValue(love_value,1);//存储爱心值
				
				/*基质每隔1天减一*/
				matrix = FlashGetMatrixData(1);
				if(matrix>0){
					matrix = matrix -1;
					FlashSetMatrixData(matrix,1);
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
	//按下
	if(KeySmartConfig[3] == 1)
	{
		KeyPressTime = KeySmartConfig[5];//获取按钮按下时间
//		KeyPressTime = system_run_time_ms;
		if(KeySmartConfig[5]>=4500)//按下时间大于5S
		{
			KeySmartConfig[3] = 0;//清零以后,只有按键松开再按下的时候才会进入
			if(!SmartConfigFlage)
			{
				SmartConfigFlage = 1;
				SmartConfigCnt=0;
				InitBleFlag = 0;
				
				MainLen = sprintf(MainBuffer,"%s\r","restore");
				BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
				BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
				BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
			}
		}
	}
	
	//松开
	if(KeySmartConfig[4] == 1)
	{
		KeySmartConfig[4] = 0;//清零以后,只有按键按下然后松开的时候才会进入
		
//		//执行的代码
//		if(KeyPressTime>100 && KeyPressTime<2000){
//			if(key_down_count==1)
//			{
//				TIM2->CCR4 = 600;;
//				key_down_count=0;
//			}
//			else if(key_down_count==2)
//			{
//				key_down_count=0;
//			}
//		}
	}
	
	if(key_down_count_copy==1)
	{
		key_down_count_copy=0;
		TIM2->CCR4 = 400;
	}
	else if(key_down_count_copy==2)
	{
		key_down_count_copy=0;
		if(PBin(5)==1){
			PBout(5) = 0;
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","7","1","0");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","7","1","false");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);//关闭补光灯
							MainLen =0 ;
		}
		else
		{
			PBout(5) = 1;
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","7","1","0");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","7","1","true");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							MainLen =0 ;
		}
	}
	
//	if(key_down_count !=0)
//	{
//		if(key_down_time_ms>=2000)
//		{
//			key_down_time_ms=0;
//			if(key_down_count==1)
//			{
//				key_down_count=0;
//				TIM2->CCR4 = 600;;
//			}
//			else if(key_down_count==1)
//			{
//				key_down_count=0;
//				if(PBin(5)==1){
//					PBout(5) = 0;//关闭补光灯
//				}
//				else
//				{
//					PBout(1) = 0;
//				}
//			}
//		}
//	}
	
	/*检测配网状态,超时检测*/
	if(SmartConfigFlage){
		
		if(usart1_idle_flag)
		{
			usart1_idle_flag =0 ;
			MainLen = rbCanRead(&rb_t_usart1_read);
			if(MainLen >0)
			{
				memset(BMUsart1ReadBuff,0,rb_t_usart1_read_buff_len);
				rbRead(&rb_t_usart1_read, BMUsart1ReadBuff, MainLen);
			}
		}
		
		if(SmartConfigCnt>5000){
			if(module_net_state == net_status_cloud || SmartConfigCnt>100000){
				SmartConfigFlage=0;
				InitInitData();
				timestamp=0;
				PAout(5) = 0;
				PAout(6) = 0;
				PAout(2) = 0;
			}
		}
	}
}



/**
* @brief   初始化产品Model
* @param   None
* @param   None
* @param   None
* @retval  None
* @warning None
* @example 
**/
void InitModel(void){
	if(IAPStructValue.UpdateStatusValue!=0 && mcu_version_appear1 ==0)//升级过程序
	{
		mcu_version_appear1 = 1;
		InitModelFlag = 1;
		MainLen = sprintf(MainBuffer,"mcu_version %s\r",mcu_version);
		BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
		BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
	}
	
	if(InitModelFlag && !InitVersionFlag)
	{
		if(SendModelCnt>500){
			SendModelCnt=0;
			MainLen = sprintf(MainBuffer,"mcu_version %s\r",mcu_version);
			BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
		}
		
		if(usart1_idle_flag)
		{
			usart1_idle_flag =0 ;
			MainLen = rbCanRead(&rb_t_usart1_read);
			if(MainLen >0)
			{
				memset(BMUsart1ReadBuff,0,rb_t_usart1_read_buff_len);
				rbRead(&rb_t_usart1_read, BMUsart1ReadBuff, MainLen);
				if(memcmp("ok",BMUsart1ReadBuff,2) == 0){
					InitVersionFlag = 1;
				}
			}
		}
	}
	
	
	if(!InitModelFlag){
		if(SendModelCnt>500){
			SendModelCnt=0;
			MainLen = sprintf(MainBuffer,"model ymt.flowerpot.v3\r");
			BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
		}
		
		if(usart1_idle_flag)
		{
			usart1_idle_flag =0 ;
			MainLen = rbCanRead(&rb_t_usart1_read);
			if(MainLen >0)
			{
				memset(BMUsart1ReadBuff,0,rb_t_usart1_read_buff_len);
				rbRead(&rb_t_usart1_read, BMUsart1ReadBuff, MainLen);
				if(memcmp("ok",BMUsart1ReadBuff,2) == 0){
					InitModelFlag = 1;
				}
			}
		}
	}
}


/**
* @brief   初始化产品Blue
* @param   None
* @param   None
* @param   None
* @retval  None
* @warning None
* @example 
**/
void InitBlue(void){
	if(!InitBleFlag){
		if(InitBleCnt>1000){
			InitBleCnt=0;
			MainLen = sprintf(MainBuffer,"%s %s\r","ble_config set 5531",mcu_version);
			BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
		}
		
		if(SendBleCnt>1500){
			SendBleCnt=0;
			InitBleCnt=0;
			MainLen = sprintf(MainBuffer,"ble_config dump\r");
			BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
		}
		
		
//		if(usart1_idle_flag)
		{
			MainLen = rbCanRead(&rb_t_usart1_read);
			if(MainLen >0)
			{
				memset(BMUsart1ReadBuff,0,rb_t_usart1_read_buff_len);
				rbRead(&rb_t_usart1_read, BMUsart1ReadBuff, MainLen);
				//["product id":5531
				
				if(strstr("\"product id\":5531",(char *)BMUsart1ReadBuff))
				{
					InitBleFlag = 1;
				}
			}
			usart1_idle_flag =0 ;
		}
		
		
		//["product id":2692,"version":2.0.1_0005]
//		memset(BMUsart1ReadBuff,0,BMUsart1BuffLen);
//		buff_manage_struct_t1.SendLen = BufferManageRead(&buff_manage_struct_t1,BMUsart1ReadBuff);//读取模块返回的数据
//		if(buff_manage_struct_t1.SendLen>0){
//			if(memcmp(dump_ble,BMUsart1ReadBuff,18) == 0){
//				InitBleFlag = 1;
//			}
//		}
	}
}



/**
* @brief   采集数据
* @param   None
* @param   None
* @param   None
* @retval  None
* @warning None
* @example 
**/
void data_collection(void){
	int value=0;
	float Value;
	int LostValue = 4;
	
	/*获取土壤湿度*************************/
	Value = Get_Adc_Average(1,5);
	Value = Value/4096*3.3;//PA1(湿度)
	
	humidity = -1;
	if(Value>=2.7){
		humidity = 0;
		humidityErrCnt=0;
	}else if(Value>=1.8){
		humidity = -22*Value+60;
		humidityErrCnt=0;
	}else if(Value>=1.6){
		humidity = -100*Value+200;
		humidityErrCnt=0;
	}else if(Value>=1.5){
		humidity = -200*Value+360;
		humidityErrCnt=0;
	}else if(Value>=1.44){
		humidity = -333*Value+560;
		humidityErrCnt=0;
	}else if(Value>=1.4){
		humidity = -500*Value+800;
		humidityErrCnt=0;
	}
	if(humidity!=-1)
	{
		humidity=humidity*2.8*1.5;
		
		if(humidity>100)humidity=100;
		if(humidity<0)humidity=0;
	}else{
		humidity=0;
	}
	
	if((humidityCopy- humidity >=10 || humidity - humidityCopy>=10))
	{
		humidityCopy = humidity;
		
		if(module_net_state == net_status_cloud)
		{
			MainLen = sprintf(MainBuffer,"%s %s %s %d\r","properties_changed","6","4",humidityCopy);
			BufferManageWrite(&buff_manage_struct_t2,MainBuffer,MainLen);
			MainLen=0;
		}
	}
	
	/*******************************/
	
	//电池电量
	Value = Get_Adc_Average(7,5);
	Value = Value/4096*3.3;
	if(Value>=2.1)
	{
		battery_level=100;
	}
	else if(Value<=1.9)
	{
		battery_level=0;
	}
	else
	{
		battery_level = 500*Value-950;
	}
	
	if(battery_level-battery_level_copy>2 || battery_level_copy - battery_level>2)
	{
		battery_level_copy = battery_level;
//		MainLen = sprintf(MainBuffer,"%s %s %s %d\r","properties_changed","5","12",battery_level);
//		BufferManageWrite(&buff_manage_struct_t2,MainBuffer,MainLen);
//		MainLen=0;
	}
	
	/*获取空气温湿度数据,电池电量************************/
	if(DS18B20Cnt>2000){
		DS18B20Cnt=0;
		read_AHT10_once(); 
//		printf("\r\n");
//		printf("温度:%d",T1);
//		printf("湿度:%d%d.%d",H1/100,(H1/10)%10,H1%10);
//		printf("\r\n");
		
//		value = AHT10ReadData(&humidity_air_copy,&temperatureCopy);
		
//		MainLen = sprintf(MainBuffer,"%s %s %s %d\r","properties_changed","9","9",T1);
//		BufferManageWrite(&buff_manage_struct_t2,MainBuffer,MainLen);
		
//		temperatureCopy = DS18B20_Get_Temp();

		temperatureCopy = T1/10;
		
		if(temperatureCopy == 255){//传感器有问题
//			temperatureCopy = DS18B20_Get_Temp();
//			if(temperatureCopy == 255){//传感器有问题
//				temperatureCopy = DS18B20_Get_Temp();
//			}
		}
		
		
//humidity = humidity + 10;
//		MainLen = sprintf(MainBuffer,"%s %s %s %d\r","properties_changed","6","4",humidity);
//		BufferManageWrite(&buff_manage_struct_t2,MainBuffer,MainLen);
//		MainLen=0;
		
//		BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
		if(temperatureCopy == 255){//传感器有问题
//			temperature = 0;
		}
		else{
			if(temperatureCopy>-50 && temperatureCopy<100)
			{
				if((temperatureCopy- temperature >=1 || temperature - temperatureCopy>=1))
				{
					temperature = temperatureCopy;
					
					if(module_net_state == net_status_cloud)
					{
						MainLen = sprintf(MainBuffer,"%s %s %s %d\r","properties_changed","6","3",temperature);

						BufferManageWrite(&buff_manage_struct_t2,MainBuffer,MainLen);
						MainLen=0;
					}
				}
				
				humidity_air_copy = H1/10;
				if((humidity_air_copy- humidity_air >=1 || humidity_air - humidity_air_copy>=1))
				{
					humidity_air = humidity_air_copy;
					
					if(module_net_state == net_status_cloud)
					{
						MainLen = sprintf(MainBuffer,"%s %s %s %d\r","properties_changed","5","13",humidity_air);
						BufferManageWrite(&buff_manage_struct_t2,MainBuffer,MainLen);
						MainLen=0;
					}
				}
			}
		}
	}
	
	/*光照传感器***************************/
	if(sensor_ligntCnt>1000)
	{
		sensor_ligntCnt=0;
		
		illuminationCopy = illumination_get();
		if(illuminationCopy >=0 && (illuminationCopy- illumination >=5 || illumination - illuminationCopy>=5))
		{
			illumination = illuminationCopy;
			
			if(module_net_state == net_status_cloud)
			{
				MainLen = sprintf(MainBuffer,"%s %s %s %d\r","properties_changed","6","6",illumination);
				BufferManageWrite(&buff_manage_struct_t2,MainBuffer,MainLen);
				MainLen=0;
			}
		}
	}
	
	
	if(humidity>95)
	{
		fertility = 1050;
	}
	else
	{
		fertility = (humidity*humidity+29*humidity)/6+temperature/2+sensor_lignt_temp[1];
	}
	if(humidity<=0)
	{
		fertility = 0;
	}
	
	if(fertility<=0)
	{
		fertility = 0;
	}
	if(fertility>2000){
		fertility=2000;
	}
	
	
	if(water_stageCnt>1500)
	{
		water_stageCnt=0;
		Value = Get_Adc_Average(9,5);
		Value = Value/4096*3.3;
//		MainLen = sprintf(MainBuffer,"%s %s %s %f\r","properties_changed","9","9",Value);
//		BufferManageWrite(&buff_manage_struct_t2,MainBuffer,MainLen);
		
		if(Value >0.8)
		{
			water_stage = 0;
		}
		else
		{	
			water_stage = 100;
		}
		
		if(water_stage != water_stageCopy)
		{
			water_stageCopy = water_stage;
			
			if(module_net_state == net_status_cloud)
			{
				MainLen = sprintf(MainBuffer,"%s %s %s %d\r","properties_changed","3","5",water_stage);

				BufferManageWrite(&buff_manage_struct_t2,MainBuffer,MainLen);
				MainLen=0;
			}
		}
	}
}


/**
* @brief   上报事件
* @param   None
* @param   None
* @param   None
* @retval  None
* @warning None
* @example 
**/
void safe_appear(void){
	
	//肥力高
	if(fertility>safe_fertility[1]){
		event_t.cnt_timer_fertilityH++;
		if(event_t.cnt_timer_fertilityH>event_20minute)
		{
			event_t.cnt_timer_fertilityH=event_20minute;
		}
	}
	else
	{
		event_t.cnt_timer_fertilityH=0;
	}
	
	//肥力低
	if(fertility<safe_fertility[0]){
		event_t.cnt_timer_Lfertility++;
		if(event_t.cnt_timer_Lfertility>event_20minute)
		{
			event_t.cnt_timer_Lfertility=event_20minute;
		}
	}
	else
	{
		event_t.cnt_timer_Lfertility=0;
	}
	
	//湿度高
	if(humidity>safe_humidity[1]){
		event_t.cnt_timer_humidityH++;
		if(event_t.cnt_timer_humidityH>event_20minute)
		{
			event_t.cnt_timer_humidityH=event_20minute;
		}
	}
	else
	{
		event_t.cnt_timer_humidityH=0;
	}
	
	//湿度低
	if(humidity<safe_humidity[0]){
		event_t.cnt_timer_Lhumidity++;
		if(event_t.cnt_timer_Lhumidity>event_20minute)
		{
			event_t.cnt_timer_Lhumidity=event_20minute;
		}
	}
	else
	{
		event_t.cnt_timer_Lhumidity=0;
	}
	
	//温度高
	if(temperature>safe_temperature[1]){
		event_t.cnt_timer_temperatureH++;
		if(event_t.cnt_timer_temperatureH >= event_20minute)
		{
			event_t.cnt_timer_temperatureH=event_20minute;
		}
	}
	else
	{
		event_t.cnt_timer_temperatureH=0;
	}
	
	//温度低
	if(temperature<safe_temperature[0]){
		event_t.cnt_timer_Ltemperature++;
		if(event_t.cnt_timer_Ltemperature >= event_20minute)
		{
			event_t.cnt_timer_Ltemperature=event_20minute;
		}
	}
	else
	{
		event_t.cnt_timer_Ltemperature=0;
	}
	
	//lightheight
	if(illumination>safe_illumination[1]){
		event_t.cnt_timer_illuminationH++;
		if(event_t.cnt_timer_illuminationH >= event_20minute)
		{
			event_t.cnt_timer_illuminationH=event_20minute;
		}
	}
	else
	{
		event_t.cnt_timer_illuminationH=0;
	}
	
	
	//lightlow
	if(illumination<safe_illumination[0]){
		event_t.cnt_timer_Lillumination++;
		if(event_t.cnt_timer_Lillumination >= event_20minute)
		{
			event_t.cnt_timer_Lillumination=event_20minute;
		}
	}
	else
	{
		event_t.cnt_timer_Lillumination=0;
	}
	
	
	if(EventReportTimeCnt1 <=0 && botany_type[0]!='-' )//3小时
	{
		EventReportTimeCnt1=EventReportTime1;
		
		
		if(timestamp > 1592113818){//从模块获取到了时间戳
			startTime = timestamp+28800;//加8小时变为北京时间
			lcTime = localtime (&startTime);//解析时间戳
		}
		else{
			
		}
		
		/*早8点 到 晚 8点才允许报警*/
		if((lcTime->tm_hour>=event_timer_start && lcTime->tm_hour<event_timer_stop) || ReportEventFlag==1){
			
				if(event_t.cnt_timer_humidityH >= event_20minute)//湿度高
				{
					event_t.cnt_timer_humidityH=0;
					MainLen = sprintf(MainBuffer,"%s %s %s\r","event_occured","5","4");
					BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
				}
				
				if(event_t.cnt_timer_Lhumidity >= event_20minute)//湿度低
				{
					event_t.cnt_timer_Lhumidity=0;
					MainLen = sprintf(MainBuffer,"%s %s %s\r","event_occured","5","5");
					BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
				}
				
				if(temperatureCopy != 255)//温度传感器没有故障
				{
					if(event_t.cnt_timer_temperatureH >= event_20minute)//温度高
					{
						event_t.cnt_timer_temperatureH=0;
						MainLen = sprintf(MainBuffer,"%s %s %s\r","event_occured","5","6");
						BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
					}

					if(event_t.cnt_timer_Ltemperature >= event_20minute)//温度低
					{
						event_t.cnt_timer_Ltemperature=0;
						MainLen = sprintf(MainBuffer,"%s %s %s\r","event_occured","5","7");
						BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
					}
				}
				

				if(event_t.cnt_timer_illuminationH >= event_20minute)//光照高
				{
					event_t.cnt_timer_illuminationH=0;
					MainLen = sprintf(MainBuffer,"%s %s %s\r","event_occured","5","8");
					BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
				}
				
				if(event_t.cnt_timer_Lillumination >= event_20minute)//光照低
				{
					event_t.cnt_timer_Lillumination=0;
					MainLen = sprintf(MainBuffer,"%s %s %s\r","event_occured","5","9");
					BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
				}
				
				if(event_t.cnt_timer_fertilityH>event_20minute)//肥力高
				{
					event_t.cnt_timer_fertilityH=0;
					MainLen = sprintf(MainBuffer,"%s %s %s\r","event_occured","5","2");
					BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
				}
				
				if(event_t.cnt_timer_Lfertility>event_20minute)//肥力低
				{
					event_t.cnt_timer_Lfertility=0;
					MainLen = sprintf(MainBuffer,"%s %s %s\r","event_occured","5","3");
					BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
				}
				
				
				//缺水
				if(water_stage<=0){
					MainLen = sprintf(MainBuffer,"%s %s %s\r","event_occured","5","1");
					BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
				}
				//水位中等偏低
				if(water_stage>10 && water_stage<20){
					MainLen = sprintf(MainBuffer,"%s %s %s\r","event_occured","5","14");
					BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
				}
				
				//水位偏低
				if(water_stage>5 && water_stage<10){
					MainLen = sprintf(MainBuffer,"%s %s %s\r","event_occured","5","13");
					BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
				}
				
				//水位低
				if(water_stage<safe_water_l && water_stage >0){
					MainLen = sprintf(MainBuffer,"%s %s %s\r","event_occured","5","11");
					BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
				}
		}
	}
}



/**
* @brief   处理模块返回的数据
* @param   None
* @param   None
* @param   None
* @retval  None
* @warning None
* @example 
**/

void processing_data(void)
{
	int i=0,j=0;
	
	if(usart1_idle_flag)
	{
		usart1_idle_flag =0 ;
		MainLen = rbCanRead(&rb_t_usart1_read);
	}
	
	if(MainLen>0)
	{
		memset(BMUsart1ReadBuff,0,rb_t_usart1_read_buff_len);
		rbRead(&rb_t_usart1_read, BMUsart1ReadBuff, MainLen);
		
		if(BMUsart1ReadBuff[MainLen-1] == '\r'){BMUsart1ReadBuff[MainLen-1] = 0;}
		
		data_parse_struct_t.message_type=0;
		
		data_parse_len = data_parse(&data_parse_struct_t,BMUsart1ReadBuff);//提取数据
		MainLen = 0;
		
		if(data_parse_struct_t.message_type == set_properties)//设置属性
		{
			if(data_parse_len<3){
				
				if(data_parse_len==2){
					MainLenCopy = sprintf(MainBuffer,"%s %s %s %s ","result",data_parse_struct_t.siid[0],data_parse_struct_t.piid[0],"0");
					MainLen = MainLen+MainLenCopy;
					MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s\r",data_parse_struct_t.siid[1],data_parse_struct_t.piid[1],"0");
					MainLen = MainLen+MainLenCopy;
					if(MainLen>0)
					{
						BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
						MainLen=0;
					}
				}
				
				
				for(i=0;i<data_parse_len;i++){
					if(i>=1){
						j=1;
						memset(data_parse_struct_t.siid[0],0,MAXDataLen);
						memset(data_parse_struct_t.piid[0],0,MAXDataLen);
						memset(data_parse_struct_t.value[0],0,MAXDataLen);
						
						memcpy(data_parse_struct_t.siid[0],data_parse_struct_t.siid[i],MAXDataLen);//获取siid
						memcpy(data_parse_struct_t.piid[0],data_parse_struct_t.piid[i],MAXDataLen);//获取piid
						memcpy(data_parse_struct_t.value[0],data_parse_struct_t.value[i],MAXDataLen);//获取value
					}
					
					
			
//			if(data_parse_len == 1 || j==1)//单个属性
			{
				j=0;
				//设置手动自动浇水模式   浇水/停止浇水
				if(memcmp(data_parse_struct_t.siid[0],"2",1)==0 )
				{
					if( memcmp(data_parse_struct_t.piid[0],"1",1)==0 )//设置手动自动浇水模式
					{
						if(memcmp(data_parse_struct_t.value[0],"0",1)==0)//自动浇水
						{
							WateringMode = '0';
							WateringModeCopy = '1';
							FlashSetModeAuto();//设置为自动浇水
							MainLen = sprintf(MainBuffer,"%s 2 1 %s\r","result","0");
						}
						else if(memcmp(data_parse_struct_t.value[0],"1",1)==0)
						{
							WateringMode = '1';
							WateringModeCopy = '0';
							FlashSetModeManual();//设置为手动浇水
							MainLen = sprintf(MainBuffer,"%s 2 1 %s\r","result","0");
						}
						else{
							MainLen = sprintf(MainBuffer,"%s 2 1 %s\r","result","-4005");
						}
					}
					else if( memcmp(data_parse_struct_t.piid[0],"2",1)==0 )//浇水
					{
						if( memcmp(data_parse_struct_t.value[0],"true",4)==0)//设置浇水
						{
//							if(water_stage>safe_water_l){ //有水
								
//								if(humidity<safe_humidity[1])
//								{
									TIM2->CCR4 = 400;
									
									if(TIM2->CCR4 == 400)
									{
									  //water_count++;//浇水次数加一
										MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","2","2","0");
										BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
										MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","2","2","true");
										BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
										MainLen =0 ;
									}
									else
									{
										MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","2","2","-4004");
									}
//								}
//							}
//							else{
//								MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","2","2","-4004");
//							}
						}
						else if(memcmp(data_parse_struct_t.value[0],"false",5)==0)//关闭浇水
						{
							TIM2->CCR4 = 0;
							if(TIM2->CCR4 == 0)
							{
								MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","2","2","0");
								BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
								MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","2","2","false");
								BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
								MainLen =0 ;
							}
							else
							{
								MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","2","2","-4004");
							}
						}
					}
					else if( memcmp(data_parse_struct_t.piid[0],"4",1)==0 )//开关机
					{
						if( memcmp(data_parse_struct_t.value[0],"true",4)==0)
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","2","4","0");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);

							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","2","4","true");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							MainLen =0 ;
						}
						else
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","2","4","0");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);

							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","2","4","false");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							MainLen =0 ;
						}
					}
				}
				//设置植物类型
				else if(memcmp(data_parse_struct_t.siid[0],"3",1)==0)
				{
					if(memcmp(data_parse_struct_t.piid[0],"7",1)==0)//设置植物类型
					{
						if(timestamp!=0)//需要保证有时间戳
						{
							memset(botany_typeCopyCopy,0,5);
							sprintf(botany_typeCopyCopy,"%s",data_parse_struct_t.value[0]);//接收植物类型
							if(botany_typeCopyCopy[0]!=0)
							{
								/*清除植物类型*/
								if(botany_typeCopyCopy[0] == '-')
								{
									if(botany_typeCopyCopy[1] == '1')
									{
										memcpy(botany_type,botany_typeCopyCopy,5);
										memcpy(botany_typeCopy,botany_typeCopyCopy,5);
										
										FlashSetBotanyType(botany_type);//Flash存储植物类型
										
										MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","3","7","0");
										BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
										MainLen=0;
										
										MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","3","7",botany_type);
										BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
										MainLen=0;
										
										cumulative_harvest++;//累加收获加一
										FlashSetHarvestCount(cumulative_harvest);//设置累计收获

										love_value=love_value+20;//爱心值累加
										FlashSetLoveValue(love_value,1);//存储爱心值

									}
								}
								else
								{
									//种植的时候需要是未种植状态
									if(botany_typeCopy[0]=='-')
									{
										if(1)
										{
											love_value=love_value+20;//爱心值累加
											FlashSetLoveValue(love_value,1);//存储爱心值

											start_date = timestamp;
											FlashSetDataPlant(start_date);//flash存储种植时间

											memcpy(botany_type,botany_typeCopyCopy,5);
											FlashSetBotanyType(botany_type);//Flash存储植物类型
											
											
											MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","3","7","0");
											BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
											
											
											if(atoi(botany_type)>=19){//植物类型大于19
												MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","3","7","19");
												BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
											}else{
												MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","3","7",botany_type);
												BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
											}
											MainLen=0;
											
											if(one_start_date==0)
											{
												one_start_date = timestamp;
												FlashSetOneDataPlant(one_start_date);
												one_start_dateCopy=0;
												MainLen = sprintf(MainBuffer,"%s %s %s \"%lld\"\r","properties_changed","5","7",one_start_date);	
												BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
											}
											
											MainLen = sprintf(MainBuffer,"%s %s %s \"%lld\"\r","properties_changed","5","2",start_date);
											BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
											MainLen=0;

											MainLenCopy=0;
											MainLen=0;

											MainLenCopy = sprintf(MainBuffer,"%s %s %s %d ","properties_changed","3","4",fertility);
											MainLen =  MainLen + MainLenCopy;
											
											if(soil_ec_en){
												MainLenCopy = sprintf(MainBuffer,"%s %s %d ","6","7",fertility);
												MainLen =  MainLen + MainLenCopy;
											}
											

											MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %d ","6","4",humidity);
											MainLen =  MainLen + MainLenCopy;

											MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %d ","6","3",temperature);
											MainLen =  MainLen + MainLenCopy;

											MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %d ","6","6",illumination);
											MainLen =  MainLen + MainLenCopy;

											MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %d\r","3","5",water_stage);
											MainLen =  MainLen + MainLenCopy;

											BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
											MainLen=0;
										}
									}
									else //已经种植
									{
										MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","3","7","10001");
									}
								}
							}
							else
							{
								MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","3","7","-4005");
							}	
						}
						else//内部还没有获取时间戳
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","3","7","-4004");
						}
					}
					else if(memcmp(data_parse_struct_t.piid[0],"8",1)==0)//设置植物类型
					{
						if(timestamp!=0)//需要保证有时间戳
						{
							memset(botany_typeCopyCopy,0,5);
							sprintf(botany_typeCopyCopy,"%s",data_parse_struct_t.value[0]);//接收植物类型
							if(botany_typeCopyCopy[0]!=0)
							{
								/*清除植物类型*/
								if(botany_typeCopyCopy[0] == '-')
								{
									if(botany_typeCopyCopy[1] == '1')
									{
										memcpy(botany_type,botany_typeCopyCopy,5);
										memcpy(botany_typeCopy,botany_typeCopyCopy,5);
										
										FlashSetBotanyType(botany_type);//Flash存储植物类型
										
										MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","3","8","0");
										BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
										
										MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","3","8",botany_type);
										BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
										
										MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","3","7","0");
										BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
										
										MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","3","7",botany_type);
										BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
										MainLen=0;
										
										cumulative_harvest++;//累加收获加一
										FlashSetHarvestCount(cumulative_harvest);//设置累计收获

										love_value=love_value+20;//爱心值累加
										FlashSetLoveValue(love_value,1);//存储爱心值

									}
								}
								else
								{
									//种植的时候需要是未种植状态
									if(botany_typeCopy[0]=='-')//查看本地是否是未种植状态
									{
										if(1)
										{
											love_value=love_value+20;//爱心值累加
											FlashSetLoveValue(love_value,1);//存储爱心值

											start_date = timestamp;
											FlashSetDataPlant(start_date);//flash存储种植时间

											memcpy(botany_type,botany_typeCopyCopy,5);
											FlashSetBotanyType(botany_type);//Flash存储植物类型
											
											MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","3","8","0");
											BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
											
											MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","3","8",botany_type);
											BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
											
											MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","3","7","0");
											BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
											
											if(atoi(botany_type)>=19){//植物类型大于19
												MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","3","7","19");
												BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
											}else{
												MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","3","7",botany_type);
												BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
											}
											MainLen=0;
											
											if(one_start_date==0)
											{
												one_start_date = timestamp;
												FlashSetOneDataPlant(one_start_date);
												one_start_dateCopy=0;
												MainLen = sprintf(MainBuffer,"%s %s %s \"%lld\"\r","properties_changed","5","7",one_start_date);	
												BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
											}
											
											MainLen = sprintf(MainBuffer,"%s %s %s \"%lld\"\r","properties_changed","5","2",start_date);
											BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
											MainLen=0;

											MainLenCopy=0;
											MainLen=0;

											MainLenCopy = sprintf(MainBuffer,"%s %s %s %d ","properties_changed","3","4",fertility);
											MainLen =  MainLen + MainLenCopy;
											
											if(soil_ec_en){
												MainLenCopy = sprintf(MainBuffer,"%s %s %d ","6","7",fertility);
												MainLen =  MainLen + MainLenCopy;
											}
											

											MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %d ","6","4",humidity);
											MainLen =  MainLen + MainLenCopy;

											MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %d ","6","3",temperature);
											MainLen =  MainLen + MainLenCopy;

											MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %d ","6","6",illumination);
											MainLen =  MainLen + MainLenCopy;

											MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %d\r","3","5",water_stage);
											MainLen =  MainLen + MainLenCopy;

											BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
											MainLen=0;
										}
									}
									else //已经种植
									{
										MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","3","8","10001");
									}
								}
							}
							else
							{
								MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","3","8","-4005");
							}	
						}
						else//内部还没有获取时间戳
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","3","8","-4004");
						}
					}
				}
				//设置安全阈值  down set_properties 4 1 "10,400"
				else if( memcmp(data_parse_struct_t.siid[0],"4",1)==0)
				{
					if(memcmp(data_parse_struct_t.piid[0],"1",1)==0)//safe_temperature:-30 100
					{
						MainLenCopy = MinMaxParse(&data_parse_struct_t,data_parse_struct_t.value[0]);
						if(MainLenCopy==2){
							//检测范围
							if(data_parse_struct_t.MinValue>=-30 && data_parse_struct_t.MinValue<=100 &&
								data_parse_struct_t.MaxValue>=-30 && data_parse_struct_t.MaxValue<=100	){
								
								safe_temperature[0] = data_parse_struct_t.MinValue;
								safe_temperature[1] = data_parse_struct_t.MaxValue;
								safe_temperatureCopy[0] = data_parse_struct_t.MinValue-1;
								safe_temperatureCopy[1] = data_parse_struct_t.MaxValue-1;
								FlashSetSafe(safe_temperature[0],safe_temperature[1],safe_type_temperature);
								MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","4","1","0");
							}
							else{
								MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","4","1","-4005");
							}
						}
					}
					else if(memcmp(data_parse_struct_t.piid[0],"2",1)==0)//safe_humidity
					{
						MainLenCopy = MinMaxParse(&data_parse_struct_t,data_parse_struct_t.value[0]);
						if(MainLenCopy==2){
							safe_humidity[0] = data_parse_struct_t.MinValue;
							safe_humidity[1] = data_parse_struct_t.MaxValue;
							safe_humidityCopy[0] = data_parse_struct_t.MinValue-1;
							safe_humidityCopy[1] = data_parse_struct_t.MaxValue-1;
							
							FlashSetSafe(safe_humidity[0],safe_humidity[1],safe_type_humidity);
							
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","4","2","0");
						}
						else{
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","4","2","-4005");
						}
					}
					else if(memcmp(data_parse_struct_t.piid[0],"3",1)==0)//safe_illumination
					{
						MainLenCopy = MinMaxParse(&data_parse_struct_t,data_parse_struct_t.value[0]);
						if(MainLenCopy==2){
							safe_illumination[0] = data_parse_struct_t.MinValue;
							safe_illumination[1] = data_parse_struct_t.MaxValue;
							safe_illuminationCopy[0] = data_parse_struct_t.MinValue-1;
							safe_illuminationCopy[1] = data_parse_struct_t.MaxValue-1;
							
							FlashSetSafe(safe_illumination[0],safe_illumination[1],safe_type_illumination);
							
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","4","3","0");
						}else{
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","4","3","-4005");
						}
					}
					else if(memcmp(data_parse_struct_t.piid[0],"4",1)==0)//safe_fertility
					{
						MainLenCopy = MinMaxParse(&data_parse_struct_t,data_parse_struct_t.value[0]);
						if(MainLenCopy==2){
							safe_fertility[0] = data_parse_struct_t.MinValue;
							safe_fertility[1] = data_parse_struct_t.MaxValue;
							safe_fertilityCopy[0] = data_parse_struct_t.MinValue-1;
							safe_fertilityCopy[1] = data_parse_struct_t.MaxValue-1;
							
							FlashSetSafe(safe_fertility[0],safe_fertility[1],safe_type_fertility);
							
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","4","4","0");
						}else{
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","4","4","-4005");
						}
					}
					else if(memcmp(data_parse_struct_t.piid[0],"5",1)==0)//safe_air_humidity
					{
						MainLenCopy = MinMaxParse(&data_parse_struct_t,data_parse_struct_t.value[0]);
						if(MainLenCopy==2){
							safe_air_humidity[0] = data_parse_struct_t.MinValue;
							safe_air_humidity[1] = data_parse_struct_t.MaxValue;
							safe_air_humidityCopy[0] = data_parse_struct_t.MinValue-1;
							safe_air_humidityCopy[1] = data_parse_struct_t.MaxValue-1;
							
							FlashSetSafe(safe_air_humidity[0],safe_air_humidity[1],safe_type_air_humidity);
							
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","4","5","0");
						}
						else{
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","4","2","-4005");
						}
					}
				}
				//开灯/关灯 ,基质时间 浇水次数 累计收获 首次种植植物的时间
				else if( memcmp(data_parse_struct_t.siid[0],"5",1)==0)
				{
					if(strlen(data_parse_struct_t.piid[0]) == 1 && memcmp(data_parse_struct_t.piid[0],"1",1)==0)
					{
						if(memcmp(data_parse_struct_t.value[0],"true",4)==0)
						{
							PAout(5) = 0;
							PAout(6) = 0;
							PAout(2) = 0;
							if(PAin(5)==0)
							{
								LEDState =1;
								MainLen = sprintf(MainBuffer,"%s","result 5 1 0\r");
								BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
								
								MainLen = sprintf(MainBuffer,"%s","properties_changed 5 1 true\r");
								BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
								MainLen=0;
							}
							else{
								MainLen = sprintf(MainBuffer,"%s 5 1 %s\r","result","-4004");
							}
						}
						else if(memcmp(data_parse_struct_t.value[0],"false",5)==0)
						{

							PAout(5) = 1;
							PAout(6) = 1;
							PAout(2) = 1;
							if(PAin(5))
							{
								LEDState =0;
								MainLen = sprintf(MainBuffer,"%s","result 5 1 0\r");
								BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
								
								MainLen = sprintf(MainBuffer,"%s","properties_changed 5 1 false\r");
								BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
								MainLen=0;
							}
							else{
								MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","5","1","-4004");
							}
						}
					}
					else if(memcmp(data_parse_struct_t.piid[0],"3",1)==0)//基质时间
					{
						MainLen = strlen(data_parse_struct_t.value[0]);
						IntValue = atoi(data_parse_struct_t.value[0]);
						if(MainLen !=0 && MainLen<=5 && IntValue!=0){
							matrix = IntValue;
							matrixCopy=0;
							FlashSetMatrixData(matrix,1);

							love_value=love_value+200;//爱心值累加
							FlashSetLoveValue(love_value,1);//存储爱心值
							
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","5","3","0");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);

							MainLen = sprintf(MainBuffer,"%s %s %s %lld\r","properties_changed","5","3",matrix);
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							MainLen=0;
						}
						else{
							if(MainLen==1 && IntValue==0 && data_parse_struct_t.value[0][0]=='0'){
								matrix = IntValue;
								matrixCopy=0;
								FlashSetMatrixData(matrix,1);

								love_value=love_value+200;//爱心值累加
								FlashSetLoveValue(love_value,1);//存储爱心值

								MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","5","3","0");
								BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);

								MainLen = sprintf(MainBuffer,"%s %s %s %lld\r","properties_changed","5","3",matrix);
								BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
								MainLen=0;
							}else{
								MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","5","3","-4005");
							}
						}
					}
					else if(memcmp(data_parse_struct_t.piid[0],"5",1)==0)//浇水次数
					{
						MainLen = strlen(data_parse_struct_t.value[0]);
						IntValue = atoi(data_parse_struct_t.value[0]);
						if(MainLen !=0 && MainLen<=5 && IntValue!=0){
							water_count = IntValue;
							FlashSetWaterCount(water_count,1);//存储浇水次数
							MainLen = sprintf(MainBuffer,"%s","result 5 5 0\r");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
								
							MainLen = sprintf(MainBuffer,"properties_changed 5 5 %d\r",water_count);
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							MainLen=0;
						}
						else{
							if(MainLen==1 && IntValue==0 && data_parse_struct_t.value[0][0]=='0'){
								water_count = IntValue;
								FlashSetWaterCount(water_count,1);//存储浇水次数
								MainLen = sprintf(MainBuffer,"%s","result 5 5 0\r");
								BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
								
								MainLen = sprintf(MainBuffer,"properties_changed 5 5 %d\r",water_count);
								BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
								MainLen=0;
							}else{
								MainLen = sprintf(MainBuffer,"%s 5 5 %s\r","result","-4005");
							}
						}
					}
					else if(memcmp(data_parse_struct_t.piid[0],"6",1)==0)//累计收获
					{
						MainLen = strlen(data_parse_struct_t.value[0]);
						IntValue = atoi(data_parse_struct_t.value[0]);
						
						if(MainLen !=0 && MainLen<=5 && IntValue!=0){
							cumulative_harvest = IntValue;
							FlashSetHarvestCount(cumulative_harvest);
							MainLen = sprintf(MainBuffer,"%s 5 6 %s\r","result","0");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
								
							MainLen = sprintf(MainBuffer,"%s 5 6 %d\r","properties_changed",cumulative_harvest);
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							MainLen=0;
						}
						else{
							if(MainLen==1 && IntValue==0 && data_parse_struct_t.value[0][0]=='0'){
								cumulative_harvest = IntValue;
								FlashSetHarvestCount(cumulative_harvest);
								MainLen = sprintf(MainBuffer,"%s 5 6 %s\r","result","0");
								BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
								
								MainLen = sprintf(MainBuffer,"%s 5 6 %d\r","properties_changed",cumulative_harvest);
								BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
								MainLen=0;
							}else{
								MainLen = sprintf(MainBuffer,"%s 5 6 %s\r","result","-4005");
							}
						}
					}
					else if(memcmp(data_parse_struct_t.piid[0],"7",1)==0)//首次种植植物的时间
					{
						MainLen = strlen(data_parse_struct_t.value[0]);
						
						if(MainLen == 12 && data_parse_struct_t.value[0][0]=='\"' && data_parse_struct_t.value[0][11]=='\"'){//时间戳
							data_parse_struct_t.value[0][11]=0;
							Valuell = atoi(&data_parse_struct_t.value[0][1]);
							if(Valuell != 0){
								one_start_date = Valuell;
								FlashSetOneDataPlant(one_start_date);
								one_start_dateCopy=0;
								MainLen = sprintf(MainBuffer,"%s 5 7 %s\r","result","0");
								BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
								MainLen = sprintf(MainBuffer,"%s %s %s \"%lld\"\r","properties_changed","5","7",one_start_date);	
							}
							else{
								MainLen = sprintf(MainBuffer,"%s 5 7 %s\r","result","-4004");
							}
						}
						else{
							MainLen = sprintf(MainBuffer,"%s 5 7 %s\r","result","-4005");
						}
					}
					else if(memcmp(data_parse_struct_t.piid[0],"8",1)==0)//刷新数据指令
					{
						if(memcmp(data_parse_struct_t.value[0],"true",4)==0)
						{
							manual_refresh = 1;
							manual_refreshCopy = 0;
							manual_refresh_value = 1;
							fertility_control_cnt = ((1000*60*20)+10000);
							
							MainLen = sprintf(MainBuffer,"result 5 8 0\r");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							
							
							MainLenCopy=0;
							MainLen = sprintf(MainBuffer,"properties_changed 5 8 true ");
							MainLen =  MainLen + MainLenCopy;
							
							MainLenCopy = sprintf(MainBuffer+MainLen,"3 4 %d ",humidity);
							MainLen =  MainLen + MainLenCopy;
							
							if(soil_ec_en){
								MainLenCopy = sprintf(MainBuffer+MainLen,"6 7 %d ",humidity);
								MainLen =  MainLen + MainLenCopy;
							}
							
							MainLenCopy = sprintf(MainBuffer+MainLen,"6 4 %d ",humidity);
							MainLen =  MainLen + MainLenCopy;
							
							MainLenCopy = sprintf(MainBuffer+MainLen,"6 3 %d ",temperature);
							MainLen =  MainLen + MainLenCopy;
							
							MainLenCopy = sprintf(MainBuffer+MainLen,"6 6 %d ",illumination);
							MainLen =  MainLen + MainLenCopy;
							
							MainLenCopy = sprintf(MainBuffer+MainLen,"3 5 %d \r",water_stage);
							MainLen =  MainLen + MainLenCopy;
							//电池电量
							MainLenCopy = sprintf(MainBuffer+MainLen,"5 12 %d \r",battery_level_copy);
							MainLen =  MainLen + MainLenCopy;
							//空气湿度
							MainLenCopy = sprintf(MainBuffer+MainLen,"5 13 %d\r",humidity_air_copy);
							MainLen =  MainLen + MainLenCopy;
							
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							
							
							MainLen = sprintf(MainBuffer,"properties_changed 5 8 true\r");							
						}
						else if(memcmp(data_parse_struct_t.value[0],"false",5)==0)
						{
							manual_refresh_value = 0;
							MainLen = sprintf(MainBuffer,"result 5 8 0\r");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							MainLen = sprintf(MainBuffer,"properties_changed 5 8 false\r");	
						}
					}
					else if(memcmp(data_parse_struct_t.piid[0],"9",1)==0)/*控制在休息时间段是否报警*/
					{
						if(memcmp(data_parse_struct_t.value[0],"true",4)==0)//夜间不报警浇水
						{
							FlashSetReportEvent(0);//夜间不报警浇水
							ReportEventFlag = 0;
							MainLen = sprintf(MainBuffer,"result 5 9 0\r");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							
							MainLen = sprintf(MainBuffer,"properties_changed 5 9 true\r");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
						}
						else if(memcmp(data_parse_struct_t.value[0],"false",5)==0)
						{
							FlashSetReportEvent(1);
							ReportEventFlag=1;
							MainLen = sprintf(MainBuffer,"result 5 9 0\r");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							
							MainLen = sprintf(MainBuffer,"properties_changed 5 9 false\r");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
						}
					}
					else if(memcmp(data_parse_struct_t.piid[0],"10",2)==0)//设置光照不足参数
					{
						MainLen = strlen(data_parse_struct_t.value[0]);
						IntValue = atoi(data_parse_struct_t.value[0]);
						
						if(MainLen !=0 && IntValue!=0 && IntValue>=-60000 && IntValue<=60000){
							
							FlashSetLightAlarm(IntValue);
							
							MainLen = sprintf(MainBuffer,"result 5 10 0\r");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
								
							MainLen = sprintf(MainBuffer,"properties_changed 5 10 %d\r",IntValue);
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							MainLen=0;
						}
						else{
							if(MainLen==1 && IntValue==0 && data_parse_struct_t.value[0][0]=='0'){
								FlashSetLightAlarm(IntValue);
								
								MainLen = sprintf(MainBuffer,"result 5 10 0\r");
								BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
									
								MainLen = sprintf(MainBuffer,"properties_changed 5 10 %d\r",IntValue);
								BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
								MainLen=0;
							}else{
								MainLen = sprintf(MainBuffer,"result 5 10 %d\r",60000);
							}
						}
					}
					// 5 11 补光灯
					else if(memcmp(data_parse_struct_t.piid[0],"11",2)==0){
						if(memcmp(data_parse_struct_t.value[0],"true",4)==0)//打开
						{
							fill_light = 1;
							MainLen = sprintf(MainBuffer,"%s","result 5 11 0\r");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							
							MainLen = sprintf(MainBuffer,"%s","properties_changed 5 11 true\r");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
						}
						else if(memcmp(data_parse_struct_t.value[0],"false",5)==0)
						{
							fill_light = 0;
							MainLen = sprintf(MainBuffer,"%s","result 5 11 0\r");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							
							MainLen = sprintf(MainBuffer,"%s","properties_changed 5 11 false\r");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
						}
					}
				}
				//补光灯
				else if(memcmp(data_parse_struct_t.siid[0],"7",1)==0 )
				{
					if( memcmp(data_parse_struct_t.piid[0],"2",1)==0 )//设置补光灯是不是自动打开
					{
						if(memcmp(data_parse_struct_t.value[0],"0",1)==0)//自动
						{
							FlashSetFillLightMode(0);
							
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","7","2","0");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);

							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","7","2","0");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							MainLen =0 ;

						}
						else if(memcmp(data_parse_struct_t.value[0],"1",1)==0)
						{
							FlashSetFillLightMode(1);
							
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","7","2","0");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);

							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","7","2","1");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							MainLen =0 ;
						}
						else{
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","7","2","-4005");
						}
					}
					else if( memcmp(data_parse_struct_t.piid[0],"1",1)==0 )//控制补光灯打开或者关闭
					{
						if( memcmp(data_parse_struct_t.value[0],"true",4)==0)//打开
						{
							PBout(5) = 1;
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","7","1","0");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","7","1","true");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							MainLen =0 ;
						}
						else if(memcmp(data_parse_struct_t.value[0],"false",5)==0)//关闭浇水
						{
							PBout(5) = 0;
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","7","1","0");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","7","1","false");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							MainLen =0 ;
						}
					}
				}
				//查找花盆
				else if(memcmp(data_parse_struct_t.siid[0],"8",1)==0 )
				{
					if(memcmp(data_parse_struct_t.piid[0],"1",1)==0 )
					{
						if(memcmp(data_parse_struct_t.value[0],"true",4)==0)//查找花盆
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","8","1","0");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","8","1","true");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							MainLen =0;
						}
						else
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","8","1","0");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","8","1","false");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							MainLen =0;
						}
					}
					else if(memcmp(data_parse_struct_t.piid[0],"2",1)==0 )
					{
						if(memcmp(data_parse_struct_t.value[0],"true",4)==0)//查找花盆
						{
							find_flower=1;
							PAout(4) = 1;
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","8","2","0");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","8","2","true");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							MainLen =0;
						}
						else
						{
							find_flower=0;
							PAout(4) = 0;
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","8","2","0");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","8","2","false");
							BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
							MainLen =0;
						}
					}
				}
			}
			
			if(MainLen>0)
			{
				BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
				MainLen=0;
			}
			
			  }
			}
			else if(data_parse_len >=3){//设置多个属性
				if(memcmp(data_parse_struct_t.siid[0],"4",1)==0 && memcmp(data_parse_struct_t.piid[0],"1",1)==0 &&
					 memcmp(data_parse_struct_t.siid[1],"4",1)==0 && memcmp(data_parse_struct_t.piid[1],"2",1)==0 &&
					 memcmp(data_parse_struct_t.siid[2],"4",1)==0 && memcmp(data_parse_struct_t.piid[2],"3",1)==0 &&
					 memcmp(data_parse_struct_t.siid[3],"4",1)==0 && memcmp(data_parse_struct_t.piid[3],"4",1)==0)
				{
					MainLen = 0;
					
					//safe_type_temperature
					MainLenCopy = MinMaxParse(&data_parse_struct_t,data_parse_struct_t.value[0]);
					if(MainLenCopy==2){
						//检测范围
						if(data_parse_struct_t.MinValue>=-30 && data_parse_struct_t.MinValue<=100 &&
							data_parse_struct_t.MaxValue>=-30 && data_parse_struct_t.MaxValue<=100	){
							
							safe_temperature[0] = data_parse_struct_t.MinValue;
							safe_temperature[1] = data_parse_struct_t.MaxValue;
								
								safe_temperatureCopy[0]=safe_temperature[0]-1;
								safe_temperatureCopy[1]=safe_temperature[1]-1;
								
							FlashSetSafe(safe_temperature[0],safe_temperature[1],safe_type_temperature);
							MainLenCopy = sprintf(MainBuffer,"%s %s %s %s ","result","4","1","0");
						}
						else{
							MainLenCopy = sprintf(MainBuffer,"%s %s %s %s ","result","4","1","-4005");
						}
					}
					else{
						MainLenCopy = sprintf(MainBuffer,"%s %s %s %s ","result","4","1","-4005");
					}
					MainLen = MainLenCopy;
					if(MainLen>0){
						BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);MainLen=0;
					}
					
					//safe_humidity
					MainLenCopy = MinMaxParse(&data_parse_struct_t,data_parse_struct_t.value[1]);
					if(MainLenCopy==2){
						safe_humidity[0] = data_parse_struct_t.MinValue;
						safe_humidity[1] = data_parse_struct_t.MaxValue;
						safe_humidityCopy[0] = safe_humidity[0]-1;
						safe_humidityCopy[1] = safe_humidity[1]-1;
						FlashSetSafe(safe_humidity[0],safe_humidity[1],safe_type_humidity);
						MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s ","4","2","0");
					}
					else{
						MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s ","4","2","-4005");
					}
					MainLen = MainLen+MainLenCopy;
					if(MainLen>0){
						BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);MainLen=0;
					}
						
					//safe_illumination
					MainLenCopy = MinMaxParse(&data_parse_struct_t,data_parse_struct_t.value[2]);
					if(MainLenCopy==2){
						safe_illumination[0] = data_parse_struct_t.MinValue;
						safe_illumination[1] = data_parse_struct_t.MaxValue;
						safe_illuminationCopy[0] = safe_illumination[0]-1;
						safe_illuminationCopy[1] = safe_illumination[1]-1;
						FlashSetSafe(safe_illumination[0],safe_illumination[1],safe_type_illumination);
						MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s ","4","3","0");
					}
					else{
						MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s ","4","3","-4005");
					}
					MainLen = MainLen+MainLenCopy;
					if(MainLen>0){
						BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);MainLen=0;
					}
						
					//safe_type_fertility
					MainLenCopy = MinMaxParse(&data_parse_struct_t,data_parse_struct_t.value[3]);
					if(MainLenCopy==2){
						safe_fertility[0] = data_parse_struct_t.MinValue;
						safe_fertility[1] = data_parse_struct_t.MaxValue;
						safe_fertilityCopy[0] = data_parse_struct_t.MinValue-1;
						safe_fertilityCopy[1] = data_parse_struct_t.MaxValue-1;
						FlashSetSafe(safe_fertility[0],safe_fertility[1],safe_type_fertility);
						MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s\r","4","4","0");
					}
					else{
						MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s\r","4","4","-4005");
					}
					MainLen = MainLen+MainLenCopy;
					if(MainLen>0){
						BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);MainLen=0;
					}
					
					//safe_air_humidity
					MainLenCopy = MinMaxParse(&data_parse_struct_t,data_parse_struct_t.value[4]);
					if(MainLenCopy==2){
						safe_air_humidity[0] = data_parse_struct_t.MinValue;
						safe_air_humidity[1] = data_parse_struct_t.MaxValue;
						safe_air_humidityCopy[0] = safe_air_humidity[0]-1;
						safe_air_humidityCopy[1] = safe_air_humidity[1]-1;
						FlashSetSafe(safe_air_humidity[0],safe_air_humidity[1],safe_type_air_humidity);
						MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s ","4","5","0");
					}
					else{
						MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s ","4","5","-4005");
					}
					MainLen = MainLen+MainLenCopy;
					if(MainLen>0){
						BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);MainLen=0;
					}
					
					
					
					MainLen=0;
					MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s \"%d,%d\" ","properties_changed","4","1",safe_temperature[0],safe_temperature[1]);
					MainLen = MainLen+MainLenCopy;
					MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s \"%d,%d\" ","4","2",safe_humidity[0],safe_humidity[1]);
					MainLen = MainLen+MainLenCopy;
				  MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s \"%d,%d\" ","4","3",safe_illumination[0],safe_illumination[1]);
					MainLen = MainLen+MainLenCopy;
					MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s \"%d,%d\" ","4","5",safe_air_humidity[0],safe_air_humidity[1]);
					MainLen = MainLen+MainLenCopy;
					MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s \"%d,%d\"\r","4","4",safe_fertility[0],safe_fertility[1]);
					MainLen = MainLen+MainLenCopy;
					BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
					MainLen=0;
				}
			}
		}
		else if(data_parse_struct_t.message_type == get_properties)//读取属性
		{
			if(data_parse_len == 1)//单个属性
			{
				//读取浇水模式  读取浇水状态 浇水/没在浇水  读故障状态------------
				if(memcmp(data_parse_struct_t.siid[0],"2",1)==0)
				{
					if(memcmp(data_parse_struct_t.piid[0],"1",1)==0)//读取浇水模式
					{
						MainLen = sprintf(MainBuffer,"%s %s %s %s %c\r","result","2","1","0",WateringMode);
					}
					else if(memcmp(data_parse_struct_t.piid[0],"2",1)==0)//读取浇水状态 浇水/没在浇水
					{
						if(TIM2->CCR4 == 400)
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","2","2","0","true");
						}
						else
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","2","2","0","false");
						}						
					}
					else if(memcmp(data_parse_struct_t.piid[0],"3",1)==0){//读故障状态
						MainLen = sprintf(MainBuffer,"%s %s %s %s %d\r","result","2","3","0",fault);
						BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
						MainLen = sprintf(MainBuffer,"%s %s %s %d\r","properties_changed","2","3",fault);				
					}
				}
				//读取肥力  读取水位 读取浇水次数 读取植物类型-----------------
				else if(memcmp(data_parse_struct_t.siid[0],"3",1)==0)
				{
					if(memcmp(data_parse_struct_t.piid[0],"4",1)==0)//读取肥力
					{
						MainLen = sprintf(MainBuffer,"%s %s %s %s %d\r","result","3","4","0",fertility);
					}
					else if(memcmp(data_parse_struct_t.piid[0],"5",1)==0)//读取水位
					{
						MainLen = sprintf(MainBuffer,"%s %s %s %s %d\r","result","3","5","0",water_stage);
					}
					else if(memcmp(data_parse_struct_t.piid[0],"6",1)==0)//读取浇水次数
					{
						MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","3","6","0","1");
					}	
					else if(memcmp(data_parse_struct_t.piid[0],"7",1)==0)//读取植物类型
					{
						if(atoi(botany_type)>=19){//植物类型大于19
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","3","7","0","19");
						}else{
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","3","7","0",botany_type);
						}
					}
					else if(memcmp(data_parse_struct_t.piid[0],"8",1)==0)//读取植物类型
					{
						MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","3","8","0",botany_type);
					}
				}
				//读取安全值
				else if(memcmp(data_parse_struct_t.siid[0],"4",1)==0)
				{
					if(memcmp(data_parse_struct_t.piid[0],"1",1)==0)//safe-temperature
					{
						MainLen = sprintf(MainBuffer,"%s %s %s %s \"%d,%d\"\r","result","4","1","0",safe_temperature[0],safe_temperature[1]);
					}
					else if(memcmp(data_parse_struct_t.piid[0],"2",1)==0)//safe_humidity
					{
						MainLen = sprintf(MainBuffer,"%s %s %s %s \"%d,%d\"\r","result","4","2","0",safe_humidity[0],safe_humidity[1]);
					}
					else if(memcmp(data_parse_struct_t.piid[0],"3",1)==0)//safe_illumination
					{
						MainLen = sprintf(MainBuffer,"%s %s %s %s \"%d,%d\"\r","result","4","3","0",safe_illumination[0],safe_illumination[1]);
					}	
					else if(memcmp(data_parse_struct_t.piid[0],"4",1)==0)//safe_fertility
					{
						MainLen = sprintf(MainBuffer,"%s %s %s %s \"%d,%d\"\r","result","4","4","0",safe_fertility[0],safe_fertility[1]);
					}
					else if(memcmp(data_parse_struct_t.piid[0],"5",1)==0)//safe_air_humidity
					{
						MainLen = sprintf(MainBuffer,"%s %s %s %s \"%d,%d\"\r","result","4","5","0",safe_air_humidity[0],safe_air_humidity[1]);
					}
				}
				//读取LED状态 start-date  matrix  love-value  water-count  cumulative-harvest
				else if(memcmp(data_parse_struct_t.siid[0],"5",1)==0)
				{
					if(strlen(data_parse_struct_t.piid[0]) == 1 && memcmp(data_parse_struct_t.piid[0],"1",1)==0)//LED
					{
						if(PAin(5)==0)
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","5","1","0","true");
						}
						else
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","5","1","0","false");
						}
					}
					else if(memcmp(data_parse_struct_t.piid[0],"2",1)==0)//start-date //种植时间
					{
						if(start_date!=0)
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s \"%lld\"\r","result","5","2","0",start_date);
						}
						else
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","5","2","0","-4004");
						}
					}
					else if(memcmp(data_parse_struct_t.piid[0],"3",1)==0)//matrix  基质时间
					{
						MainLen = sprintf(MainBuffer,"%s %s %s %s %lld\r","result","5","3","0",matrix);
					}
					else if(memcmp(data_parse_struct_t.piid[0],"4",1)==0)//love-value 上报爱心值
					{
						MainLen = sprintf(MainBuffer,"%s %s %s %s \"%d\"\r","result","5","4","0",love_value);
					}
					else if(memcmp(data_parse_struct_t.piid[0],"5",1)==0)//water-count 
					{
						MainLen = sprintf(MainBuffer,"%s %s %s %s %d\r","result","5","5","0",water_count_copy);
					}
					else if(memcmp(data_parse_struct_t.piid[0],"6",1)==0)//cumulative-harvest 累计收获
					{
						MainLen = sprintf(MainBuffer,"%s %s %s %s %d\r","result","5","6","0",cumulative_harvest);
					}
					else if(memcmp(data_parse_struct_t.piid[0],"7",1)==0)//首次种植植物时间
					{
						MainLen = sprintf(MainBuffer,"%s %s %s %s \"%lld\"\r","result","5","7","0",one_start_date);
					}
					else if(memcmp(data_parse_struct_t.piid[0],"8",1)==0)//刷新数据
					{
						if(manual_refresh_value){
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","5","8","0","true");
						}else{
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","5","8","0","false");
						}
					}
					else if(memcmp(data_parse_struct_t.piid[0],"9",1)==0)
					{
						if(FlashGetReportEvent()==0)
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","5","9","0","true");
						}
						else
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","5","9","0","false");
						}
					}
					else if(memcmp(data_parse_struct_t.piid[0],"10",2)==0)//读取光照强度报警设置值
					{
						MainLen = sprintf(MainBuffer,"%s %s %s %s %d\r","result","5","10","0",FlashGetLightAlarm());
					}
					else if(memcmp(data_parse_struct_t.piid[0],"11",2)==0)//查询补光灯状态
					{
						if(fill_light){
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","5","11","0","true");
						}else{
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","5","11","0","false");
						}
					}		
					else if(memcmp(data_parse_struct_t.piid[0],"12",2)==0)//电池电量
					{
						MainLen = sprintf(MainBuffer,"%s %s %s %s %d\r","result","5","12","0",battery_level);
					}
					else if(memcmp(data_parse_struct_t.piid[0],"13",2)==0)//电池电量
					{
						MainLen = sprintf(MainBuffer,"%s %s %s %s %d\r","result","5","13","0",humidity_air);
					}
					else if(memcmp(data_parse_struct_t.piid[0],"14",2)==0)//是够支持电池
					{
						if(HardwareVersion>=6){//支持补光灯
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","5","14","0","true");
						}else{
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","5","14","0","false");
						}
					}
				}
				//读取温度  读取湿度  读取光照强度
				else if(memcmp(data_parse_struct_t.siid[0],"6",1)==0)//读取温度  读取湿度  读取光照强度
				{
					if(memcmp(data_parse_struct_t.piid[0],"3",1)==0)//读取温度
					{
						if(temperatureCopy !=255)//温度传感器故障
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s %d\r","result","6","3","0",temperature);
						}
						else
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s\r","result","6","3","-4004");
						}
					}
					else if(memcmp(data_parse_struct_t.piid[0],"4",1)==0)//读取湿度
					{
						MainLen = sprintf(MainBuffer,"%s %s %s %s %d\r","result","6","4","0",humidity);
					}
					else if(memcmp(data_parse_struct_t.piid[0],"6",1)==0)//读取光照强度
					{
						MainLen = sprintf(MainBuffer,"%s %s %s %s %d\r","result","6","6","0",illumination);
					}
					else if(memcmp(data_parse_struct_t.piid[0],"7",1)==0)//读取肥力
					{
						MainLen = sprintf(MainBuffer,"%s %s %s %s %d\r","result","3","4","0",fertility);
					}
				}
				//补光灯
				else if(memcmp(data_parse_struct_t.siid[0],"7",1)==0)
				{
					if(memcmp(data_parse_struct_t.piid[0],"3",1)==0)//是不是支持补光灯
					{
						if(HardwareVersion>=2)//支持补光灯
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","7","3","0","true");
						}
						else
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","7","3","0","false");
						}
					}
					else if(memcmp(data_parse_struct_t.piid[0],"2",1)==0)//是不是自动打开补光灯
					{
						if(FlashGetFillLightMode()==0)//自动打开补光灯
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","7","2","0","0");
						}
						else
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","7","2","0","1");
						}
					}
					else if(memcmp(data_parse_struct_t.piid[0],"1",1)==0)//补光灯状态
					{
						if(PBin(5)==0)//补光灯处于关闭状态
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","7","1","0","false");
						}
						else
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","7","1","0","true");
						}
					}
				}
				else if(memcmp(data_parse_struct_t.siid[0],"8",1)==0)
				{
					if(memcmp(data_parse_struct_t.piid[0],"1",1)==0)//是不是支持查找
					{
						if(HardwareVersion>=2)//支持
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","8","1","0","true");
						}
						else
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","8","1","0","false");
						}
					}
					else if(memcmp(data_parse_struct_t.piid[0],"2",1)==0)
					{
						if(find_flower)
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","8","2","0","true");
						}
						else
						{
							MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","8","2","0","false");
						}
					}
				}
			}
			else if(data_parse_len > 1)//多个属性
			{
				//读取 浇水模式,是否在浇水,是否故障
				if(memcmp(data_parse_struct_t.siid[0],"1",1)==0)
				{
					MainLenCopy = sprintf(MainBuffer,"%s %s %s %s %s ","result","1","1","0","ymt");
					MainLen = MainLenCopy;
					MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s %s ","1","2","0","ymt.flowerpot.v3");
					MainLen = MainLen + MainLenCopy;
					MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s %d\r","1","3","0",0);
					MainLen = MainLen + MainLenCopy;
				}
				//读取 浇水模式,是否在浇水,是否故障
				else if(memcmp(data_parse_struct_t.siid[0],"2",1)==0)
				{
					MainLenCopy = sprintf(MainBuffer,"%s %s %s %s %c ","result","2","1","0",WateringMode);//浇水模式
					MainLen = MainLenCopy;
					//是否在浇水
					if(TIM2->CCR4 == 400)
					{
						MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s %s ","2","2","0","true");
					}
					else
					{
						MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s %s ","2","2","0","false");
					}
					MainLen = MainLen + MainLenCopy;
					
					MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s %d\r","2","3","0",fault);//是否故障
					MainLen = MainLen + MainLenCopy;
				}
				//读取 肥力,水位,浇水次数,植物类型
				else if(memcmp(data_parse_struct_t.siid[0],"3",1)==0)
				{
					MainLenCopy = sprintf(MainBuffer,"%s %s %s %s %d ","result","3","4","0",fertility);//肥力
					MainLen = MainLenCopy;
					
					MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s %d ","3","5","0",water_stage);//水位
					MainLen = MainLen + MainLenCopy;
					
					MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s %s ","3","6","0","1");//浇水次数
					MainLen = MainLen + MainLenCopy;
					
					
					
					if(atoi(botany_type)>=19){//植物类型大于19
						MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s %s ","3","7","0","19");//植物类型
						MainLen = MainLen + MainLenCopy;
					}else{
						MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s %s ","3","7","0",botany_type);//植物类型
						MainLen = MainLen + MainLenCopy;
					}
					
					MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s %s\r","3","8","0",botany_type);//植物类型
					MainLen = MainLen + MainLenCopy;
				}
				//读取 温度,湿度,光照,肥力
				else if(memcmp(data_parse_struct_t.siid[0],"4",1)==0)
				{					
					MainLenCopy = sprintf(MainBuffer,"%s %s %s %s \"%d,%d\" ","result","4","1","0",safe_temperature[0],safe_temperature[1]);//温度
					MainLen = MainLenCopy;
					
					MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s \"%d,%d\" ","4","2","0",safe_humidity[0],safe_humidity[1]);//湿度
					MainLen = MainLen + MainLenCopy;
					
					MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s \"%d,%d\" ","4","3","0",safe_illumination[0],safe_illumination[1]);//光照
					MainLen = MainLen + MainLenCopy;
					
					MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s \"%d,%d\" ","4","4","0",safe_fertility[0],safe_fertility[1]);//肥力
					MainLen = MainLen + MainLenCopy;
					
					MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s \"%d,%d\"\r","4","5","0",safe_air_humidity[0],safe_air_humidity[1]);//空气湿度
					MainLen = MainLen + MainLenCopy;
				}
				else if(memcmp(data_parse_struct_t.siid[0],"5",1)==0)	
				{					
					//灯开关状态
					if(PAin(5)==0){
						MainLenCopy = sprintf(MainBuffer,"%s %s %s %s %s ","result","5","1","0","true");
					}
					else{
						MainLenCopy = sprintf(MainBuffer,"%s %s %s %s %s ","result","5","1","0","false");
					}
					MainLen = MainLenCopy;
					
					
					//种植时间
					if(start_date!=0){
						MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s \"%lld\" ","5","2","0",start_date);
					}
					else{
						MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s %s ","5","2","0","-4004");
					}
					MainLen = MainLen + MainLenCopy;
					
					
					//基质时间
					MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s %lld ","5","3","0",matrix);
					MainLen = MainLen + MainLenCopy;
					
					
					//爱心值
					MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s %d ","5","4","0",love_value);
					MainLen = MainLen + MainLenCopy;
					
					
					//浇水次数
					MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s %d ","5","5","0",water_count_copy);
					MainLen = MainLen + MainLenCopy;
					
					
					//累计收获
					MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s %d ","5","6","0",cumulative_harvest);
					MainLen = MainLen + MainLenCopy;	

					//首次种植植物时间
					MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s \"%lld\" ","5","7","0",one_start_date);
					MainLen = MainLen + MainLenCopy;
					
					//刷新数据
					if(manual_refresh){
						MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s %s ","5","8","0","true");
					}
					else{
						MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s %s ","5","8","0","false");
					}
					MainLen = MainLen + MainLenCopy;
					
					
					if(FlashGetReportEvent()==0)
					{
						MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s %s ","5","9","0","true");
					}
					else
					{
						MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s %s ","5","9","0","false");
					}
					MainLen = MainLen + MainLenCopy;
					
					
					if(HardwareVersion>=6){//支持电池
						MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s %s ","5","14","0","true");
					}else{
						MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s %s ","5","14","0","false");
					}
					MainLen = MainLen + MainLenCopy;
					
					
					if(fill_light){
						MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s %s ","5","11","0","true");
						
					}else{
						MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s %s ","5","11","0","false");
					}
					MainLen = MainLen + MainLenCopy;
//					....
					
					//电池电量
					MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s \"%d\" ","5","12","0",battery_level);
					MainLen = MainLen + MainLenCopy;
					
					//空气湿度
					MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s \"%d\" ","5","13","0",humidity_air);
					MainLen = MainLen + MainLenCopy;
					
					/*
											MainLen = sprintf(MainBuffer,"%s %s %s %s %d\r","result","5","12","0",battery_level);
					}
					else if(memcmp(data_parse_struct_t.piid[0],"13",2)==0)//电池电量
					{
						MainLen = sprintf(MainBuffer,"%s %s %s %s %d\r","result","5","13","0",humidity_air);
					*/
					
					MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s %d\r","5","10","0",FlashGetLightAlarm());
					MainLen = MainLen + MainLenCopy;
					
				}
				else if(memcmp(data_parse_struct_t.siid[0],"6",1)==0)
				{
					MainLenCopy = sprintf(MainBuffer,"%s 6 3 %s %d ","result","0",temperature);
					MainLen = MainLen + MainLenCopy;
					MainLenCopy = sprintf(MainBuffer+MainLen,"6 4 %s %d ","0",humidity);
					MainLen = MainLen + MainLenCopy;
					MainLenCopy = sprintf(MainBuffer+MainLen,"6 6 %s %d ","0",illumination);
					MainLen = MainLen + MainLenCopy;

					if(soil_ec_en){
						MainLenCopy = sprintf(MainBuffer+MainLen,"6 7 %s %d\r","0",fertility);
						MainLen = MainLen + MainLenCopy;
					}			
				}
				else if(memcmp(data_parse_struct_t.siid[0],"7",1)==0)
				{
					if(PBin(5)==0){
						MainLenCopy = sprintf(MainBuffer,"%s 7 1 %s %s ","result","0","false");
					}
					else{
						MainLenCopy = sprintf(MainBuffer,"%s 7 1 %s %s ","result","0","true");
					}
					MainLen = MainLen + MainLenCopy;
					
					if(FlashGetFillLightMode()==0){
						MainLenCopy = sprintf(MainBuffer+MainLen,"7 2 %s %d ","0",0);
					}
					else{
						MainLenCopy = sprintf(MainBuffer+MainLen,"7 2 %s %d ","0",1);
					}
					
					MainLen = MainLen + MainLenCopy;
					if(HardwareVersion>=2)//支持补光灯
					{
						MainLenCopy = sprintf(MainBuffer+MainLen,"7 3 %s %s ","0","true");
					}
					else{
						MainLenCopy = sprintf(MainBuffer+MainLen,"7 3 %s %s ","0","false");
					}
					
					MainLen = MainLen + MainLenCopy;
				}
				else if(memcmp(data_parse_struct_t.siid[0],"8",1)==0)
				{
					if(HardwareVersion>=2){//支持
						MainLenCopy = sprintf(MainBuffer,"%s 8 1 %s %s ","result","0","true");
					}
					else{
						MainLenCopy = sprintf(MainBuffer,"%s 8 1 %s %s ","result","0","false");
					}
					MainLen = MainLen + MainLenCopy;
					
					if(find_flower==0){
						MainLenCopy = sprintf(MainBuffer+MainLen,"8 2 %s %s ","0","false");
					}
					else{
						MainLenCopy = sprintf(MainBuffer+MainLen,"8 2 %s %s ","0","true");
					}
					
					MainLen = MainLen + MainLenCopy;
				}
			}
		}
		//时间戳
		else if(data_parse_struct_t.message_type == get_time_stamp)
		{
			timestamp = data_parse_struct_t.timestamp;
			
			startTime = timestamp+28800;
			lcTime = localtime (&startTime);		
		}
		else if(data_parse_struct_t.message_type == net_status){
//			module_net_state = data_parse_struct_t.net_status;
		}
		else if(data_parse_struct_t.message_type == power_off){
			MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","9","1","0","true");
		}
		else if(data_parse_struct_t.message_type == power_off){
			MainLen = sprintf(MainBuffer,"%s %s %s %s %s\r","result","9","1","0","true");
		}
		else if(data_parse_struct_t.message_type == mcu_version_req){
			MainLen = sprintf(MainBuffer,"mcu_version %s\r",mcu_version);
		}
		else if(data_parse_struct_t.message_type == error_code){
			MainLen = sprintf(MainBuffer,"%s \"%s\" %s\r","error",data_parse_struct_t.siid[0],"-5001");
		}
		
		data_parse_struct_t.message_type = set_type_none;
		if(MainLen>0)
		{
			BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
		}
	}
}

/**
* @brief   初始化flash里面的数据
* @param   None
* @warning None
* @example 
**/
void InitInitData(void)
{
	//植物类型
	botany_type[0]='-';
	botany_type[1]='1';
	botany_type[2]=0;
	botany_type[3]=0;
	FlashSetBotanyType(botany_type);//植物类型
	
	botany_typeCopy[0]='-';
	botany_typeCopy[1]='1';
	botany_typeCopy[2]=0;
	botany_typeCopy[3]=0;
	
	//爱心值
	love_value = 0;
	love_valueCopy = love_value;
	FlashSetLoveValue(love_value,1);
	
	//浇水模式
	FlashSetModeAuto();
	WateringMode='0';
	WateringModeCopy = WateringMode;
	
	//浇水次数
	water_count=0;
	FlashSetWaterCount(water_count,1);
	water_count_copy = water_count;
	
	//阈值:温度
	safe_temperature[0]=0;
	safe_temperature[1]=0;
	FlashSetSafe(safe_temperature[0],safe_temperature[1],safe_type_temperature);
	safe_temperatureCopy[0] = safe_temperature[0];
	safe_temperatureCopy[1] = safe_temperature[1];
	
	//阈值:湿度
	safe_humidity[0]=0;
	safe_humidity[1]=0;
	FlashSetSafe(safe_humidity[0],safe_humidity[1],safe_type_humidity);
	safe_humidityCopy[0] = safe_humidity[0];
	safe_humidityCopy[1] = safe_humidity[1];
	
	//阈值:空气湿度
	safe_air_humidity[0]=0;
	safe_air_humidity[1]=0;
	FlashSetSafe(safe_air_humidity[0],safe_air_humidity[1],safe_type_air_humidity);
	safe_air_humidityCopy[0] = safe_air_humidity[0];
	safe_air_humidityCopy[1] = safe_air_humidity[1];


	//阈值:光照
	safe_illumination[0]=0;
	safe_illumination[1]=0;
	FlashSetSafe(safe_illumination[0],safe_illumination[1],safe_type_illumination);
	safe_illuminationCopy[0] = safe_illumination[0];
	safe_illuminationCopy[1] = safe_illumination[1];
	
	//阈值:肥力
	safe_fertility[0]=0;
	safe_fertility[1]=0;
	FlashSetSafe(safe_fertility[0],safe_fertility[1],safe_type_fertility);
	safe_fertilityCopy[0] = safe_fertility[0];
	safe_fertilityCopy[1] = safe_fertility[1];
	
	//种植时间
	start_date=0;
	FlashSetDataPlant(start_date);
	start_dateCopy = start_date;
	
	//首次种植时间
	one_start_date=0;
	FlashSetOneDataPlant(one_start_date);
	one_start_dateCopy = one_start_date;
	
	//累计收获
	cumulative_harvest=0;
	FlashSetHarvestCount(cumulative_harvest);
	cumulative_harvestCopy = cumulative_harvest;
	
	//基质更改时间
	matrix=0;
	FlashSetMatrixData(matrix,1);
	matrixCopy = matrix;

	//培育时间
	NurtureDay = 0;
	FlashSetNurtureDay(NurtureDay,1);
	NurtureDayCopy = NurtureDay;
	
	
	//夜间不报警浇水
	FlashSetReportEvent(0);
	ReportEventFlag=0;
}

/**
* @brief   获取flash存储的数据
* @param   None
* @param   None
* @param   None
* @retval  None
* @warning None
* @example 
**/
void InitData(void)
{
	u16 ReadDat;
	STMFLASH_Read(FLASH_USE_FLASH_HELPER,&ReadDat,1);//获取是否已经把数据更新到 flash_helper
	
	FlashGetBotanyType(botany_type);//获取植物类型
	if(botany_type[0]==0){
		botany_type[0]='-';
		botany_type[1]='1';
		botany_type[2]=0;
		botany_type[3]=0;
	}
	memcpy(botany_typeCopy,botany_type,5);
//	printf("\r\nbotany_type:%s\r\n",botany_type);
	
	//爱心值
	if(ReadDat!=1){
		love_value = FlashGetLoveValue(0);
		FlashSetLoveValue(love_value,1);
	}
	love_value = FlashGetLoveValue(1);
	love_valueCopy = love_value;
	
	//浇水模式
	WateringMode = FlashGetModeWatering();
	if(WateringMode==0){
		WateringMode='0';
	}
	WateringModeCopy = WateringMode;
//	printf("\r\nWateringMode:%c\r\n",WateringMode);
	
	//浇水次数
	if(ReadDat!=1){
		water_count = FlashGetWaterCount(0);
		FlashSetWaterCount(water_count,1);
	}
	water_count = FlashGetWaterCount(1);
	water_count_copy = water_count;
//	printf("\r\nwater_count:%d\r\n",water_count);
	
	//阈值:温度
	FlashGetSafe(&safe_temperature[0],&safe_temperature[1],safe_type_temperature);
	safe_temperatureCopy[0] = safe_temperature[0];
	safe_temperatureCopy[1] = safe_temperature[1];
//	printf("\r\nsafe_temperature:%d,%d\r\n",safe_temperature[0],safe_temperature[1]);
	
	//阈值:湿度
	FlashGetSafe(&safe_humidity[0],&safe_humidity[1],safe_type_humidity);
	safe_humidityCopy[0] = safe_humidity[0];
	safe_humidityCopy[1] = safe_humidity[1];
	
	//阈值:空气湿度
	FlashGetSafe(&safe_air_humidity[0],&safe_air_humidity[1],safe_type_air_humidity);
	safe_air_humidityCopy[0] = safe_air_humidity[0];
	safe_air_humidityCopy[1] = safe_air_humidity[1];
	
//	printf("\r\nsafe_humidity:%d,%d\r\n",safe_humidity[0],safe_humidity[1]);
	
	//阈值:光照
	FlashGetSafe(&safe_illumination[0],&safe_illumination[1],safe_type_illumination);
	safe_illuminationCopy[0] = safe_illumination[0];
	safe_illuminationCopy[1] = safe_illumination[1];
//	printf("\r\nsafe_illumination:%d,%d\r\n",safe_illumination[0],safe_illumination[1]);
	
	//阈值:肥力
	FlashGetSafe(&safe_fertility[0],&safe_fertility[1],safe_type_fertility);
	safe_fertilityCopy[0] = safe_fertility[0];
	safe_fertilityCopy[1] = safe_fertility[1];
//	printf("\r\nsafe_fertility:%d,%d\r\n",safe_fertility[0],safe_fertility[1]);
	
	//种植时间
	start_date = FlashGetDataPlant();
	start_dateCopy = start_date;
//	printf("\r\nstart_date:%lld\r\n",start_date);
	
	//首次种植时间
	one_start_date = FlashGetOneDataPlant();
	one_start_dateCopy = one_start_date;
//	printf("\r\none_start_date:%lld\r\n",one_start_date);
	
	//累计收获
	cumulative_harvest = FlashGetHarvestCount();
	cumulative_harvestCopy = cumulative_harvest;
	
	//基质更改时间
	if(ReadDat!=1)
	{
		matrix = FlashGetMatrixData(0);
		FlashSetMatrixData(matrix,1);
	}
	matrix = FlashGetMatrixData(1);
	matrixCopy = matrix;
	
	//培育时间
	if(ReadDat!=1){
		NurtureDay = FlashGetNurtureDay(0);
		FlashSetNurtureDay(NurtureDay,1);
	}
	NurtureDay = FlashGetNurtureDay(1);
	NurtureDayCopy = NurtureDay;
	
	
	if(ReadDat!=1)
	{
		ReadDat = 1;
		STMFLASH_Write(FLASH_USE_FLASH_HELPER,&ReadDat,1);
	}
	
	//夜间是否报警浇水
	ReportEventFlag=FlashGetReportEvent();
	
}


/**
* @brief   状态改变时上报数据
* @param   None
* @param   None
* @param   None
* @retval  None
* @warning None
* @example 
**/
void properties_changed(void)
{
	MainLen=0;
	MainLenCopy=0;
	
	/*状态改变上报*T***************************************************************/
	//浇水模式
	//WateringMode
	if(WateringModeCopy != WateringMode){
		WateringModeCopy = WateringMode;
		MainLen = sprintf(MainBuffer,"%s %s %s %c\r","properties_changed","2","1",WateringModeCopy);
		
		BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
	}
	
	//浇水状态
	//WateringState
	if(WateringStateCopy != TIM2->CCR4){
		WateringStateCopy = TIM2->CCR4;
		if(WateringStateCopy){
			MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","2","2","true");
			water_count++;//浇水次数加一
		}
		else{
			MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","2","2","false");
		}
		
		BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
	}
	//故障
	//fault
	if(faultcopy != fault){
		faultcopy = fault;
		MainLen = sprintf(MainBuffer,"%s %s %s %d\r","properties_changed","2","3",fault);
		BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
	}
	/*状态改变上报*P***************************************************************/
			//肥力		
		//fertilityCopy
		if(fertilityCopy - fertility>5 || fertility-fertilityCopy>5)
		{
			fertilityCopy = fertility;
			MainLen = sprintf(MainBuffer,"%s %s %s %d\r","properties_changed","3","4",fertility);
			
			BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
			
			if(soil_ec_en){
				MainLen = sprintf(MainBuffer,"%s %s %s %d\r","properties_changed","6","7",fertility);
				
				BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
			}
		}
		//湿度
		//humidityCopy
		if(humidityCopy!=humidity)
		{
			humidityCopy = humidity;
			MainLen = sprintf(MainBuffer,"%s %s %s %d\r","properties_changed","6","4",humidity);
			
			BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
		}
		//温度
		//temperatureCopy
		if(temperatureCopy!=temperature)
		{
			temperatureCopy = temperature;
			MainLen = sprintf(MainBuffer,"%s %s %s %d\r","properties_changed","6","3",temperature);
			
			BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
		}
		//光照
		//illuminationCopy
		if(illuminationCopy >=0 && (illuminationCopy- illumination >=5 || illumination - illuminationCopy>=5))
		{
			illuminationCopy = illumination;
			MainLen = sprintf(MainBuffer,"%s %s %s %d\r","properties_changed","6","6",illumination);
			BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
		}
		
		//水位
		//water_stage
		if(water_stageCopy!=water_stage)
		{
			water_stageCopy = water_stage;
			MainLen = sprintf(MainBuffer,"%s %s %s %d\r","properties_changed","3","5",water_stage);
			
			BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
		}
	
	/*定时上报*S***************************************************************/
	if(testing_time_appear_timer_cnt>EventReportTime || manual_refresh==1 || manual_refresh_one==0){
		testing_time_appear_timer_cnt=0;
		
		if(manual_refresh_one==0){
			manual_refresh_one = 1;
			
			MainLenCopy=0;
			MainLen = sprintf(MainBuffer,"%s %s %s %s ","properties_changed","5","8","true");
			MainLen =  MainLen + MainLenCopy;
			
			MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %d ","3","4",fertility);
			MainLen =  MainLen + MainLenCopy;
			
			if(soil_ec_en){
				MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %d ","6","7",humidity);
				MainLen =  MainLen + MainLenCopy;
			}
			
			MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %d ","6","4",humidity);
			MainLen =  MainLen + MainLenCopy;
			
			MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %d ","6","3",temperature);
			MainLen =  MainLen + MainLenCopy;
			
			MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %d ","6","6",illumination);
			MainLen =  MainLen + MainLenCopy;
//			if(atoi(botany_type)>=19){//植物类型大于19
				MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s ","9","9","1111");//植物类型
				MainLen = MainLen + MainLenCopy;
//			}else
			{
				MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s ","3","7",botany_type);//植物类型
				MainLen = MainLen + MainLenCopy;
			}
			
			MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s ","3","8",botany_type);//植物类型
			MainLen =  MainLen + MainLenCopy;
			
			MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %d ","5","6",cumulative_harvest);
			MainLen =  MainLen + MainLenCopy;
			
			if(HardwareVersion>=2){
				MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s ","7","3","true");
			}
			else{
				MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s ","7","3","false");
			}
			MainLen =  MainLen + MainLenCopy;
			
			if(HardwareVersion>=2){
				MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s ","8","1","true");
			}
			else{
				MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %s ","8","1","false");
			}
			MainLen =  MainLen + MainLenCopy;
			
			
			MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s %d\r","3","5",water_stage);
			MainLen =  MainLen + MainLenCopy;
			
			
			BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
			
			
			
			MainLenCopy=0;
			MainLen=0;
			MainLenCopy = sprintf(MainBuffer,"%s %s %s \"%d,%d\" ","properties_changed","4","1",safe_temperature[0],safe_temperature[1]);//温度
			MainLen = MainLenCopy;

			MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s \"%d,%d\" ","4","2",safe_humidity[0],safe_humidity[1]);//湿度
			MainLen = MainLen + MainLenCopy;

			MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s \"%d,%d\" ","4","3",safe_illumination[0],safe_illumination[1]);//光照
			MainLen = MainLen + MainLenCopy;
			
			MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s \"%d,%d\" ","4","5",safe_air_humidity[0],safe_air_humidity[1]);//湿度
			MainLen = MainLen + MainLenCopy;

			MainLenCopy = sprintf(MainBuffer+MainLen,"%s %s \"%d,%d\"\r","4","4",safe_fertility[0],safe_fertility[1]);//肥力
			MainLen = MainLen + MainLenCopy;
			
			BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
		}
		
		manual_refresh = 0;
		

	}
	/*定时上报*P***************************************************************/
	
	
	
	
	//浇水次数改变
	if(water_count_copy != water_count)
	{
		water_count_copy = water_count;
		
		FlashSetWaterCount(water_count,1);//存储浇水次数
		
		MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","3","6","1");
		BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
		
		
		MainLen = sprintf(MainBuffer,"%s %s %s %d\r","properties_changed","5","5",water_count_copy);
		BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
		
		if(water_count!=0){
			love_value++;//爱心值累加
			FlashSetLoveValue(love_value,1);//存储爱心值
		}
	}
	//植物类型
	//botany_typeCopy
	if(botany_typeCopy[0]!=botany_type[0] ||
		botany_typeCopy[1]!=botany_type[1] ||
		botany_typeCopy[2]!=botany_type[2] ||
		botany_typeCopy[3]!=botany_type[3]){
		
		if(atoi(botany_type)>=19){//植物类型大于19
			MainLen = sprintf(MainBuffer,"%s %s %s %d\r\n","properties_changed","3","7",19);
			BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
		}else{
			MainLen = sprintf(MainBuffer,"%s %s %s %s\r\n","properties_changed","3","7",botany_type);
			BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
		}
				
		MainLen = sprintf(MainBuffer,"%s %s %s %s\r\n","properties_changed","3","8",botany_type);
		BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
			
		
//		MainLen = sprintf(MainBuffer,"%s %s %s %s\r\n","properties_changed","3","7",botany_type);
			memcpy(botany_typeCopy,botany_type,5);
			
//		BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
	}
	
	//LED
	if(LEDStateCopy != (1-PAin(5))){
		LEDStateCopy = (1-PAin(5));
		if(LEDStateCopy){
			MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","5","1","true");
		}
		else{
			MainLen = sprintf(MainBuffer,"%s %s %s %s\r","properties_changed","5","1","false");
		}
		BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
	}
	//种植时间改变
//	if(start_dateCopy != start_date){
//		start_dateCopy = start_date;
//		MainLen = sprintf(MainBuffer,"%s %s %s \"%lld\"\r\n","properties_changed","5","2",start_date);
//		
//		BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
//	}
	//首次种植时间改变
//	if(one_start_dateCopy != one_start_date){
//		one_start_dateCopy = one_start_date;
//		MainLen = sprintf(MainBuffer,"%s %s %s \"%lld\"\r\n","properties_changed","5","7",one_start_date);
//		
//		BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
//	}
	//基质更改时间
	if(matrixCopy!=matrix){
		matrixCopy = matrix;
		MainLen = sprintf(MainBuffer,"%s %s %s %lld\r","properties_changed","5","3",matrix);
		
		BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
	}
	//love_value
	if(love_valueCopy!=love_value){
		love_valueCopy = love_value;
		
		MainLen = sprintf(MainBuffer,"%s %s %s %d\r","properties_changed","5","4",love_value);
		BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
	}
	//累计收获
	if(cumulative_harvestCopy != cumulative_harvest){
		cumulative_harvestCopy = cumulative_harvest;
		MainLen = sprintf(MainBuffer,"%s %s %s %d\r","properties_changed","5","6",cumulative_harvest);
		
		BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
	}
	
	/*安全值*********************************************/
	//safe_air_humidity
	if(safe_air_humidityCopy[0]!=safe_air_humidity[0] || safe_air_humidityCopy[1]!=safe_air_humidity[1]){
		safe_air_humidityCopy[0]=safe_air_humidity[0];
		safe_air_humidityCopy[1]=safe_air_humidity[1];
		
		MainLen = sprintf(MainBuffer,"%s %s %s \"%d,%d\"\r","properties_changed","4","5",safe_air_humidity[0],safe_air_humidity[1]);
		BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
	}
	//safe_fertility
	if(safe_fertilityCopy[0]!=safe_fertility[0] || safe_fertilityCopy[1]!=safe_fertility[1]){
		safe_fertilityCopy[0]=safe_fertility[0];
		safe_fertilityCopy[1]=safe_fertility[1];
		MainLen = sprintf(MainBuffer,"%s %s %s \"%d,%d\"\r","properties_changed","4","4",safe_fertility[0],safe_fertility[1]);
		
		BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
	}
	//safe_humidity
	if(safe_humidityCopy[0]!=safe_humidity[0] || safe_humidityCopy[1]!=safe_humidity[1]){
		safe_humidityCopy[0]=safe_humidity[0];
		safe_humidityCopy[1]=safe_humidity[1];
		
		MainLen = sprintf(MainBuffer,"%s %s %s \"%d,%d\"\r","properties_changed","4","2",safe_humidity[0],safe_humidity[1]);
		BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
	}
	//safe_temperature
	if(safe_temperatureCopy[0]!=safe_temperature[0] || safe_temperatureCopy[1]!=safe_temperature[1]){
		safe_temperatureCopy[0]=safe_temperature[0];
		safe_temperatureCopy[1]=safe_temperature[1];
		MainLen = sprintf(MainBuffer,"%s %s %s \"%d,%d\"\r","properties_changed","4","1",safe_temperature[0],safe_temperature[1]);
		
		BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
	}
	//safe_illumination
	if(safe_illuminationCopy[0]!=safe_illumination[0] || safe_illuminationCopy[1]!=safe_illumination[1]){
		safe_illuminationCopy[0]=safe_illumination[0];
		safe_illuminationCopy[1]=safe_illumination[1];
		MainLen = sprintf(MainBuffer,"%s %s %s \"%d,%d\"\r","properties_changed","4","3",safe_illumination[0],safe_illumination[1]);
		
		BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
	}
	
	
//	//刷新
//	if(manual_refreshCopy != manual_refresh){
//		manual_refreshCopy = manual_refresh;
//		if(manual_refresh){
//			MainLen = sprintf(MainBuffer,"%s %s %s %s\r\n","properties_changed","5","8","true");
//		}
//		else{
//			MainLen = sprintf(MainBuffer,"%s %s %s %s\r\n","properties_changed","5","8","false");
//		}
//		BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
//	}
	
	
//	if(MainLen>0){
//		BufferManageWrite(&buff_manage_struct_t,MainBuffer,MainLen);
//	}
}



void InitValue(void)
{
	memset(mcu_version,0,5);
	memcpy(mcu_version,define_mcu_version,4);
	
//	memset(get_model,0,20);
//	memcpy(get_model,define_get_model,17);
	
//	memset(get_down,0,10);
//	memcpy(get_down,"get_down\r",9);
	
//	memset(get_time_posix,0,15);
//	memcpy(get_time_posix,"time posix\r",11);
	
//	memset(get_net,0,6);
//	memcpy(get_net,"net\r",4);
	
//	memset(set_ble,0,25);
//	memcpy(set_ble,define_set_ble,19);
//	
//	memset(dump_ble,0,25);
//	memcpy(dump_ble,define_dump_ble,18);
	
	water_stage_empty_flag=0; //缺水
	water_stage_empty_flagCopy=0; //缺水
	water_stage_empty_cnt1=0;//缺水
	water_stage_empty_run_flage=0;
	water_stage_water_flag=0; //有水
	water_stage_water_flagCopy=0; //有水
	water_stage_water_cnt1=0;//有水
	water_stage_full_flag=0; //水满
	water_stage_full_flagCopy=0; //水满
	water_stage_full_cnt1=0;//水满
	water_stage_full_run_flage=0;
	water_stage_nofull_flag=0;
	water_stage_nofull_flagCopy=0;
	water_stage_nofull_cnt1=0;
	
	InitModelFlag=0;
	InitModelCnt=0;
	SendModelCnt=0;
	InitBleFlag=0;
	InitBleCnt=0;
	SendBleCnt=0;
	
	matrix=0;matrixCopy=0;//基质时间
	love_value=0;love_valueCopy=0;//爱心值
	cumulative_harvest=0;cumulative_harvestCopy=0; //累计收获
	WateringMode='0';WateringModeCopy='0';//默认自动模式(浇水模式)
	WateringStateCopy = 0;//浇水状态
	WateringFlag=0;//在浇水:1
	WateringDelayTimer=0;//浇水延时时间
	WateringCirculation = 0;//每隔1个小时浇一次水
	
	LEDState=-1,LEDStateCopy=-1;//LED状态
	fault = 0,faultcopy=0;//故障
	water_count=0,water_count_copy=0;//浇水次数,每浇一次累加一次,上报一次
	botany_type[0]='-';botany_type[0]='1';botany_type[0]=0;botany_type[0]=0;//植物类型 0-999
	botany_typeCopy[0]='-';botany_typeCopy[0]='1';botany_typeCopy[0]=0;botany_typeCopy[0]=0;//植物类型 0-999
	botany_typeCopyCopy[0]='-';botany_typeCopyCopy[0]='1';botany_typeCopyCopy[0]=0;botany_typeCopyCopy[0]=0;//植物类型 0-999
	
	water_stage=0;water_stageCopy=0;//水位 0-100
	water_stageTempCount=0;//水位 0-100

	fertility=0;fertilityCopy=0;//肥力 0-2000
	humidity=0;humidityCopy=0;//湿度
	temperature=0;temperatureCopy=0;//温度
	
	/*检测传感器故障*/
  fertilityErrCnt=0;humidityErrCnt=0,sensor_ligntErrCnt=0;

  DS18B20Cnt=0;
  sensor_ligntCnt=0;
  water_stageCnt=0;

  illumination=0,illuminationCopy=0;//光照
	illuminationTempCount=0;//光照
	for(MainLen=0;MainLen<2;MainLen++)sensor_lignt_temp[MainLen]=0;//采集的光照传感器的高低位
	sensor_lignt_exponent=0;sensor_lignt_mantissa=0;

  testing_time_appear_timer_cnt=0;//控制温度,湿度,光照,肥力检测上报时间
	for(MainLen=0;MainLen<2;MainLen++){
		safe_humidity[MainLen]=0;
		safe_humidityCopy[MainLen]=0;
		safe_temperature[MainLen]=0;
		safe_temperatureCopy[MainLen]=0;
		safe_illumination[MainLen]=0;
		safe_illuminationCopy[MainLen]=0;
		safe_fertility[MainLen]=0;
		safe_fertilityCopy[MainLen]=0;
	}
	/*********************************/
	
	send_get_down_cnt=0;//控制发送get_down时间
	data_parse_len=0;//解析数据后获取的字段长度
	send_get_net_cnt = 0;//控制发送get_net的时间
	/*******/
	one_start_date=0;one_start_dateCopy=0;//首次种植植物的开始时间
	/*******/
	manual_refresh = 0;manual_refreshCopy=0;//获取刷新数据指令:1,上报:水位,温度,湿度,光照,肥力
	manual_refresh_one=0;
	/************/
	timestamp=0;//当前时间戳
	startTime = 28800;
	lcTime = localtime (&startTime);//解析时间戳
	
	
	start_date=0;start_dateCopy=0;//种植植物的开始时间
	get_time_stamp_flag=0;//是否获取了时间戳
	get_time_stamp_time = 1000;
	get_time_stamp_cnt=0; //控制定时获取时间戳
	/************/
	module_net_state=0;
	cloud_flage=0;//设备连接上小米服务器
	mcu_version_appear=0;//连接上服务器上报一次版本
	mcu_version_appear1=0;//升级完成直接上报版本
	NurtureDay=0;NurtureDayCopy=0;//培育天数
	
	EventReportTimeCnt=EventReportTime;
	EventReportTimeCnt1=EventReportTime1;
	EventReportTimeCnt2=EventReportTime2;
	
	SmartConfigState=0;
	SmartConfigFlage=0;
	SmartConfigCnt=0;
	KeyPressTime=0;//按键按下时间
	
	
	watering_timer_start = 8;//开始时间
	watering_timer_stop = 20;//结束时间

	/*报警事件事件范围*/
	event_timer_start = 8;//开始时间
	event_timer_stop = 20;//结束时间
	
	/****光照强度统计时间****/
	illumination_statistics_timer_start = 8;//开始时间
	illumination_statistics_timer_stop = 20;//结束时间
}



