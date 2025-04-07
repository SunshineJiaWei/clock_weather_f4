#ifndef __DEV_LED_H
#define __DEV_LED_H


#include "stm32f4xx.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>


typedef struct
{
	uint32_t clk_source;
	GPIO_TypeDef *port;
	uint16_t pin;
	BitAction on_lvl;
	BitAction off_lvl;
    char name[10];
} led_desc_t;

void led_init(const led_desc_t *led);
void led_deinit(const led_desc_t *led);
void led_on(const led_desc_t *led);
void led_off(const led_desc_t *led);
void led_toggle(const led_desc_t *led);


#endif /* __DEV_LED_H */
