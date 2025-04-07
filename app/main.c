#include "main.h"

TaskFunction_t task1(void)
{
    uint32_t num = 16;
    while(1)
    {
        led_toggle(&led0);
        printf("task1 num:%d\r\n",++num);
        vTaskDelay(pdMS_TO_TICKS(500));
    }

}

TaskFunction_t task2(void)
{
    uint32_t num = 0;
    while(1)
    {
        led_toggle(&led1);
        printf("task2 num:%d\r\n",++num);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}

int main(void)
{
    board_low_level_init();

    usart_init();

    delay_init();

    st7735_init();
    st7735_fill_screen(ST7735_BLACK);

    led_init(&led0);
    led_init(&led1);
    led_init(&led2);

    st7735_write_char(0, 0, 'A', &st_font_ascii_8x16, ST7735_RED, ST7735_BLACK);

    xTaskCreate((TaskFunction_t)task1, "task1", 128, NULL, 3, NULL);
    xTaskCreate((TaskFunction_t)task2, "task2", 128, NULL, 5, NULL);

    vTaskStartScheduler();

	while(1)
	{
		// led_toggle(&led2);
        printf("Hello World!\r\n"); // 发送字符串到串口1
        printf("你好\r\n"); // 发送字符串到串口1

        delay_ms(1000); // 延时1秒
	}
}

