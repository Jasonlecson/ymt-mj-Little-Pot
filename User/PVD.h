#ifndef __PVD_H__
#define __PVD_H__
#include <stm32f10x.h>

#ifndef PVD_C_//如果没有定义
#define PVD_Ex_ extern
#else
#define PVD_Ex_
#endif

void PVD_Init(void);


#endif
