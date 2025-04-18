#ifndef __ST7735_H
#define __ST7735_H

#include "stm32f4xx.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "lcd_spi.h"
#include "delay.h"
#include "stfonts.h"


// 1.44" display, default orientation
#define ST7735_IS_128X128 1
#define ST7735_WIDTH 128
#define ST7735_HEIGHT 128
#define ST7735_XSTARTOFFSET 2
#define ST7735_YSTARTOFFSET 3
#define ST7735_ROTATION (ST7735_MADCTL_MX | ST7735_MADCTL_MY | ST7735_MADCTL_BGR)

// Color definitions
#define ST7735_BLACK 0x0000
#define ST7735_BLUE 0x001F
#define ST7735_RED 0xF800
#define ST7735_GREEN 0x07E0
#define ST7735_CYAN 0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW 0xFFE0
#define ST7735_WHITE 0xFFFF

void st7735_init(void);
void st7735_reset(void);

void st7735_select(void);
void st7735_unselect(void);

void st7735_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void st7735_fill_screen(uint16_t color);
void st7735_write_char(uint16_t x, uint16_t y, char ch, st_fonts_t *font, uint16_t color, uint16_t bgcolor);
void st7735_write_string(uint16_t x, uint16_t y, const char *str, st_fonts_t *font, uint16_t color, uint16_t bgcolor);
void st7735_draw_image(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *data);

#endif
