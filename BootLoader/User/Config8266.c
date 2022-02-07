/**
  ******************************************************************************
  * @author  yang feng wu 
  * @version V1.0.0
  * @date    2019/10/12
  * @brief   配置8266
  ******************************************************************************
	一,使用说明:指示灯(PC13)
		1,把以下程序放在1ms定时器中断中
			SmartConfigCnt++;
			if(SmartConfigFlage)//配网状态,指示灯闪耀
			{
				Config8266LedDelay++;
				if(Config8266LedDelay>100)
				{
					Config8266LedDelay=0;
					SmartConfigPinOut = ~SmartConfigPinOut;
				}
			}
			else
			{
				Config8266LedDelay=0;
			}
		
		2,调用使用,建议使用一个按钮控制
			
			if(SmartConfig())//配网成功
			{
				//执行操作
			}
  ******************************************************************************
  */

#define CONFIG8266_C_
#include "include.h"

char SmartConfigFlage = 0;//是不是在配网
u32  SmartConfigCnt = 0;//配网连接路由器延时使用
char SmartConfigSuccess = 0;//是否配网成功
u32  Config8266Delay=0;//执行Config8266函数内部所需延时
u32  Config8266LedDelay=0;//配置8266指示灯闪耀


/**
* @brief  启用SmartConfig 给WIFI配网
* @ warn  None
* @param  None
* @param  None
* @param  None
* @param  None
* @retval 1:成功
* @example 
**/
char SmartConfig(void)
{
  u32 delay=0,Flage=0;
	SmartConfigPinOut = 1;
	SmartConfigSuccess = 0;
	Rst8266();
	
	if(ConfigModuleBlock("+++","+++",NULL))//退出透传
	{
		if(ConfigModuleBlock("AT+RESTORE\r\n","ready",NULL))//恢复出厂设置
		{
			if(ConfigModuleBlock("AT+CWMODE_DEF=1\r\n","OK",NULL))//模式1
			{
				if(ConfigModuleBlock("AT+CWAUTOCONN=1\r\n","OK",NULL))//自动连接路由器
				{
					if(ConfigModuleBlock("AT+CWSTARTSMART=3\r\n","OK",NULL))//配网(支持SmartConfig和微信AirKiss)
					{
						SmartConfigCnt = 0;
						while(1)
						{
							IWDG_Feed();//喂狗
							if(Usart1ReadFlage==1)
							{
								Usart1ReadFlage=0;
								if(strstr(Usart1ReadBuff,"WIFI CONNECTED") || strstr(Usart1ReadBuff,"WIFI GOT IP"))
								{
									SmartConfigPinOut = 1;//设备连接上路由器
									break;
								}
							}
							if(SmartConfigCnt>30000)
							{
								break;
							}
						}
						if(SmartConfigCnt>30000)//配网超时
						{
							SmartConfigPinOut = 0;
						}
						else
						{
							Flage = 1;
							SmartConfigSuccess=1;
							delay = SmartConfigCnt+5000;//延时5S
							while(SmartConfigCnt<delay)//让WIFI把MAC信息传给APP
							{
								IWDG_Feed();//喂狗
							}
						}
					}
				}
			}
		}
	}
	Rst8266();//复位
	SmartConfigFlage = 0;
	return Flage;
}

/**
* @brief  复位8266
* @ warn  单片机 Rst8266Pin 引脚需要和8266 RST引脚相连接
* @param  None
* @param  None
* @param  None
* @param  None
* @retval None
* @example 
**/
void Rst8266(void)
{
	Rst8266PinOut = 0;
	delay_ms(500);
	Rst8266PinOut = 1;
}


/**
* @brief  连接路由器
* @ warn  None
* @param  None
* @param  None
* @param  None
* @param  None
* @retval None
* @example 
**/
void Init8266(void)
{
	Rst8266();
	ConfigModuleBlock("+++","+++",NULL);//退出透传
	ConfigModuleBlock("AT+RESTORE\r\n","ready",NULL);//回复出厂设置
	ConfigModuleBlock("AT+CWMODE_DEF=1\r\n","OK",NULL);//模式1
	ConfigModuleBlock("AT+CWAUTOCONN=1\r\n","OK",NULL);//自动连接路由器
  ConfigModuleBlock("AT+CWJAP_DEF=\"qqqqq\",\"11223344\"\r\n","OK","WIFI CONNECTED");//设置连接的路由器
	Rst8266();//复位
}


/**
* @brief  链接TCP服务器
* @ warn  None
* @param  None
* @param  None
* @param  None
* @param  None
* @retval 1:可以连接TCP服务器
* @example 
**/
char AutoConnectTCP(void)
{
	char flage=0;
	char data[100];
	Rst8266();
	ConfigModuleBlock("+++","+++",NULL);//退出透传
	sprintf(data,"AT+SAVETRANSLINK=1,\"%s\",%s,\"TCP\"\r\n",IP,Port);
	flage = ConfigModuleBlock(data,"OK","ALREADY CONNECTED");//连接TCP服务器
	Rst8266();
	return flage;
}
