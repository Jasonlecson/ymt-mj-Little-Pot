/**
  ******************************************************************************
  * @author  yang feng wu 
  * @version V1.0.0
  * @date    2019/10/12
  * @brief   ����8266
  ******************************************************************************
	һ,ʹ��˵��:ָʾ��(PC13)
		1,�����³������1ms��ʱ���ж���
			SmartConfigCnt++;
			if(SmartConfigFlage)//����״̬,ָʾ����ҫ
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
		
		2,����ʹ��,����ʹ��һ����ť����
			
			if(SmartConfig())//�����ɹ�
			{
				//ִ�в���
			}
  ******************************************************************************
  */

#define CONFIG8266_C_
#include "include.h"

char SmartConfigFlage = 0;//�ǲ���������
u32  SmartConfigCnt = 0;//��������·������ʱʹ��
char SmartConfigSuccess = 0;//�Ƿ������ɹ�
u32  Config8266Delay=0;//ִ��Config8266�����ڲ�������ʱ
u32  Config8266LedDelay=0;//����8266ָʾ����ҫ


/**
* @brief  ����SmartConfig ��WIFI����
* @ warn  None
* @param  None
* @param  None
* @param  None
* @param  None
* @retval 1:�ɹ�
* @example 
**/
char SmartConfig(void)
{
  u32 delay=0,Flage=0;
	SmartConfigPinOut = 1;
	SmartConfigSuccess = 0;
	Rst8266();
	
	if(ConfigModuleBlock("+++","+++",NULL))//�˳�͸��
	{
		if(ConfigModuleBlock("AT+RESTORE\r\n","ready",NULL))//�ָ���������
		{
			if(ConfigModuleBlock("AT+CWMODE_DEF=1\r\n","OK",NULL))//ģʽ1
			{
				if(ConfigModuleBlock("AT+CWAUTOCONN=1\r\n","OK",NULL))//�Զ�����·����
				{
					if(ConfigModuleBlock("AT+CWSTARTSMART=3\r\n","OK",NULL))//����(֧��SmartConfig��΢��AirKiss)
					{
						SmartConfigCnt = 0;
						while(1)
						{
							IWDG_Feed();//ι��
							if(Usart1ReadFlage==1)
							{
								Usart1ReadFlage=0;
								if(strstr(Usart1ReadBuff,"WIFI CONNECTED") || strstr(Usart1ReadBuff,"WIFI GOT IP"))
								{
									SmartConfigPinOut = 1;//�豸������·����
									break;
								}
							}
							if(SmartConfigCnt>30000)
							{
								break;
							}
						}
						if(SmartConfigCnt>30000)//������ʱ
						{
							SmartConfigPinOut = 0;
						}
						else
						{
							Flage = 1;
							SmartConfigSuccess=1;
							delay = SmartConfigCnt+5000;//��ʱ5S
							while(SmartConfigCnt<delay)//��WIFI��MAC��Ϣ����APP
							{
								IWDG_Feed();//ι��
							}
						}
					}
				}
			}
		}
	}
	Rst8266();//��λ
	SmartConfigFlage = 0;
	return Flage;
}

/**
* @brief  ��λ8266
* @ warn  ��Ƭ�� Rst8266Pin ������Ҫ��8266 RST����������
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
* @brief  ����·����
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
	ConfigModuleBlock("+++","+++",NULL);//�˳�͸��
	ConfigModuleBlock("AT+RESTORE\r\n","ready",NULL);//�ظ���������
	ConfigModuleBlock("AT+CWMODE_DEF=1\r\n","OK",NULL);//ģʽ1
	ConfigModuleBlock("AT+CWAUTOCONN=1\r\n","OK",NULL);//�Զ�����·����
  ConfigModuleBlock("AT+CWJAP_DEF=\"qqqqq\",\"11223344\"\r\n","OK","WIFI CONNECTED");//�������ӵ�·����
	Rst8266();//��λ
}


/**
* @brief  ����TCP������
* @ warn  None
* @param  None
* @param  None
* @param  None
* @param  None
* @retval 1:��������TCP������
* @example 
**/
char AutoConnectTCP(void)
{
	char flage=0;
	char data[100];
	Rst8266();
	ConfigModuleBlock("+++","+++",NULL);//�˳�͸��
	sprintf(data,"AT+SAVETRANSLINK=1,\"%s\",%s,\"TCP\"\r\n",IP,Port);
	flage = ConfigModuleBlock(data,"OK","ALREADY CONNECTED");//����TCP������
	Rst8266();
	return flage;
}
