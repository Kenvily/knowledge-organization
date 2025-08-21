#ifndef _SERVO_H
#define _SERVO_H
#include "sys.h"
#include "delay.h"
#include "pwm.h"
#include "FreeRTOS.h"
# include "task.h"

#define SERVO_SET(compare)        \
  {                               \
    PWM_SetTIM2Compare1(compare); \
  }

#define SERVO_FORWARD() \
  {                     \
    SERVO_SET(5);       \
    vTaskDelay(250);      \
    SERVO_OFF();          \
  }
#define SERVO_BACKWORD() \
  {                      \
    SERVO_SET(25);       \
    vTaskDelay(250);       \
    SERVO_OFF();           \
  }
#define SERVO_OFF() \
  {                 \
    SERVO_SET(15);  \
  }

#define SERVO_TOGGLE(x) \
  {                     \
    SERVO_SET(x);       \
    vTaskDelay(300);      \
    SERVO_SET(15);      \
  }


#endif
