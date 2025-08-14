#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "buzzer.h"
#include "dht11.h"
#include "rtc.h"
#include "pwm.h"
#include "servo.h"
#include "adc.h"
#include "gase.h"
#include "decibel.h"

#include "freertos_demo.h"

int main(void)
{
	/* 设备初始化 */
	HAL_Init();                     // 初始化HAL库
	Stm32_Clock_Init(RCC_PLL_MUL9); // 设置时钟,72M
	delay_init(72);                 // 初始化延时函数
	UART_Init(115200);              // 初始化串口

	LCD_Init();          // 初始化LCD FSMC接口
	PWM_Init(200, 7200); // 初始化PWM
	LED_Init();          // 初始化LED
	KEY_Init();          // 初始化按键
	BUZZER_Init();       // 初始化蜂鸣器
	DHT11_Init();        // 初始化DHT11
	RTC_Init();          // 初始化RTC
	ADC_Init();          // 初始化ADC
	LCD_Show_Theme();    // 布局初始化

	/* 开启系统 */
	freertos_start();

}
