/**
  ******************************************************************************
  * @file    usart.c
  * @author  fengwu yang
  * @version V1.0.0
  * @date    2019/10/12
  * @brief   
  ******************************************************************************
	一,使用说明
		1,把以下程序放在1ms定时器中断中
			ConfigModuleNoBlockCnt++;
			
		2,把以下程序放到主函数
		while(1)
		{
			ConfigModuleNoBlock();//配置模块
			if(串口接收到一条完整的数据标志)
			{
			  清零 串口接收到一条完整的数据标志
				if(ConfigConnectDispose != NULL)
				{
					ConfigConnectDispose(Usart1ReadBuff);//处理模块返回的数据
				}
			}
		}
			
  ******************************************************************************
  */


#define CONFIGMODULENOBLOCK_C_
#include "include.h"

u32 ConfigModuleNoBlockCnt =0;        //配置函数延时变量,定时器内部累加
int ConfigModuleNoBlockCaseValue = 0; //控制执行哪一条Case 语句
char ConfigModuleNoBlockFlage  = 0;   //1-配置完连接 0-未配置完连接

u32 CompareValue=1500;                //每隔 Ms 发送一次数据
u32 SendNextDelay =0;                 //接收SendConfigFunction函数最后一个参数,最终传递给 ConfigModuleNoBlockCnt 控制写一条数据发送的时间
int Cnt = 0;                          //记录运行状态发送的次数
char DataReturnFlage  = 0;            //是否返回了预期的数据

char HopeReturnData1[20]="";//存储希望返回的数据
char HopeReturnData2[20]="";//存储希望返回的数据

void (*ConfigConnectDispose)(char *data);//定义一个函数指针变量,用来处理模块返回的数据


/**
* @brief  发送指令配置模块,非阻塞版
* @waring 
* @param  None 
* @param  None
* @param  None
* @retval None
* @example 
**/
void ConfigModuleNoBlock(void)
{
	if(ConfigModuleNoBlockCnt>CompareValue && ConfigModuleNoBlockFlage==0)
	{
		ConfigModuleNoBlockCnt=0;
		if(DataReturnFlage == 1)//上一条指令是OK的
		{
			Cnt=0;
			DataReturnFlage = 0;
			ConfigModuleNoBlockCaseValue ++;//执行下一条
		}
		else
		{
			Cnt ++;
			if(Cnt>=3)//超过3次重新执行
			{
				Cnt=0;
				ConfigModuleNoBlockCaseValue = 0;
			}
		}
		switch(ConfigModuleNoBlockCaseValue)
		{
			case 0://关闭WIFI模块
				Rst8266PinOut=0;DataReturnFlage=1;ConfigModuleNoBlockCnt = CompareValue-100; break;//
			case 1://启动WIFI模块
				Rst8266PinOut=1;DataReturnFlage=1;break;
			case 2://退出透传 																									
				SendConfigFunction("+++",NULL,"+++",NULL,FunctionParseCompare,CompareValue-100);break;
			case 3://关闭回显                                      					
				SendConfigFunction("ATE0\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 4://连接TCP                                
				MainLen = sprintf(MainBuffer,"AT+SAVETRANSLINK=1,\"%s\",%s,\"TCP\"\r\n",IP,Port);
				MainBuffer[MainLen]=0;
				SendConfigFunction(MainBuffer,NULL,"OK","ALREADY CONNECTED",FunctionParseCompare,CompareValue);break;
			case 5://关闭WIFI模块
				Rst8266PinOut=0;DataReturnFlage=1;ConfigModuleNoBlockCnt = CompareValue-500;break;
			case 6://启动WIFI模块
				Rst8266PinOut=1;DataReturnFlage=1;break;
			default: 
				SendConfigFunction(NULL,NULL,NULL,NULL,NULL,CompareValue);//这句必须加,清除所有的执行函数
				DataReturnFlage=0; 
				ConfigModuleNoBlockFlage=1; break;
		}
	}
}


/**
* @brief  发送配置指令函数
* @param  order          发送数据(字符串)
* @param  FunctionSend   发送数据(运行发送数据函数)
* @param  HopeReturn1    接收数据(希望返回的字符串1)
* @param  HopeReturn2    接收数据(希望返回的字符串2)
* @param  FunctionParse  数据处理函数
* @param  ConfigFunctionValue  控制下一条数据的发送时间,取值为 ConfigFunctionCntCom:下一条立即发送
* @retval 
* @example 
**/
void SendConfigFunction(char *order,void (*FunctionSend)(),char *HopeReturn1,char *HopeReturn2,void (*FunctionParse)(char *data),u32 ConfigFunctionValue)
{
	memset(HopeReturnData1,NULL,strlen(HopeReturnData1));
	memset(HopeReturnData2,NULL,strlen(HopeReturnData2));
	if(HopeReturn1!=NULL)  sprintf(HopeReturnData1,"%s",HopeReturn1);//拷贝数据到数组 HopeReturn1,希望返回的数据1
	if(HopeReturn2!=NULL)  sprintf(HopeReturnData2,"%s",HopeReturn2);//拷贝数据到数组 HopeReturn1,希望返回的数据2
	if(FunctionSend!=NULL) FunctionSend();//调用一个函数发送指令
	
	ConfigConnectDispose = FunctionParse;//传递处理函数指针
	SendNextDelay = ConfigFunctionValue;//如果上一条数据处理成功,控制发送下一条数据的时间
	
	if(order!=NULL)printf("%s",order);//直接发送命令
}


/**
* @brief  处理配置返回数据:判断是否返回想要的数据
* @param  data
* @param  
* @retval 
* @example 
**/
void FunctionParseCompare(char *data)
{
	if(strlen(HopeReturnData1) != 0 && strstr(data, HopeReturnData1))
	{
		DataReturnFlage=1;
		ConfigModuleNoBlockCnt = (SendNextDelay == 0 ? ConfigModuleNoBlockCnt:SendNextDelay);
	}
	if(strlen(HopeReturnData2) != 0 && strstr(data, HopeReturnData2))
	{
		DataReturnFlage=1;
		ConfigModuleNoBlockCnt = (SendNextDelay == 0 ? ConfigModuleNoBlockCnt:SendNextDelay);
	}
}


