#include "stm32f4xx.h"
#include "delay.h"

uint32_t fac_us, fac_ms = 0; // 微妙/毫秒因子：描述每单位时间内的时钟周期数

void delay_init(void)
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); // 设置时钟源HCLK / 8
    fac_us = SystemCoreClock / 8000000;                   // 每微秒的时钟周期数
    fac_ms = (uint32_t)fac_us * 1000;                     // 每毫秒的时钟周期数
}

void delay_us(uint32_t nus)
{
    uint32_t temp = 0;                        // 临时变量，用于存储当前计数值
    SysTick->LOAD = nus * fac_us - 1;         // 设置重载值
    SysTick->VAL = 0x00;                      // 清空计数器
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // 启动SysTick计数器

    do
    {
        temp = SysTick->CTRL; // 读取SysTick控制寄存器
    } while ((temp & SysTick_CTRL_COUNTFLAG_Msk) == 0); // 等待计数完成

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; // 停止计数器
    SysTick->VAL = 0x00;                       // 清空计数器
}

void delay_ms(uint32_t nms)
{
    while (nms--) // 循环延时
    {
        delay_us(1000); // 每次延时1毫秒
    }
}
