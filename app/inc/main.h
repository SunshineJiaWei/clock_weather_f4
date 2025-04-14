#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "delay.h"
#include "led.h"
#include "console_usart.h"
#include "st7735.h"
#include "timer.h"

#include "ILI9341.h"
#include "touch.h"

#include "FreeRTOS.H"
#include "task.h"

#include "lvgl_demo.h"

#include <elog.h>


void board_low_level_init(void);
void elog_low_level_init(void);

extern const led_desc_t led0;
extern const led_desc_t led1;
extern const led_desc_t led2;


#endif /* __MAIN_H */
