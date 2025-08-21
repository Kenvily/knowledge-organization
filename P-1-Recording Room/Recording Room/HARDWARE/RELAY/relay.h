#ifndef _RELAY_H
#define _RELAY_H
#include "sys.h"

void RELAY_Init(void); // 继电器初始化

#define RELAY_DEV1_ON()                                 \
  {                                                     \
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_12, GPIO_PIN_SET); \
  }

#define RELAY_DEV1_OFF()                                  \
  {                                                       \
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_12, GPIO_PIN_RESET); \
  }

#define RELAY_DEV2_ON()                                 \
  {                                                     \
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_13, GPIO_PIN_SET); \
  }

#define RELAY_DEV2_OFF()                                  \
  {                                                       \
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_13, GPIO_PIN_RESET); \
  }

#define RELAY_DEV3_ON()                                 \
  {                                                     \
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_14, GPIO_PIN_SET); \
  }

#define RELAY_DEV3_OFF()                                  \
  {                                                       \
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_14, GPIO_PIN_RESET); \
  }

#define RELAY_DEV4_ON()                                 \
  {                                                     \
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_15, GPIO_PIN_SET); \
  }

#define RELAY_DEV4_OFF()                                  \
  {                                                       \
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_15, GPIO_PIN_RESET); \
  }

#endif
