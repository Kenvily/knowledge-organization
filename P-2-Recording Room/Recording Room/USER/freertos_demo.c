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

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
#include "freertos_demo.h"

TaskHandle_t StartTask_Handler; /* 任务句柄 */
TaskHandle_t Task1_Handler;     /* 任务句柄 */
TaskHandle_t Task2_Handler;     /* 任务句柄 */
TaskHandle_t Task3_Handler;     /* 任务句柄 */

u8 key3_temp = 5;
u16 crcErrorTimes = 0;
extern Data data;
extern u8 sendBackFlag;

/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_start(void)
{
    xTaskCreate((TaskFunction_t)start_task,          /* 任务函数 */
                (const char *)"start_task",          /* 任务名称 */
                (uint16_t)START_STK_SIZE,            /* 任务堆栈大小 */
                (void *)NULL,                        /* 传入给任务函数的参数 */
                (UBaseType_t)START_TASK_PRIO,        /* 任务优先级 */
                (TaskHandle_t *)&StartTask_Handler); /* 任务句柄 */

    vTaskStartScheduler(); // 开启任务调度
}

/**
 * @brief       start_task
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL(); /* 进入临界区 */

    /* 创建任务1 */
    xTaskCreate((TaskFunction_t)task1,
                (const char *)"task1",
                (uint16_t)TASK1_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)TASK1_PRIO,
                (TaskHandle_t *)&Task1_Handler);

    /* 创建任务2 */
    xTaskCreate((TaskFunction_t)task2,
                (const char *)"task2",
                (uint16_t)TASK2_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)TASK2_PRIO,
                (TaskHandle_t *)&Task2_Handler);

    /* 创建任务3 */
    xTaskCreate((TaskFunction_t)task3,
                (const char *)"task3",
                (uint16_t)TASK3_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)TASK3_PRIO,
                (TaskHandle_t *)&Task3_Handler);

    vTaskDelete(StartTask_Handler); /* 删除开始任务 */
    taskEXIT_CRITICAL();            /* 退出临界区 */
}

/**
 * @brief       任务1
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task1(void *pvParameters)
{
    u8 servo_flag = 0;
    u8 key; // 按键
    while (1)
    {
        /* 实时时钟显示 */
        RTC_Show_Time();

        /* 采集显示并发送数据 */
        // taskENTER_CRITICAL();                               /* 进入临界区 */
        DHT11_Read_Data(&data.temperature, &data.humidity); // 读取温湿度值

        LCD_ShowNum(30 + 48 + 50, 120, data.temperature, 2, 16); // 显示温度
        LCD_ShowNum(30 + 48 + 50, 140, data.humidity, 2, 16);    // 显示湿度

        data.smoke = GASE_GET_EX();
        LCD_ShowNum(30 + 48 + 50, 160, data.smoke, 2, 16); // 显示烟雾浓度

        data.alcohol = GASE_GET_ET();
        LCD_ShowNum(30 + 48 + 50, 180, data.alcohol, 2, 16); // 显示酒精浓度

        // 发送温湿度、烟雾酒精浓度数据
        data.music = DECIBEL_GET();
        LCD_ShowNum(30 + 40 + 50, 200, data.music, 3, 16); // 显示分贝
        // taskEXIT_CRITICAL();                               /* 退出临界区 */

        /* 按键控制外设 */
        key = KEY_Scan(0); // 按键扫描
        if (key == 1)
        {
            LED_WHITE_TOGGLE(); // 白灯开关
        }
        else if (key == 2)
        {
            key3_temp %= 40;
            SERVO_TOGGLE(key3_temp); // 舵机开关
            key3_temp += 20;
        }
        else if (key == 3)
        {
            BUZZER_TOGGLE(); // 蜂鸣器开关
        }

        /* 串口控制外设 */
        if (data.led_white_mode == 1)
        {
            LED_WHITE_TOGGLE(); // 白灯开关
            data.led_white_mode = 0;
        }
        // else if (data.led_white_mode == 1)
        // {
        // 	LED_WHITE_ON(); // 白灯开关
        // }
        if (data.servo_mode == 1)
        {
            key3_temp %= 40;
            SERVO_TOGGLE(key3_temp); // 舵机开关
            key3_temp += 20;
            data.servo_mode = 0;
        }
        // else if (data.servo_mode == 2)
        // {
        // 	SERVO_BACKWORD(); // 舵机开关
        // 	data.servo_mode = 0;
        // }
        if (data.buzzer_mode == 1)
        {
            BUZZER_TOGGLE(); // 蜂鸣器开关
            data.buzzer_mode = 0;
        }
        // else if (data.buzzer_mode == 1)
        // {
        // 	BUZZER_ON(); // 蜂鸣器开关
        // }

        /* 火灾时自动开启窗户并报警 */
        if (data.smoke > 25)
        {
            LED_GREEN_OFF();
            LED_RED_ON();
            if (servo_flag == 0)
            {
                key3_temp %= 40;
                SERVO_TOGGLE(key3_temp); // 舵机开关
                key3_temp += 20;
            }
            servo_flag = 1;
        }
        /* 酒精超标时红灯闪烁 */
        if (data.alcohol > 45)
        {
            LED_GREEN_OFF();
            LED_RED_TOGGLE();
            BUZZER_ON();
            vTaskDelay(50);
        }
        else if (data.alcohol < 35 && data.smoke < 20)
        {
            LED_GREEN_ON();
            LED_RED_OFF();
            servo_flag = 0;
        }

        // 串口相应与错误处理
        if (sendBackFlag == 1 || sendBackFlag == 2)
        {
            vTaskResume(Task3_Handler); // 开启任务3
        }
    }
}

/**
 * @brief       task2
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task2(void *pvParameters)
{
    u16 count = 0;
    while (1)
    {
        // 发送分贝数据
        if (count > 5)
        {
						taskENTER_CRITICAL(); /* 进入临界区 */
            UART_SendData_L(data.temperature, data.humidity, data.smoke, data.alcohol);
            taskEXIT_CRITICAL();            /* 退出临界区 */
						count = 0;
        }
        count++;
				taskENTER_CRITICAL(); /* 进入临界区 */
        UART_SendData_H(data.music);
				taskEXIT_CRITICAL();            /* 退出临界区 */
        vTaskDelay(200);
    }
}

// char task_buff[524];
/**
 * @brief       task3
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task3(void *pvParameters)
{
    // vTaskList(task_buff);
    // UART_Printf("%s\r\n", task_buff);

    while (1)
    {
        /* 串口ACK与错误处理 */
        LCD_ShowNum(170, 280, crcErrorTimes, 5, 16); // 显示串口通信出错次数
        if (sendBackFlag == 1)                       // ACK
        {
            ACK();
            sendBackFlag = 0;
        }
        else if (sendBackFlag == 2)
        {
            crcErrorTimes++;
            sendBackFlag = 0;
        }
        vTaskSuspend(Task3_Handler); // 任务挂起
    }
}
