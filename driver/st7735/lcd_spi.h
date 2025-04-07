#ifndef __LCD_SPI_H
#define ___LCD_SPI_H


#include "stm32f4xx.h"
#include <stdbool.h>
#include <stdint.h>


typedef void (*lcd_spi_send_finish_callback_t)(void);

void lcd_spi_init(void);
void lcd_spi_write(uint8_t *data, uint16_t len);
void lcd_spi_dma_write(uint8_t *data, uint16_t len);
void lcd_spi_send_finish_register(lcd_spi_send_finish_callback_t callback);


#endif /* __LCD_SPI_H*/
