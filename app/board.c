#include "main.h"

void board_low_level_init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
}

void elog_low_level_init(void)
{
    /* close printf buffer */
    setbuf(stdout, NULL);
    /* initialize EasyLogger */
    elog_init();
    /* set EasyLogger log format */
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
    /* start EasyLogger */
    elog_start();
}

const led_desc_t led0 =
    {
        .clk_source = RCC_AHB1Periph_GPIOE,
        .port = GPIOE,
        .pin = GPIO_Pin_5,
        .on_lvl = Bit_RESET,
        .off_lvl = Bit_SET,
        .name = "led0"};

const led_desc_t led1 =
    {
        .clk_source = RCC_AHB1Periph_GPIOE,
        .port = GPIOE,
        .pin = GPIO_Pin_6,
        .on_lvl = Bit_RESET,
        .off_lvl = Bit_SET,
        .name = "led1"};

const led_desc_t led2 =
    {
        .clk_source = RCC_AHB1Periph_GPIOC,
        .port = GPIOC,
        .pin = GPIO_Pin_13,
        .on_lvl = Bit_RESET,
        .off_lvl = Bit_SET,
        .name = "led2"};
