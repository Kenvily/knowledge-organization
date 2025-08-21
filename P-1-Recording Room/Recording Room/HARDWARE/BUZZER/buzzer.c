#include "buzzer.h"

// 初始化PE0为输出.并使能时钟
// BUZZER IO初始化
void BUZZER_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();                         // 开启GPIOA时钟

  GPIO_InitTypeDef GPIO_Initure;

  GPIO_Initure.Pin   = GPIO_PIN_5;                      // PA5
  GPIO_Initure.Mode  = GPIO_MODE_OUTPUT_PP;             // 推挽输出
  GPIO_Initure.Pull  = GPIO_PULLUP;                     // 上拉输入
  GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;            // 高速

  HAL_GPIO_Init(GPIOA, &GPIO_Initure);

  BUZZER_OFF();                                           // PA5置0，默认初始化后蜂鸣器不工作
}
