#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "rtc.h"

RTC_HandleTypeDef RTC_Handler; // RTC句柄

_calendar_obj calendar; // 时钟结构体

// 实时时钟配置
// 初始化RTC时钟,同时检测时钟是否工作正常
// BKP->DR1用于保存是否第一次配置的设置
// 返回0:正常
// 其他:错误代码
void RTC_Init(void)
{
  RTC_Handler.Instance = RTC;
  RTC_Handler.Init.AsynchPrediv = 32767; // 时钟周期设置(有待观察,看是否跑慢了?)理论值：32767
  while (1)
  {
    if (HAL_RTC_Init(&RTC_Handler) != HAL_OK)
    {
      LCD_ShowString(30, 40, 200, 16, 16, "RTC ERROR!   ");
      delay_ms(800);
      LCD_ShowString(30, 40, 200, 16, 16, "RTC Trying...");
    }
    break;
  }
	 

  if (HAL_RTCEx_BKUPRead(&RTC_Handler, RTC_BKP_DR1) != 0X5050) // 是否第一次配置
  {
    RTC_Set(2025, 3, 28, 21, 45, 0);
    HAL_RTCEx_BKUPWrite(&RTC_Handler, RTC_BKP_DR1, 0X5050); // 标记已经初始化过了
  }

  __HAL_RTC_ALARM_ENABLE_IT(&RTC_Handler, RTC_IT_SEC);    // 允许秒中断
  HAL_NVIC_SetPriority(RTC_IRQn, 15, 0);            // 抢占优先级15,子优先级0
  HAL_NVIC_EnableIRQ(RTC_IRQn);

  RTC_Get(); // 更新时间
}

// RTC底层驱动，时钟配置
// 此函数会被HAL_RTC_Init()调用
// hrtc:RTC句柄
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

  __HAL_RCC_PWR_CLK_ENABLE(); // 使能电源时钟PWR
  HAL_PWR_EnableBkUpAccess(); // 取消备份区域写保护
  __HAL_RCC_BKP_CLK_ENABLE(); // 使能BSP时钟

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE; // LSE配置
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON; // RTC使用LSE
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC; // 外设为RTC
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE; // RTC时钟源为LSE
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

  __HAL_RCC_RTC_ENABLE(); // RTC时钟使能
}

// RTC时钟中断
// 每秒触发一次
void RTC_IRQHandler(void)
{
  if (__HAL_RTC_ALARM_GET_FLAG(&RTC_Handler, RTC_FLAG_SEC) != RESET) // 秒中断
  {
    __HAL_RTC_ALARM_CLEAR_FLAG(&RTC_Handler, RTC_FLAG_SEC); // 清除秒中断
    RTC_Get();                                              // 更新时间
  }
  __HAL_RTC_ALARM_CLEAR_FLAG(&RTC_Handler, RTC_FLAG_OW); // 清除溢出
}

// 判断是否是闰年函数
// 月份   1  2  3  4  5  6  7  8  9  10 11 12
// 闰年   31 29 31 30 31 30 31 31 30 31 30 31
// 非闰年 31 28 31 30 31 30 31 31 30 31 30 31
// year:年份
// 返回值:该年份是不是闰年.1,是.0,不是
u8 RTC_Is_Leap_Year(u16 year)
{
  if (year % 4 == 0) // 必须能被4整除
  {
    if (year % 100 == 0)
    {
      if (year % 400 == 0)
        return 1; // 如果以00结尾,还要能被400整除
      else
        return 0;
    }
    else
      return 1;
  }
  else
    return 0;
}

// 设置时钟
// 把输入的时钟转换为秒钟
// 以1970年1月1日为基准
// 1970~2099年为合法年份
// 返回值:0,成功;其他:错误代码.
// 月份数据表
u8 const table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5}; // 月修正数据表
// 平年的月份日期表
const u8 mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
// syear,smon,sday,hour,min,sec：年月日时分秒
// 返回值：设置结果。0，成功；1，失败。
u8 RTC_Set(u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec)
{
  u16 t;
  u32 seccount = 0;
  //	RTC_DateTypeDef RTC_DateStructure;
  //	RTC_TimeTypeDef RTC_TimeStructure;

  if (syear < 1970 || syear > 2099)
    return 1;
  for (t = 1970; t < syear; t++) // 把所有年份的秒钟相加
  {
    if (RTC_Is_Leap_Year(t))
      seccount += 31622400; // 闰年的秒钟数
    else
      seccount += 31536000; // 平年的秒钟数
  }
  smon -= 1;
  for (t = 0; t < smon; t++) // 把前面月份的秒钟数相加
  {
    seccount += (u32)mon_table[t] * 86400; // 月份秒钟数相加
    if (RTC_Is_Leap_Year(syear) && t == 1)
      seccount += 86400; // 闰年2月份增加一天的秒钟数
  }
  seccount += (u32)(sday - 1) * 86400; // 把前面日期的秒钟数相加
  seccount += (u32)hour * 3600;        // 小时秒钟数
  seccount += (u32)min * 60;           // 分钟秒钟数
  seccount += sec;                     // 最后的秒钟加上去

  // 设置时钟
  RCC->APB1ENR |= 1 << 28; // 使能电源时钟
  RCC->APB1ENR |= 1 << 27; // 使能备份时钟
  PWR->CR |= 1 << 8;       // 取消备份区写保护
  // 上面三步是必须的!
  RTC->CRL |= 1 << 4; // 允许配置
  RTC->CNTL = seccount & 0xffff;
  RTC->CNTH = seccount >> 16;
  RTC->CRL &= ~(1 << 4); // 配置更新
  while (!(RTC->CRL & (1 << 5)))
    ; // 等待RTC寄存器操作完成

  RTC_Get(); // 设置完之后更新一下数据
  return 0;
}

// 得到当前的时间，结果保存在calendar结构体里面
// 返回值:0,成功;其他:错误代码.
u8 RTC_Get(void)
{
  static u16 daycnt = 0;
  u32 timecount = 0;
  u32 temp = 0;
  u16 temp1 = 0;
  timecount = RTC->CNTH; // 得到计数器中的值(秒钟数)
  timecount <<= 16;
  timecount += RTC->CNTL;

  temp = timecount / 86400; // 得到天数(秒钟数对应的)
  if (daycnt != temp)       // 超过一天了
  {
    daycnt = temp;
    temp1 = 1970; // 从1970年开始
    while (temp >= 365)
    {
      if (RTC_Is_Leap_Year(temp1)) // 是闰年
      {
        if (temp >= 366)
          temp -= 366; // 闰年的秒钟数
        else
          break;
      }
      else
        temp -= 365; // 平年
      temp1++;
    }
    calendar.w_year = temp1; // 得到年份
    temp1 = 0;
    while (temp >= 28) // 超过了一个月
    {
      if (RTC_Is_Leap_Year(calendar.w_year) && temp1 == 1) // 当年是不是闰年/2月份
      {
        if (temp >= 29)
          temp -= 29; // 闰年的秒钟数
        else
          break;
      }
      else
      {
        if (temp >= mon_table[temp1])
          temp -= mon_table[temp1]; // 平年
        else
          break;
      }
      temp1++;
    }
    calendar.w_month = temp1 + 1; // 得到月份
    calendar.w_date = temp + 1;   // 得到日期
  }
  temp = timecount % 86400;                                                         // 得到秒钟数
  calendar.hour = temp / 3600;                                                      // 小时
  calendar.min = (temp % 3600) / 60;                                                // 分钟
  calendar.sec = (temp % 3600) % 60;                                                // 秒钟
  calendar.week = RTC_Get_Week(calendar.w_year, calendar.w_month, calendar.w_date); // 获取星期
  return 0;
}
// 获得现在是星期几
// 功能描述:输入公历日期得到星期(只允许1901-2099年)
// year,month,day：公历年月日
// 返回值：星期号
u8 RTC_Get_Week(u16 year, u8 month, u8 day)
{
  u16 temp2;
  u8 yearH, yearL;

  yearH = year / 100;
  yearL = year % 100;
  // 如果为21世纪,年份数加100
  if (yearH > 19)
    yearL += 100;
  // 所过闰年数只算1900年之后的
  temp2 = yearL + yearL / 4;
  temp2 = temp2 % 7;
  temp2 = temp2 + day + table_week[month - 1];
  if (yearL % 4 == 0 && month < 3)
    temp2--;
  return (temp2 % 7);
}

/* 
 - 作用：将时间实时显示在lcd显示屏中，一秒刷新一次
*/
void RTC_Show_Time()
{
  static u8 t = 62;

  if (t != calendar.sec)
  {
    t = calendar.sec;
    LCD_ShowNum(47, 60, calendar.w_year, 4, 16);
    LCD_ShowNum(85, 60, calendar.w_month, 2, 16);
    LCD_ShowNum(109, 60, calendar.w_date, 2, 16);
    switch (calendar.week)
    {
    case 0:
      LCD_ShowString(90, 80, 200, 16, 16, "Sunday   ");
      break;
    case 1:
      LCD_ShowString(90, 80, 200, 16, 16, "Monday   ");
      break;
    case 2:
      LCD_ShowString(90, 80, 200, 16, 16, "Tuesday  ");
      break;
    case 3:
      LCD_ShowString(90, 80, 200, 16, 16, "Wednesday");
      break;
    case 4:
      LCD_ShowString(90, 80, 200, 16, 16, "Thursday ");
      break;
    case 5:
      LCD_ShowString(90, 80, 200, 16, 16, "Friday   ");
      break;
    case 6:
      LCD_ShowString(90, 80, 200, 16, 16, "Saturday ");
      break;
    }
    LCD_ShowNum(137, 60, calendar.hour, 2, 16);
    LCD_ShowNum(162, 60, calendar.min, 2, 16);
    LCD_ShowNum(187, 60, calendar.sec, 2, 16);
  }
}
