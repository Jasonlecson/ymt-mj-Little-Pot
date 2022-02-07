#ifndef __PARSE_H_
#define __PARSE_H_
#include <stm32f10x.h>
#include "DMA.h"


#ifndef __PARSE_C_
#define __PARSE_EX_ extern
#else
#define __PARSE_EX_
#endif


typedef union Resolve
{
  long long lld_data;
  u16  u16_table[4];
}Resolve_Typedef;

__PARSE_EX_ Resolve_Typedef Resolve_Data;

#define MAXList 150
#define MAXDataLen 30

typedef enum {/*接收的消息类型*/
	set_type_none = 0,
  set_properties = 1,
  get_properties = 2,
	get_time_stamp = 3,//时间戳
	net_status = 4,//网络状态
	power_off = 5,//power_off
	mcu_version_req = 6,//mcu_version
	error_code = 7,//error_code
}message_type_typedef;


typedef enum {
	net_none = 0,
	net_status_offline = 1,
  net_status_local = 2,
  net_status_cloud = 3,
	net_status_updating = 4,
	net_status_uap = 5,
	net_status_unprov = 6,
}net_status_typedef;



typedef struct{
	message_type_typedef message_type;
	u8 siid[MAXList/3][MAXDataLen];
	u8 piid[MAXList/3][MAXDataLen];
	u8 value[MAXList/3][MAXDataLen];
	int MinValue;
	int MaxValue;
	long long timestamp;
	net_status_typedef net_status;
}data_parse_struct;

__PARSE_EX_ char *split_buff[MAXList];


int data_parse(data_parse_struct *data_parse_struct_t,u8 *data);
int MinMaxParse(data_parse_struct *data_parse_struct_t,u8 *data);

u16 SortMethod(u16 *r,u16 DataNumber,u8 LostNumber);
//u16 SortMethod2(u16 (*r)[GatherList],u16 DataNumber,u8 list,u8 LostNumber);

/**
* @brief  CRC8
* @param  p:源数据
* @param  counter:数据长度
* @param  None
* @retval None
* @example 
**/
u8 CRC8_Table(u8 *p, u16 counter);

#endif


