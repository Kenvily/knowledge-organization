/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-4-20
 * @brief       LVGL移植课堂代码1 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 阿波罗 F429开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/SDRAM/sdram.h"
#include "./BSP/TOUCH/touch.h"
#include "./BSP/TIMER/btim.h"
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "lv_demo_stress.h"
#include "lv_demo_music.h"

int main(void)
{
    HAL_Init();                              /* 初始化HAL库 */
    sys_stm32_clock_init(360, 25, 2, 8);     /* 设置时钟,180Mhz */
    delay_init(180);                         /* 延时初始化 */
    usart_init(115200);                      /* 初始化USART */
    led_init();                              /* 初始化LED */
    key_init();                              /* 初始化按键 */
    sdram_init();                            /* 初始化SDRAM */

    btim_timx_int_init(10-1, 9000-1);
    lv_init(); 
    lv_port_disp_init(); 
    lv_port_indev_init();

//    lv_obj_t* switch_obj = lv_switch_create(lv_scr_act());
//    lv_obj_set_size(switch_obj, 120, 60);
//    lv_obj_align(switch_obj, LV_ALIGN_CENTER, 0, 0);

//    lv_demo_stress();
     lv_demo_music();
    
    while(1)
    {
        delay_ms(5);
        lv_timer_handler();
    }
}

