
#include "stmflash.h"
#include "FlashData.h"
#include "parse.h"
#include "flash_helper.h"

//������ֵ
void FlashSetSafe(uint16_t L,uint16_t H,safe_type_typedef safe_type)
{
	u16 ReadDat[2];
	ReadDat[0]=L;ReadDat[1]=H;
	if(safe_type == safe_type_temperature){
		STMFLASH_Write(FLASH_USER_TEMP_ADDR,&ReadDat[0],2);
	}
	else if(safe_type == safe_type_humidity){
		STMFLASH_Write(FLASH_USER_HUMI_ADDR,&ReadDat[0],2);
	}
  else if(safe_type == safe_type_illumination){
		STMFLASH_Write(FLASH_USER_ILL_ADDR,&ReadDat[0],2);
	}
	else if(safe_type == safe_type_fertility){
		STMFLASH_Write(FLASH_USER_FERT_ADDR,&ReadDat[0],2);
	}
	else if(safe_type == safe_type_air_humidity){
		STMFLASH_Write(FLASH_USER_AIR_HUMI_ADDR,&ReadDat[0],2);
	}
}

//��ȡ��ֵ
void FlashGetSafe(uint16_t* L,uint16_t* H,safe_type_typedef safe_type)
{
	u16 ReadDat[2];
	if(safe_type == safe_type_temperature){
		STMFLASH_Read(FLASH_USER_TEMP_ADDR,&ReadDat[0],2);
	}
	else if(safe_type == safe_type_humidity){
		STMFLASH_Read(FLASH_USER_HUMI_ADDR,&ReadDat[0],2);
	}
  else if(safe_type == safe_type_illumination){
		STMFLASH_Read(FLASH_USER_ILL_ADDR,&ReadDat[0],2);
	}
	else if(safe_type == safe_type_fertility){
		STMFLASH_Read(FLASH_USER_FERT_ADDR,&ReadDat[0],2);
	}
	else if(safe_type == safe_type_air_humidity){
		STMFLASH_Read(FLASH_USER_AIR_HUMI_ADDR,&ReadDat[0],2);
	}
	
	if(ReadDat[0] == 65535) ReadDat[0]=0;
	if(ReadDat[1] == 65535) ReadDat[1]=0;
	*L = ReadDat[0];
	*H = ReadDat[1];
}


//����ֲ������
void FlashSetBotanyType(char* ch)
{
	u16 ReadDat[2];
	
	ReadDat[0] = ch[0]; ReadDat[0] = ReadDat[0]<<8|ch[1];
	ReadDat[1] = ch[2]; ReadDat[1] = ReadDat[1]<<8|ch[3];
  STMFLASH_Write(FLASH_USER_BOTANY_ADDR,&ReadDat[0],2);
}

//��ȡֲ������
void FlashGetBotanyType(char* ch)
{
	u16 ReadDat[2];
  STMFLASH_Read(FLASH_USER_BOTANY_ADDR,ReadDat,2);
	
	if(ReadDat[0] == 65535) ReadDat[0]=0;
	if(ReadDat[1] == 65535) ReadDat[1]=0;
	
	ch[0]  = ReadDat[0]>>8;  ch[1] = ReadDat[0]&0x00ff;
	ch[2]  = ReadDat[1]>>8;  ch[3] = ReadDat[1]&0x00ff;
}



//��ˮģʽ:�Զ�
void FlashSetModeAuto(void)
{
	u16 ReadDat = '0';
  STMFLASH_Write(FLASH_USER_WATER_MODE_ADDR,&ReadDat,1);
}
//��ˮģʽ:�ֶ�
void FlashSetModeManual(void)
{
	u16 ReadDat = '1';
  STMFLASH_Write(FLASH_USER_WATER_MODE_ADDR,&ReadDat,1);
}

//��ȡ��ˮģʽ
u16 FlashGetModeWatering(void)
{
	u16 ReadDat = 0;
  STMFLASH_Read(FLASH_USER_WATER_MODE_ADDR,&ReadDat,1);
	return ReadDat == 65535 ? 0 : ReadDat;
}

//ҹ�䲻������ˮ 0:��  1:��
void FlashSetReportEvent(uint16_t data)
{
	u16 ReadDat = data;
  STMFLASH_Write(FLASH_USE_FLASH_REPORTEVENT,&ReadDat,1);
}
//ҹ�䲻������ˮ 0:��  1:��
u16 FlashGetReportEvent(void)
{
	u16 ReadDat = 0;
  STMFLASH_Read(FLASH_USE_FLASH_REPORTEVENT,&ReadDat,1);
	return ReadDat == 65535 ? 0 : ReadDat;
}


//����ƴ�ģʽ 0:�Զ�  1:�ֶ�
void FlashSetFillLightMode(uint16_t data)
{
	u16 ReadDat = data;
  STMFLASH_Write(FLASH_USE_FLASH_FILLLIGHT,&ReadDat,1);
}
//����ƴ�ģʽ 0:�Զ�  1:�ֶ�
u16 FlashGetFillLightMode(void)
{
	u16 ReadDat = 0;
  STMFLASH_Read(FLASH_USE_FLASH_FILLLIGHT,&ReadDat,1);
	return ReadDat == 65535 ? 0 : ReadDat;
}

//��ˮ����:����
void FlashSetWaterCount(uint16_t data,char mode)
{
	u16 ReadDat = data;
	if(mode==0){
		STMFLASH_Write(FLASH_USER_WATER_COUNT_ADDR,&ReadDat,1);
	}
	else{
		flash_helper_struct.FlashHelperData[1] = data;
		FlashHelperUpdate();//ˢ��
	}
}

//��ˮ����:��ȡ
u16 FlashGetWaterCount(char mode)
{
	u16 ReadDat = 0;
	if(mode==0){
		STMFLASH_Read(FLASH_USER_WATER_COUNT_ADDR,&ReadDat,1);
		return ReadDat == 65535 ? 0 : ReadDat;
	}
  else{
		ReadDat = flash_helper_struct.FlashHelperData[1];
		return ReadDat == 65535 ? 0 : ReadDat; 
	}
}


//����ֵ:����
void FlashSetLoveValue(uint16_t data,char mode)
{
	u16 ReadDat = data;
	if(mode==0){
		STMFLASH_Write(FLASH_USER_LOVE_ADDR,&ReadDat,1);
	}
	else{
		flash_helper_struct.FlashHelperData[2] = data;
		FlashHelperUpdate();//ˢ��
	}
}

//����ֵ:��ȡ
u16 FlashGetLoveValue(char mode)
{
	u16 ReadDat = 0;
	if(mode==0){
		STMFLASH_Read(FLASH_USER_LOVE_ADDR,&ReadDat,1);
		return ReadDat == 65535 ? 0 : ReadDat;
	}
	else{
		ReadDat = flash_helper_struct.FlashHelperData[2];
		return ReadDat == 65535 ? 0 : ReadDat; 
	}
}


//�ۼ��ջ�:����
void FlashSetHarvestCount(uint16_t data)
{
	u16 ReadDat = data;
  STMFLASH_Write(FLASH_USER_HARVEST_ADDR,&ReadDat,1);
}

//�ۼ��ջ�:��ȡ
u16 FlashGetHarvestCount(void)
{
	u16 ReadDat = 0;
  STMFLASH_Read(FLASH_USER_HARVEST_ADDR,&ReadDat,1);
	return ReadDat == 65535 ? 0 : ReadDat;
}


////����:����
//void FlashSetMatrix(uint16_t data)
//{
//	u16 ReadDat = data;
//  STMFLASH_Write(FLASH_USER_MATRIX_ADDR,&ReadDat,1);
//}

////����:��ȡ
//u16 FlashGetMatrix(void)
//{
//	u16 ReadDat = 0;
//  STMFLASH_Read(FLASH_USER_MATRIX_ADDR,&ReadDat,1);
//	return ReadDat == 65535 ? 0 : ReadDat;
//}


//��������
void FlashSetNurtureDay(uint16_t data,char mode)
{
	u16 ReadDat = data;
	if(mode==0){
		STMFLASH_Write(FLASH_USER_NURTURE_ADDR,&ReadDat,1);
	}
  else{
		flash_helper_struct.FlashHelperData[7] = data;
		FlashHelperUpdate();//ˢ��
	}
}

//��������
u16 FlashGetNurtureDay(char mode)
{
	u16 ReadDat = 0;
	if(mode==0){
		STMFLASH_Read(FLASH_USER_NURTURE_ADDR,&ReadDat,1);
		return ReadDat == 65535 ? 0 : ReadDat;
	}
	else{
		ReadDat = flash_helper_struct.FlashHelperData[7];
		return ReadDat == 65535 ? 0 : ReadDat; 
	}
}


//������ֲʱ��
void FlashSetDataPlant(long long data)
{
	u16 ReadDat[4];
	Resolve_Data.lld_data = data;
	ReadDat[0] = Resolve_Data.u16_table[0];
	ReadDat[1] = Resolve_Data.u16_table[1];
	ReadDat[2] = Resolve_Data.u16_table[2];
	ReadDat[3] = Resolve_Data.u16_table[3];
	
  STMFLASH_Write(FLASH_USER_DATA_ADDR,&ReadDat[0],4);
}


//��ȡ��ֲʱ��
long long FlashGetDataPlant(void)
{
	u16 ReadDat[4];
  STMFLASH_Read(FLASH_USER_DATA_ADDR,ReadDat,4);
	
	if(ReadDat[0] == 65535 && ReadDat[1] == 65535 && ReadDat[2] == 65535 && ReadDat[3] == 65535){
		 ReadDat[0]=0; ReadDat[1]=0; ReadDat[2]=0; ReadDat[3]=0;
	}
	
	Resolve_Data.u16_table[0] = ReadDat[0];
	Resolve_Data.u16_table[1] = ReadDat[1];
	Resolve_Data.u16_table[2] = ReadDat[2];
	Resolve_Data.u16_table[3] = ReadDat[3];
	
	return Resolve_Data.lld_data;
}


//���ù���ǿ�ȱ���ֵ
void FlashSetLightAlarm(int value)
{
	u16 ReadDat[2];
	ReadDat[0] = (value>>16)&0x0000FFFF;
	ReadDat[1] = value&0x0000FFFF;
	
  STMFLASH_Write(FLASH_USE_FLASH_LIGHTALARM,&ReadDat[0],2);
}

//��ȡ����ǿ�ȱ���ֵ
int FlashGetLightAlarm(void)
{
	u16 ReadDat[2];
	int value=0;
  STMFLASH_Read(FLASH_USE_FLASH_LIGHTALARM,ReadDat,2);
	
	value = ReadDat[0];
	value = ReadDat[0]<<16;
	value = value | ReadDat[1];
	
	if(ReadDat[0] == 0xFFFF && ReadDat[1] == 0xFFFF)
	{
		value = 60000;
	}
	return value;
}


//������ֲʱ��(�״�)
void FlashSetOneDataPlant(long long data)
{
	u16 ReadDat[4];
	Resolve_Data.lld_data = data;
	ReadDat[0] = Resolve_Data.u16_table[0];
	ReadDat[1] = Resolve_Data.u16_table[1];
	ReadDat[2] = Resolve_Data.u16_table[2];
	ReadDat[3] = Resolve_Data.u16_table[3];
	
  STMFLASH_Write(FLASH_USER_ONEDATA_ADDR,&ReadDat[0],4);
}


//��ȡ��ֲʱ��(�״�)
long long FlashGetOneDataPlant(void)
{
	u16 ReadDat[4];
  STMFLASH_Read(FLASH_USER_ONEDATA_ADDR,ReadDat,4);
	
	if(ReadDat[0] == 65535 && ReadDat[1] == 65535 && ReadDat[2] == 65535 && ReadDat[3] == 65535){
		 ReadDat[0]=0; ReadDat[1]=0; ReadDat[2]=0; ReadDat[3]=0;
	}
	
//	if(ReadDat[0] == 65535) ReadDat[0]=0;
//	if(ReadDat[1] == 65535) ReadDat[1]=0;
//	if(ReadDat[2] == 65535) ReadDat[2]=0;
//	if(ReadDat[3] == 65535) ReadDat[3]=0;
	
	Resolve_Data.u16_table[0] = ReadDat[0];
	Resolve_Data.u16_table[1] = ReadDat[1];
	Resolve_Data.u16_table[2] = ReadDat[2];
	Resolve_Data.u16_table[3] = ReadDat[3];
	
	return Resolve_Data.lld_data;
}


//���Ļ���ʱ��
void FlashSetMatrixData(long long data,char mode)
{
	u16 ReadDat[4];
	Resolve_Data.lld_data = data;
	ReadDat[0] = Resolve_Data.u16_table[0];
	ReadDat[1] = Resolve_Data.u16_table[1];
	ReadDat[2] = Resolve_Data.u16_table[2];
	ReadDat[3] = Resolve_Data.u16_table[3];
	
	if(mode==0){
		STMFLASH_Write(FLASH_USER_MATRIX_ADDR,&ReadDat[0],4);
	}
	else{
		flash_helper_struct.FlashHelperData[3] = ReadDat[0];
		flash_helper_struct.FlashHelperData[4] = ReadDat[1];
		flash_helper_struct.FlashHelperData[5] = ReadDat[2];
		flash_helper_struct.FlashHelperData[6] = ReadDat[3];
		FlashHelperUpdate();//ˢ��
	}
}

//��ȡ���Ļ���ʱ��
long long FlashGetMatrixData(char mode)
{
	u16 ReadDat[4];
	if(mode==0){
		STMFLASH_Read(FLASH_USER_MATRIX_ADDR,ReadDat,4);

		if(ReadDat[0] == 65535 && ReadDat[1] == 65535 && ReadDat[2] == 65535 && ReadDat[3] == 65535){
			ReadDat[0]=0; ReadDat[1]=0; ReadDat[2]=0; ReadDat[3]=0;
		}
		
//		if(ReadDat[0] == 65535) ReadDat[0]=0;
//		if(ReadDat[1] == 65535) ReadDat[1]=0;
//		if(ReadDat[2] == 65535) ReadDat[2]=0;
//		if(ReadDat[3] == 65535) ReadDat[3]=0;

		Resolve_Data.u16_table[0] = ReadDat[0];
		Resolve_Data.u16_table[1] = ReadDat[1];
		Resolve_Data.u16_table[2] = ReadDat[2];
		Resolve_Data.u16_table[3] = ReadDat[3];

		return Resolve_Data.lld_data;
	}
  else{
		Resolve_Data.u16_table[0] = flash_helper_struct.FlashHelperData[3];
		Resolve_Data.u16_table[1] = flash_helper_struct.FlashHelperData[4];
		Resolve_Data.u16_table[2] = flash_helper_struct.FlashHelperData[5];
		Resolve_Data.u16_table[3] = flash_helper_struct.FlashHelperData[6];
		return Resolve_Data.lld_data;
	}
}





//���ù���ǿ��ͳ���ϱ�״̬(���)
void FlashClearIlluminationState(void)
{
	u16 ReadDat = 0;
  STMFLASH_Write(FLASH_USE_FLASH_ILLEVENT,&ReadDat,1);
}
//���ù���ǿ��ͳ���ϱ�״̬(����)
void FlashSetIlluminationState(void)
{
	u16 ReadDat = 1;
  STMFLASH_Write(FLASH_USE_FLASH_ILLEVENT,&ReadDat,1);
}

//��ȡ����ǿ��ͳ���ϱ�״̬
u16 FlashGetIlluminationState(void)
{
	u16 ReadDat = 0;
  STMFLASH_Read(FLASH_USE_FLASH_ILLEVENT,&ReadDat,1);
	return ReadDat == 65535 ? 0 : ReadDat;
}



//���Ĺ���ͳ��
//data ͳ�ƵĹ���ǿ��
//count ͳ�ƵĴ���
//time ʱ���
void FlashSetIlluminationStatistics(u32 data,u16 count,long long time)
{
	u16 ReadDat[2];//����ǿ��
	ReadDat[0] = (data>>16)&0xffff;
	ReadDat[1] = data&0xffff;
	flash_helper_struct.FlashHelperData[8] = ReadDat[0];
	flash_helper_struct.FlashHelperData[9] = ReadDat[1];
	//ͳ�ƴ���
	flash_helper_struct.FlashHelperData[10] = count;
	//ʱ���
	Resolve_Data.lld_data = time;
	flash_helper_struct.FlashHelperData[11] = Resolve_Data.u16_table[0];
	flash_helper_struct.FlashHelperData[12] = Resolve_Data.u16_table[1];
	flash_helper_struct.FlashHelperData[13] = Resolve_Data.u16_table[2];
	flash_helper_struct.FlashHelperData[14] = Resolve_Data.u16_table[3];
	
	FlashHelperUpdate();//ˢ��
}
//��ȡ����ͳ��
//count ���ص�ͳ�ƴ���
//time  ���ص�ͳ��ʱ��ʱ���
//return ͳ�ƵĹ���ǿ��
u32 FlashGetIlluminationStatistics(u16* count,long long *time)
{
	u32 ReadDat;//����ǿ��
	ReadDat = flash_helper_struct.FlashHelperData[8];
	ReadDat = ReadDat<<16;
	ReadDat = ReadDat | flash_helper_struct.FlashHelperData[9];
	//ͳ�ƴ���
	*count = flash_helper_struct.FlashHelperData[10];
	//ʱ���
	Resolve_Data.u16_table[0] = flash_helper_struct.FlashHelperData[11];
	Resolve_Data.u16_table[1] = flash_helper_struct.FlashHelperData[12];
	Resolve_Data.u16_table[2] = flash_helper_struct.FlashHelperData[13];
	Resolve_Data.u16_table[3] = flash_helper_struct.FlashHelperData[14];
	*time =  Resolve_Data.lld_data;
	return ReadDat;
}

