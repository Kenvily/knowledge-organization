#ifndef __GASE_H
#define __GASE_H
#include "sys.h"
#include "adc.h"

#define GASE_EX ADC_CHANNEL_10
#define GASE_ET ADC_CHANNEL_12

u8 GASE_GET_ET(void);
u8 GASE_GET_EX(void);
  
#endif
