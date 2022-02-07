#ifndef FLASH_HELP_H_
#define FLASH_HELP_H_
#include <stm32f10x.h>

#ifndef FLASH_HELP_C_
#define FLASH_HELP_Cx_ extern
#else
#define FLASH_HELP_Cx_
#endif

#define FLASH_HELP_PAGE_SIZE  1024 //Flash最小擦除单元字节个数



#endif