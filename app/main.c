#include "main.h"


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
    st7735_write_string(0, 16, "Hello World!", &st_font_ascii_8x16, ST7735_GREEN, ST7735_RED);

	while(1)
	{
        led_toggle(&led0);
		led_toggle(&led1);
		led_toggle(&led2);
        printf("Hello World!\r\n"); // 发送字符串到串口1
        printf("你好\r\n"); // 发送字符串到串口1

        delay_ms(1000); // 延时1秒
	}
}

