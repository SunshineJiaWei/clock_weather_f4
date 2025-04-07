#include "main.h"


void board_low_level_init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
}

const led_desc_t led0 =
{
	.clk_source = RCC_AHB1Periph_GPIOE,
	.port = GPIOE,
	.pin = GPIO_Pin_5,
	.on_lvl = Bit_RESET,
	.off_lvl = Bit_SET,
    .name = "led0"
};

const led_desc_t led1 =
{
	.clk_source = RCC_AHB1Periph_GPIOE,
	.port = GPIOE,
	.pin = GPIO_Pin_6,
	.on_lvl = Bit_RESET,
	.off_lvl = Bit_SET,
    .name = "led1"
};

const led_desc_t led2 =
{
	.clk_source = RCC_AHB1Periph_GPIOC,
	.port = GPIOC,
	.pin = GPIO_Pin_13,
	.on_lvl = Bit_RESET,
	.off_lvl = Bit_SET,
    .name = "led2"
};
