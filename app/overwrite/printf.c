#include "stm32f4xx.h"
#include <stdio.h>

int fputc(int Data, FILE *BUF)
{
    USART_SendData(USART1, Data); // 串口1发送数据Data
    //	如果==0，就一直在这循环等待，直到等于1才跳出循环，这个标志位是1的时候，代表发送完成了
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        ;

    return Data;
}

