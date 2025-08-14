#ifndef __ADC_H
#define __ADC_H
#include "sys.h"
	
void ADC_Init(void); 				//ADC通道初始化
u16  Get_Adc(u32 ch); 		        //获得某个通道值 
#endif 
