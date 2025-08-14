#ifndef _LED_H
#define _LED_H
#include "sys.h"

void LED_Init(void); // LED初始化

#define LED_RED_OFF()                                   \
  {                                                     \
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET); \
  } // PB5置1，关闭红灯
#define LED_GREEN_OFF()                                 \
  {                                                     \
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET); \
  } // PE5置1，关闭绿灯

#define LED_RED_ON()                                      \
  {                                                       \
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET); \
  } // PB5置0，开启红灯
#define LED_GREEN_ON()                                    \
  {                                                       \
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET); \
  } // PE5置0，开启绿灯

#define LED_RED_TOGGLE()                   \
  {                                        \
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5); \
  } // 红灯翻转
#define LED_GREEN_TOGGLE()                 \
  {                                        \
    HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_5); \
  } // 绿灯反转

#define LED_WHITE_OFF()                                 \
  {                                                     \
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_SET); \
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET); \
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET); \
  }

#define LED_WHITE_ON()                                    \
  {                                                       \
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET); \
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_RESET); \
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET); \
  }

#define LED_WHITE_TOGGLE()                   \
  {                                        \
    HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_0); \
    HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_1); \
    HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_2); \
  } // 白灯翻转

#endif
