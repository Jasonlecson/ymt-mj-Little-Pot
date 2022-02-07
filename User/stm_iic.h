#ifndef STM_IIC_H_
#define STM_IIC_H_
#include <stm32f10x.h>
#ifndef STM_IIC_C_
#define STM_IIC_Cx_  extern
#else 
#define STM_IIC_Cx_
#endif

#define I2C1_SPEED 100000  //100KHz
#define I2C1_OwnAddress 0x40


void stm_iic_init(void);
	
#endif