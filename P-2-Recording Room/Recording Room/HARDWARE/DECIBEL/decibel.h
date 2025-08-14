#ifndef __DECIBEL_H
#define __DECIBEL_H
#include "sys.h"
#include "adc.h"

#define DECIBEL ADC_CHANNEL_11

#define DECIBEL_MIN_ADC 600    // 最小ADC值
#define DECIBEL_MAX_ADC 4095 // 最大ADC值
#define DECIBEL_MIN   10   // 对应的最小分贝值
#define DECIBEL_MAX   180   // 对应的最大分贝值

u8 DECIBEL_GET(void);

#endif
