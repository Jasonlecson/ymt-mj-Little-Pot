#ifndef CONFIG8266_H_
#define CONFIG8266_H_

#ifndef CONFIG8266_C_//如果没有定义  _TIME_C_
#define CONFIG8266_EX_ extern
#else
#define CONFIG8266_EX_
#endif

#include <stm32f10x.h>


#define Rst8266PinOut PBout(2)       //复位8266的引脚
#define SmartConfigPinOut PCout(13)  //SmartConfig指示灯引脚

CONFIG8266_EX_ char SmartConfigFlage;//是不是在配网
CONFIG8266_EX_ char SmartConfigSuccess;//是否配网成功
CONFIG8266_EX_ u32  SmartConfigCnt;//配网连接路由器延时使用
CONFIG8266_EX_ u32  Config8266LedDelay;//配置8266指示灯闪耀

char AutoConnectTCP(void);//链接TCP服务器
char SmartConfig(void);//启用SmartConfig 给WIFI配网
void Init8266(void);//连接路由器
void Rst8266(void);//复位8266
#endif

