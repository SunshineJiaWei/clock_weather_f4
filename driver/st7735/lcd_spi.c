#include "lcd_spi.h"


#define LCD_SCK_PORT        GPIOB
#define LCD_SCK_PIN         GPIO_Pin_3
#define LCD_MOSI_PORT       GPIOB
#define LCD_MOSI_PIN        GPIO_Pin_5


static lcd_spi_send_finish_callback_t lcd_spi_send_finish_callback;


static void lcd_spi_io_init(void)
{
    GPIO_PinAFConfig(LCD_SCK_PORT, GPIO_PinSource3, GPIO_AF_SPI1);
    GPIO_PinAFConfig(LCD_MOSI_PORT, GPIO_PinSource5, GPIO_AF_SPI1);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = LCD_SCK_PIN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
    GPIO_Init(LCD_SCK_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = LCD_MOSI_PIN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
    GPIO_Init(LCD_MOSI_PORT, &GPIO_InitStructure);
}

static void lcd_spi_lowlevel_init(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CRCPolynomial = 0;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);

    SPI_Cmd(SPI1, ENABLE);
}

static void lcd_spi_nvic_init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);
}

void lcd_spi_init(void)
{
    lcd_spi_io_init();
    lcd_spi_lowlevel_init();
    lcd_spi_nvic_init();
}

void lcd_spi_write(uint8_t *data, uint16_t len)
{
    for(uint16_t i = 0; i < len; i++)
    {
        while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
        SPI_I2S_SendData(SPI1, data[i]);
    }
    // 确保所有数据已从移位寄存器完全发送完毕，而不仅仅是写入发送缓冲区
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
}

void lcd_spi_dma_write(uint8_t *data, uint16_t len)
{
    DMA_InitTypeDef DMA_InitStructure;
    memset(&DMA_InitStructure, 0, sizeof(DMA_InitStructure));
    DMA_InitStructure.DMA_BufferSize = len;
    DMA_InitStructure.DMA_Channel = DMA_Channel_3;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)data;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(SPI1->DR);
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_Init(DMA2_Stream3, &DMA_InitStructure);

    DMA_ITConfig(DMA2_Stream3, DMA_IT_TC, ENABLE);

    DMA_Cmd(DMA2_Stream3, ENABLE);
}

void lcd_spi_send_finish_register(lcd_spi_send_finish_callback_t callback)
{
    lcd_spi_send_finish_callback = callback;
}

void DMA2_Stream3_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA2_Stream3, DMA_IT_TCIF3))
    {
        while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);

        if(lcd_spi_send_finish_callback) lcd_spi_send_finish_callback();

        DMA_ClearITPendingBit(DMA2_Stream3, DMA_IT_TCIF3);
    }
}
