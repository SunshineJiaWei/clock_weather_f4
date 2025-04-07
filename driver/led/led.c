#include "stm32f4xx.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "led.h"


void led_init(const led_desc_t *led)
{
	RCC_AHB1PeriphClockCmd(led->clk_source, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	memset(&GPIO_InitStructure, 0, sizeof(GPIO_InitTypeDef));

	GPIO_InitStructure.GPIO_Pin = led->pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Medium_Speed;
	GPIO_Init(led->port, &GPIO_InitStructure);
}

void led_deinit(const led_desc_t *led)
{
	GPIO_WriteBit(led->port, led->pin, led->off_lvl);
}

void led_on(const led_desc_t *led)
{
	GPIO_WriteBit(led->port, led->pin, led->on_lvl);
}

void led_off(const led_desc_t *led)
{
	GPIO_WriteBit(led->port, led->pin, led->off_lvl);
}

void led_toggle(const led_desc_t *led)
{
    if(GPIO_ReadInputDataBit(led->port, led->pin) == RESET)
    {
        led_off(led);
    }
    else
    {
        led_on(led);
    }
}
