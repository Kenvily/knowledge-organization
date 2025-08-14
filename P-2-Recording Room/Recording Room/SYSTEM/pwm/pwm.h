#ifndef _PWM_H
#define _PWM_H
#include "sys.h"

void PWM_Init(u16 arr,u16 psc);
void PWM_SetTIM2Compare1(u32 compare);
#endif

