#include "relay.h"

void RELAY_Init()
{
  __HAL_RCC_GPIOF_CLK_ENABLE(); // 开启GPIOB时钟

  GPIO_InitTypeDef GPIO_Initure;

  GPIO_Initure.Pin = GPIO_PIN_12;             // PB5
  GPIO_Initure.Mode = GPIO_MODE_OUTPUT_OD;   // 推挽输出
  GPIO_Initure.Pull = GPIO_PULLUP;           // 上拉输入
  GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH; // 高速
  HAL_GPIO_Init(GPIOF, &GPIO_Initure);

  GPIO_Initure.Pin = GPIO_PIN_13; // PE5
  HAL_GPIO_Init(GPIOF, &GPIO_Initure);

  GPIO_Initure.Pin = GPIO_PIN_14; // PE0
  HAL_GPIO_Init(GPIOF, &GPIO_Initure);

  GPIO_Initure.Pin = GPIO_PIN_15; // PE1
  HAL_GPIO_Init(GPIOF, &GPIO_Initure);

  RELAY_DEV1_OFF();
}


