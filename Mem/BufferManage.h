#ifndef BUFFMANAGE_H_
#define BUFFMANAGE_H_

#ifndef BUFFMANAGE_C_//如果没有定义
#define BUFFMANAGE_Cx_ extern
#else
#define BUFFMANAGE_Cx_
#endif

#include <stm32f10x.h>
#include "LoopList.h"


typedef struct{
	signed int  Count;
	signed int  Cnt;
	unsigned char ReadFlage;
	unsigned char SendFlage;
	
	signed int  ReadLen;
	signed int  SendLen;	//用户可自由使用以上变量
		
	signed int  Len;	//内部使用,用户不可使用
	rb_t Buff;         //管理:缓存数据数组
	rb_t ManageBuff;   //管理:每次缓存数据的数组
}buff_manage_struct;


BUFFMANAGE_Cx_ buff_manage_struct buff_manage_struct_t;//发送数据缓存管理
//BUFFMANAGE_Cx_ buff_manage_struct buff_manage_struct_t1;//串口接收数据缓存管理
BUFFMANAGE_Cx_ buff_manage_struct buff_manage_struct_t2;//串口接收数据缓存管理

int32_t BufferManageCreate(buff_manage_struct *bms,void *buff,uint32_t BuffLen,void *ManageBuff,uint32_t ManageBuffLen);
int32_t BufferManageWrite(buff_manage_struct *bms,void *buff,uint32_t BuffLen);
int32_t BufferManageRead(buff_manage_struct *bms,void *buff);

#endif

