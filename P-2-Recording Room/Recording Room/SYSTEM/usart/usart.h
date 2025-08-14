#ifndef __USART_H
#define __USART_H
#include "sys.h"
#include "stdio.h"

#define EN_USART1_RX 1      // 使能（1）/禁止（0）串口1接收
#define RXBUFFERSIZE 1      // 开启中断接收缓存大小
#define USART_REC_LEN 100   // 定义最大接收字节数 100

extern u8 aRxBuffer[RXBUFFERSIZE];     // HAL库USART接收Buffer
extern u8 USART_RX_BUF[USART_REC_LEN]; // 接收缓冲,最大USART_REC_LEN个字节.末字节为换行符

// 如果想串口中断接收，请不要注释以下宏定义
void UART_Init(u32 bound);
void UART_SendByte(u8 Byte);
void UART_SendString(char *String);
void UART_Printf(char *format, ...);
void UART_SendData_L(u8 temperature,u8 humidity,u8 smoke,u8 alcohol);
void UART_SendData_H(u8 music);

u8 calculate_crc(u8 *data, uint16_t length);  //CRC校验

#define ACK()  {UART_SendByte(1);}

typedef struct
{
	u8 music;				// 分贝
	u8 temperature; // 温度
	u8 humidity;		// 湿度
	u8 smoke;				// 烟雾浓度
	u8 alcohol;			// 酒精浓度

	u8 led_white_mode; // 照明灯工作模式
	u8 servo_mode;		 // 舵机工作模式
	u8 buzzer_mode;		 // 蜂鸣器工作模式
}Data;

#endif
