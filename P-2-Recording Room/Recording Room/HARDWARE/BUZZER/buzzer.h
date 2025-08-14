#ifndef _BUZZER_H
#define _BUZZER_H

#include "sys.h"

void BUZZER_Init(void); // 蜂鸣器初始化

#define BUZZER_ON()                                     \
  {                                                     \
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); \
  } // PE0置1，蜂鸣器工作
#define BUZZER_OFF()                                      \
  {                                                       \
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); \
  } // PE0置0，蜂鸣器不工作

#define BUZZER_TOGGLE()                   \
  {                                        \
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); \
  } // 蜂鸣器开关

#endif
