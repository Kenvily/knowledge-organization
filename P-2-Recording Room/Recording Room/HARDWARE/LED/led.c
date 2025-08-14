#include "led.h"

// 初始化PB1为输出.并使能时钟
// LED IO初始化
void LED_Init(void)
{
  __HAL_RCC_GPIOB_CLK_ENABLE(); // 开启GPIOB时钟
  __HAL_RCC_GPIOE_CLK_ENABLE(); // 开启GPIOE时钟

  GPIO_InitTypeDef GPIO_Initure;

  GPIO_Initure.Pin = GPIO_PIN_5;             // PB5
  GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;   // 推挽输出
  GPIO_Initure.Pull = GPIO_PULLUP;           // 上拉输入
  GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH; // 高速
  HAL_GPIO_Init(GPIOB, &GPIO_Initure);

  GPIO_Initure.Pin = GPIO_PIN_5; // PE5
  HAL_GPIO_Init(GPIOE, &GPIO_Initure);

  GPIO_Initure.Pin = GPIO_PIN_0; // PE0
  HAL_GPIO_Init(GPIOE, &GPIO_Initure);

  GPIO_Initure.Pin = GPIO_PIN_1; // PE1
  HAL_GPIO_Init(GPIOE, &GPIO_Initure);

  GPIO_Initure.Pin = GPIO_PIN_2; // PE2
  HAL_GPIO_Init(GPIOE, &GPIO_Initure);

  LED_RED_OFF();    // PB5置1，默认初始化后灯灭
  LED_GREEN_ON();   // PE5置1，默认初始化后灯亮
  LED_WHITE_OFF();  // 默认关闭白灯
}


