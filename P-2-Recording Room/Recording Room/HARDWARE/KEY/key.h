#ifndef _KEY_H
#define _KEY_H
#include "sys.h"

#define KEY0        HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4)  //KEY0°´¼üPE4
#define KEY1        HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_3)  //KEY1°´¼üPE3
#define WK_UP       HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)  //WKUP°´¼üPA0

#define KEY0_PRES 	1
#define KEY1_PRES	  2
#define WKUP_PRES   3

void KEY_Init(void);
u8 KEY_Scan(u8 mode);
#endif
