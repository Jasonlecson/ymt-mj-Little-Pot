
#include "parse.h"
#include "string.h"
#include <stdio.h>
#include "cString.h"
#include <stdlib.h>
#include "DMA.h"
#include "System.h"
#include "IAP.h"
#include "delay.h"


u8 CRC8Table[256]={
  0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
  157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
  35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
  190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
  70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
  219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
  101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
  248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
  140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
  17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
  175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
  50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
  202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
  87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
  233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
  116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
};


char *split_buff[MAXList] = {0};

Resolve_Typedef Resolve_Data;
/*解析数据*/
int data_parse(data_parse_struct *data_parse_struct_t,u8 *data){
	int len,i,j;
	long long value;
	
	len = split((char*)data," ",split_buff,MAXList);//以空格分隔数据
	
	if(len!=0){
		if(memcmp(split_buff[0],"down",4) ==0){//普通通信数据
			if(memcmp(split_buff[1],"set_properties",14) ==0){//down set_properties siid piid 0
				data_parse_struct_t->message_type = set_properties;
				j = ((len-2)/3);
				for(i=0;i<j;i++)
				{
					memset(data_parse_struct_t->siid[i],0,MAXDataLen);
					memset(data_parse_struct_t->piid[i],0,MAXDataLen);
					memset(data_parse_struct_t->value[i],0,MAXDataLen);
					
					memcpy(data_parse_struct_t->siid[i],split_buff[(i*3)+2],strlen(split_buff[(i*3)+2]));//获取siid
					memcpy(data_parse_struct_t->piid[i],split_buff[(i*3)+3],strlen(split_buff[(i*3)+3]));//获取piid
					memcpy(data_parse_struct_t->value[i],split_buff[(i*3)+4],strlen(split_buff[(i*3)+4]));//获取value
				}
				return j;
			}
			else if(memcmp(split_buff[1],"get_properties",14) ==0){//down get_properties siid piid
				data_parse_struct_t->message_type = get_properties;
				j = (len-2)>>1;
				for(i=0;i<j;i++)
				{
					memset(data_parse_struct_t->siid[i],0,MAXDataLen);
					memset(data_parse_struct_t->piid[i],0,MAXDataLen);
					
					memcpy(data_parse_struct_t->siid[i],split_buff[(i<<1)+2],strlen(split_buff[(i<<1)+2]));//获取siid
					memcpy(data_parse_struct_t->piid[i],split_buff[(i<<1)+3],strlen(split_buff[(i<<1)+3]));//获取piid
				}
				return j;
			}
			else if(memcmp(split_buff[1],"MIIO_net_change",15) ==0){//MIIO_net_change updating
				if(memcmp(split_buff[2],"updating",8) ==0){
					IAPSetUpdateFlage();
					delay_ms(50);
					Reset_MCU();
				}
			}
			else if(memcmp(split_buff[1],"update_fw",9) ==0){//update_fw
					IAPSetUpdateFlage();
					delay_ms(50);
					Reset_MCU();
			}
			else if(memcmp(split_buff[1],"power_off",9) ==0){//power_off
				data_parse_struct_t->message_type = power_off;
			}
			else if(memcmp(split_buff[1],"MIIO_mcu_version_req",20) ==0){//MIIO_mcu_version_req
				data_parse_struct_t->message_type = mcu_version_req;
			}
			else if(memcmp(split_buff[1],"none",4) ==0){//none
				
			}
			else if(memcmp(split_buff[1],"action",4) ==0){//none
				
			}
			else{//
				memset(data_parse_struct_t->siid[0],0,MAXDataLen);	
				memcpy(data_parse_struct_t->siid[0],split_buff[1],strlen(split_buff[1]));
				data_parse_struct_t->message_type = error_code;
			}
		}
//		else if(memcmp(split_buff[0],"offline",7) ==0){
//			//offline  local cloud  updating  uap  unprov
//			data_parse_struct_t->message_type = net_status;
//			data_parse_struct_t->net_status = net_status_offline;
//		}
//		else if(memcmp(split_buff[0],"cloud",5) ==0){
//			//offline  local cloud  updating  uap  unprov
//			data_parse_struct_t->message_type = net_status;
//			data_parse_struct_t->net_status = net_status_cloud;
//		}
//		else if(memcmp(split_buff[0],"updating",8) ==0){
//			//offline  local cloud  updating  uap  unprov
//			data_parse_struct_t->message_type = net_status;
//			data_parse_struct_t->net_status = net_status_updating;
//		}else if(memcmp(split_buff[0],"local",5) ==0){
//			//offline  local cloud  updating  uap  unprov
//			data_parse_struct_t->message_type = net_status;
//			data_parse_struct_t->net_status = net_status_local;
//		}
//		else if(memcmp(split_buff[0],"uap",3) ==0){
//			//offline  local cloud  updating  uap  unprov
//			data_parse_struct_t->message_type = net_status;
//			data_parse_struct_t->net_status = net_status_uap;
//		}
		else
		{
			len = strlen(split_buff[0]);
			value = atoi(split_buff[0]);
			if(len == 10 && value!=0 && value > 1592113818){//时间戳
				data_parse_struct_t->message_type = get_time_stamp;
				data_parse_struct_t->timestamp = value;
			}
			return 1;
		}
	}
	else{
		return -1;
	}
	return 0;
}



int MinMaxParse(data_parse_struct *data_parse_struct_t,u8 *data){
	int len,value;
	len = strlen((char*)data);
	if(len==0){
		return 0;
	}
	if(*data=='\"' && *(data+len-1)=='\"'){
		*data = 0;
		*(data+len-1)=0;
		
		
		len = split((char*)(data+1),",",split_buff,MAXList);//分隔数据
		if(len==2){
			value = atoi(split_buff[0]);
			if(value==0){
				if(*split_buff[0]!='0'){
					return -2;
				}
			}
			data_parse_struct_t->MinValue = value;
			
			value = atoi(split_buff[1]);
			if(value==0){
				if(*split_buff[1]!='0'){
					return -3;
				}
			}
			data_parse_struct_t->MaxValue = value;
			return 2;
		}
		else{
			return -4;
		}
	}
	else{
		return -1;
	}
}



/**
* @brief  排序函数
* @param  *table--要排序的数组首地址
* @param  DataNumber--要排序的数据个数
* @param  
* @param  
* @param  LostNumber -- 左右丢弃个数
* @retval 排序丢弃后剩余的和
* @example 
**/
u16 SortMethod(u16 *r,u16 DataNumber,u8 LostNumber)
{
	u16 low = 0;   
	u16 high= DataNumber -1; //设置变量的初始值  
	u16 tmp,j;  
	u16 sum = 0;
	while (low < high) {  
			for (j= low; j< high; ++j) //正向冒泡,找到最大者  
					if (r[j]> r[j+1]) {  
							tmp = r[j]; r[j]=r[j+1];r[j+1]=tmp;  
					}   
			--high;                 //修改high值, 前移一位  
			for ( j=high; j>low; --j) //反向冒泡,找到最小者  
					if (r[j]<r[j-1]) {  
							tmp = r[j]; r[j]=r[j-1];r[j-1]=tmp;  
					}  
			++low;                  //修改low值,后移一位  
	} 
  for(j=LostNumber;j<DataNumber-LostNumber;j++)
	{
    sum+=r[j];
  }	
	
	return sum;
}

/**
* @brief  排序函数--2
* @param  *table--要排序的数组首地址--二维数组
* @param  DataNumber--要排序的数据个数
* @param  list -- 要排序的列
* @param  
* @param  LostNumber -- 左右丢弃个数
* @retval 排序丢弃后剩余的和
* @example 
**/
//u16 SortMethod2(u16 (*r)[GatherList],u16 DataNumber,u8 list,u8 LostNumber)
//{
//	u16 low = 0;   
//	u16 high= DataNumber -1; //设置变量的初始值  
//	u16 tmp,j;  
//	u16 sum = 0;
////	if(list==0)PBout(1) = 0;
//	while (low < high) {  
//			for (j= low; j< high; ++j) //正向冒泡,找到最大者  
//					if (r[j][list]> r[j+1][list]) {  
//							tmp = r[j][list]; r[j][list]=r[j+1][list];r[j+1][list]=tmp;  
//					}   
//			--high;                 //修改high值, 前移一位  
//			for ( j=high; j>low; --j) //反向冒泡,找到最小者  
//					if (r[j][list]<r[j-1][list]) {  
//							tmp = r[j][list]; r[j][list]=r[j-1][list];r[j-1][list]=tmp;  
//					}  
//			++low;                  //修改low值,后移一位  
//	} 
//  for(j=LostNumber;j<DataNumber-LostNumber;j++)
//	{
//    sum+=r[j][list];
//  }	
//	
//	return sum;
//}




/**
* @brief  CRC8
* @param  p:源数据
* @param  counter:数据长度
* @param  None
* @retval None
* @example 
**/
u8 CRC8_Table(u8 *p, u16 counter)
{
    u8 crc8 = 0;
 
    for( ; counter > 0; counter--){
        crc8 = CRC8Table[crc8^*p];
        p++;
    }
    return(crc8);
}
