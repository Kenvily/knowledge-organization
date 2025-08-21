#include "usart.h"
#include "stdarg.h"
#include "lcd.h"

#if 1
#if (__ARMCC_VERSION >= 6010050)                    /* 使用AC6编译器时 */
__asm(".global __use_no_semihosting\n\t");          /* 声明不使用半主机模式 */
__asm(".global __ARM_use_no_argv \n\t");            /* AC6下需要声明main函数为无参数格式，否则部分例程可能出现半主机模式 */

#else
/* 使用AC5编译器时, 要在这里定义__FILE 和 不使用半主机模式 */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

#endif

/* 不使用半主机模式，至少需要重定义_ttywrch\_sys_exit\_sys_command_string函数,以同时兼容AC6和AC5模式 */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* 定义_sys_exit()以避免使用半主机模式 */
void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}

/* FILE 在 stdio.h里面定义. */
FILE __stdout;

/* 重定义fputc函数, printf函数最终会通过调用fputc输出字符串到串口 */
int fputc(int ch, FILE *f)
{
    while ((USART1->SR & 0X40) == 0);               /* 等待上一个字符发送完成 */

    USART1->DR = (uint8_t)ch;                       /* 将要发送的字符 ch 写入到DR寄存器 */
    return ch;
}
#endif

/**
 * 函    数：串口发送一个字节
 * 参    数：Byte 要发送的一个字节
 * 返 回 值：无
 */
void UART_SendByte(u8 Byte)
{
	USART1->DR = (u8)Byte;
	while ((USART1->SR & 0X40) == 0)
		; // 循环发送,直到发送完毕
}

/**
 * 函    数：串口发送一个字符串
 * 参    数：String 要发送字符串的首地址
 * 返 回 值：无
 */
void UART_SendString(char *String)
{
	u8 i;
	for (i = 0; String[i] != '\0'; i++) // 遍历字符数组（字符串），遇到字符串结束标志位后停止
	{
		UART_SendByte(String[i]); // 依次调用Serial_SendByte发送每个字节数据
	}
}

/**
 * 函    数：自己封装的prinf函数
 * 参    数：format 格式化字符串
 * 参    数：... 可变的参数列表
 * 返 回 值：无
 */
void UART_Printf(char *format, ...)
{
	char String[100];							 // 定义字符数组
	va_list arg;									 // 定义可变参数列表数据类型的变量arg
	va_start(arg, format);				 // 从format开始，接收参数列表到arg变量
	vsprintf(String, format, arg); // 使用vsprintf打印格式化字符串和参数列表到字符数组中
	va_end(arg);									 // 结束变量arg
	UART_SendString(String);			 // 串口发送字符数组（字符串）
}

// u8 music -> 0x1								 	// 分贝
// u8 temperature -> 0x2					 	// 温度
// u8 humidity -> 0x4						 		// 湿度
// u8 smoke -> 0x8								 	// 烟雾浓度
// u8 alcohol -> 0x10						 		// 酒精浓度
void UART_SendData_L(u8 temperature,u8 humidity,u8 smoke,u8 alcohol)
{
	u8 num[11] = {0xFF,0x4,0x2,temperature,0x4,humidity,0x8,smoke,0x10,alcohol,0x0};
	UART_SendByte(0xFF);
	UART_SendByte(0x4);
	UART_SendByte(0x2);
	UART_SendByte(temperature);
	UART_SendByte(0x4);
	UART_SendByte(humidity);
	UART_SendByte(0x8);
	UART_SendByte(smoke);
	UART_SendByte(0x10);
	UART_SendByte(alcohol);
	UART_SendByte(0x0);
	UART_SendByte(calculate_crc(num,11));	//CRC数据位
	UART_SendByte(0x55);
}

void UART_SendData_H(u8 music)
{
	u8 num[5] = {0xFF,0x1,0x1,music,0x0};
	UART_SendByte(0xFF);
	UART_SendByte(0x1);
	UART_SendByte(0x1);
	UART_SendByte(music);
	UART_SendByte(0x0);
	UART_SendByte(calculate_crc(num,5));	//CRC数据位	//CRC数据位
	UART_SendByte(0x55);
}


#if EN_USART1_RX // 如果使能了接收

UART_HandleTypeDef UART1_Handler; // UART句柄
u8 aRxBuffer[RXBUFFERSIZE];				// HAL库USART接收Buffer
u8 USART_RX_BUF[USART_REC_LEN];		// 接收缓冲,最大USART_REC_LEN个字节.末字节为换行符
u8 sendBackFlag = 0;							// 反馈标志位

// 初始化IO 串口1
// bound:波特率
void UART_Init(u32 bound)
{
	// UART 初始化设置
	UART1_Handler.Instance = USART1;										// USART1
	UART1_Handler.Init.BaudRate = bound;								// 波特率
	UART1_Handler.Init.WordLength = UART_WORDLENGTH_8B; // 字长为8位数据格式
	UART1_Handler.Init.StopBits = UART_STOPBITS_1;			// 一个停止位
	UART1_Handler.Init.Parity = UART_PARITY_NONE;				// 无奇偶校验位
	UART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE; // 无硬件流控
	UART1_Handler.Init.Mode = UART_MODE_TX_RX;					// 收发模式
	HAL_UART_Init(&UART1_Handler);											// 使能UART1

	HAL_UART_Receive_IT(&UART1_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE); // 开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
}

// UART底层初始化，时钟使能，引脚配置，中断配置
// 此函数会被HAL_UART_Init()调用
// huart:串口句柄

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	// GPIO端口设置
	GPIO_InitTypeDef GPIO_Initure;

	if (huart->Instance == USART1) // 如果是串口1，进行串口1 MSP初始化
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();	 // 使能GPIOA时钟
		__HAL_RCC_USART1_CLK_ENABLE(); // 使能USART1时钟
		__HAL_RCC_AFIO_CLK_ENABLE();

		GPIO_Initure.Pin = GPIO_PIN_9;						 // PA9
		GPIO_Initure.Mode = GPIO_MODE_AF_PP;			 // 复用推挽输出
		GPIO_Initure.Pull = GPIO_PULLUP;					 // 上拉
		GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH; // 高速
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);			 // 初始化PA9

		GPIO_Initure.Pin = GPIO_PIN_10;					// PA10
		GPIO_Initure.Mode = GPIO_MODE_AF_INPUT; // 模式要设置为复用输入模式！
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);		// 初始化PA10

#if EN_USART1_RX
		HAL_NVIC_EnableIRQ(USART1_IRQn);					// 使能USART1中断通道
		HAL_NVIC_SetPriority(USART1_IRQn, 6, 0); // 抢占优先级10
#endif
	}
}

Data data;

/* Data数据标识符对应表 */
// u8 music -> 0x1								 	// 分贝
// u8 temperature -> 0x2					 	// 温度
// u8 humidity -> 0x4						 		// 湿度
// u8 smoke -> 0x8								 	// 烟雾浓度
// u8 alcohol -> 0x10						 		// 酒精浓度
// u8 led_white_mode -> 0x20 		 		// 照明灯工作模式
// u8 servo_mode -> 0x40			 		 	// 舵机工作模式
// u8 buzzer_mode -> 0x80		 		 	// 蜂鸣器工作模式
// 接受来自上位机数据，所以只需要获取后三个数据即可
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	static u8 RxState = 0;	// 定义表示当前状态机状态的静态变量
	static u8 dataNum = 0;	// 定义表示当前接收数据个数的静态变量
	static u8 dataId = 0;		// 定义表示当前接收数据标识符的静态变量
	static u8 RxLocate = 0; // 定义表示当前接收数据标识符的静态变量

	/*使用状态机的思路，依次处理数据包的不同部分*/	
	/*当前状态为0，接收数据包包头*/
	if (RxState == 0)
	{
		if (aRxBuffer[0] == 0xFF) // 如果数据确实是包头，并且上一个数据包已处理完毕
		{
			USART_RX_BUF[RxLocate] = aRxBuffer[0];
			RxLocate++;
			RxState = 1; // 置下一个状态
		}
	}
	/*当前状态为1，接收数据包数据个数*/
	else if (RxState == 1)
	{
		USART_RX_BUF[RxLocate] = aRxBuffer[0];
		RxLocate++;
		dataNum = aRxBuffer[0]; // 记录待接收的数据个数
		RxState = 2;
	}
	/* 当前状态为2，获取数据标识符 */
	else if (RxState == 2)
	{
		USART_RX_BUF[RxLocate] = aRxBuffer[0];
		RxLocate++;
		dataId = aRxBuffer[0]; // 数据标识符
		RxState = 3;
	}
	/* 当前状态为3，根据数据标识符读取数据 */
	else if (RxState == 3)
	{
		USART_RX_BUF[RxLocate] = aRxBuffer[0];
		RxLocate++;
		switch (dataId)
		{
		case 0x20:
			data.led_white_mode = aRxBuffer[0];
			break;
		case 0x40:
			data.servo_mode = aRxBuffer[0];
			break;
		case 0x80:
			data.buzzer_mode = aRxBuffer[0];
			break;
		default:
			break;
		}
		dataNum--;
		if (dataNum == 0)
		{
			RxState = 4;
		}
		else
		{
			RxState = 2;
		}
	}
	/* 当前状态为4，获取应答位 */
	else if (RxState == 4)
	{
		if (aRxBuffer[0] == 0xAA)
		{
			USART_RX_BUF[RxLocate] = aRxBuffer[0];
			RxLocate++;
			RxState = 5;
		}
	}
	/* 当前状态为5，CRC校验 */
	else if (RxState == 5)
	{
		USART_RX_BUF[RxLocate] = aRxBuffer[0];
		RxLocate++;
		u8 crc = calculate_crc(USART_RX_BUF, RxLocate - 1); // 计算CRC	
		if (USART_RX_BUF[RxLocate-1] == crc)
		{
			RxState = 6;
		}
		else{									// 出错丢弃重发
			RxState = 0;
			RxLocate = 0;
			dataId = 0;
			sendBackFlag = 2;
		}
	}
	else if (aRxBuffer[0] == 0x55)
	{
		RxState = 0;
		RxLocate = 0;
		dataId = 0;
		sendBackFlag = 1;
	}
}

// 串口1中断服务程序
void USART1_IRQHandler(void)
{
	u32 timeout = 0;

	HAL_UART_IRQHandler(&UART1_Handler); // 调用HAL库中断处理公用函数

	timeout = 0;
	while (HAL_UART_GetState(&UART1_Handler) != HAL_UART_STATE_READY) // 等待就绪
	{
		timeout++; ////超时处理
		if (timeout > HAL_MAX_DELAY)
			break;
	}

	timeout = 0;
	while (HAL_UART_Receive_IT(&UART1_Handler,
														 (u8 *)aRxBuffer,
														 RXBUFFERSIZE) != HAL_OK) // 一次处理完成之后，重新开启中断并设置RxXferCount为1
	{
		timeout++; // 超时处理
		if (timeout > HAL_MAX_DELAY)
			break;
	}
}

// 计算CRC
u8 calculate_crc(u8 *data, uint16_t length)
{
	u8 crc = 0xFF;				 // 初始值
	u8 polynomial = 0x07; // CRC-8 多项式

	for (u16 i = 0; i < length; i++)
	{
		crc ^= data[i];
		for (u8 j = 0; j < 8; j++)
		{
			if (crc & 0x80)
			{
				crc = (crc << 1) ^ polynomial;
			}
			else
			{
				crc <<= 1;
			}
		}
	}
	return crc;
}

#endif
