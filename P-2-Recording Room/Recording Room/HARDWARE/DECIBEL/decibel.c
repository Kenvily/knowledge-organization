#include "decibel.h"
#include "math.h"

// 检测分贝
// 范围：10——180
u8 DECIBEL_GET()
{

  u8 decibel = ((float)Get_Adc(DECIBEL) - DECIBEL_MIN_ADC) * (DECIBEL_MAX - DECIBEL_MIN) / (DECIBEL_MAX_ADC - DECIBEL_MIN_ADC) + DECIBEL_MIN;
 
  return decibel;
}
 
