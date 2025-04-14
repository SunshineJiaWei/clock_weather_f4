#include "touch.h"
#include "ILI9341.h"
#include "delay.h"
#include "stdlib.h"
#include "math.h"

#define LOG_TAG "touch"
#include <elog.h>


#include "FreeRTOS.h"
#include "task.h"

// PCin(5)      //T_PEN
// PBin(14)       //T_MISO
// PBout(15)      //T_MOSI
// PBout(13)      //T_SCK
// PBout(12)      //T_CS

// #define      PEN_H()       GPIO_WriteBit(GPIOC, GPIO_Pin_5, 1)
// #define      PEN_L()       GPIO_WriteBit(GPIOC, GPIO_Pin_5, 1)
#define PEN() GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5)
// #define      DOUT_H()      GPIO_WriteBit(GPIOB, GPIO_Pin_14, 1)
// #define      DOUT_L()      GPIO_WriteBit(GPIOB, GPIO_Pin_14, 0)
#define DOUT_R() GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)

#define TDIN_H() GPIO_WriteBit(GPIOB, GPIO_Pin_15, 1)
#define TDIN_L() GPIO_WriteBit(GPIOB, GPIO_Pin_15, 0)

#define TCLK_H() GPIO_WriteBit(GPIOB, GPIO_Pin_13, 1)
#define TCLK_L() GPIO_WriteBit(GPIOB, GPIO_Pin_13, 0)

#define TCS_H() GPIO_WriteBit(GPIOB, GPIO_Pin_12, 1)
#define TCS_L() GPIO_WriteBit(GPIOB, GPIO_Pin_12, 0)

// _m_tp_dev tp_dev =
//     {
//         TP_Init,
//         TP_Scan,
//         //    TP_Adjust,
//         NULL,
//         0,
//         0,
//         0,
//         0,
//         0,
//         0,
//         0,
//         0,
// };

_m_tp_dev tp_dev = {
    TP_Init,     // init
    TP_Scan,     // scan
    NULL,        // adjust
    {0},         // x[OTT_MAX_TOUCH]（所有元素初始化为0）
    {0},         // y[OTT_MAX_TOUCH]
    0,           // sta
    0.0f,        // xfac
    0.0f,        // yfac
    0,           // xoff
    0,           // yoff
    0            // touchtype
};

// 默认为touchtype=0的数据.
u8 CMD_RDX = 0XD0;
u8 CMD_RDY = 0X90;

// SPI写数据
// 向触摸屏IC写入1byte数据
// num:要写入的数据
void TP_Write_Byte(u8 num)
{
    u8 count = 0;
    for (count = 0; count < 8; count++)
    {
        if (num & 0x80)
            TDIN_H();
        else
            TDIN_L();
        num <<= 1;
        TCLK_L();
        delay_us(1);
        TCLK_H(); // 上升沿有效
    }
}

// SPI读数据
// 从触摸屏IC读取adc值
// CMD:指令
// 返回值:读到的数据
u16 TP_Read_AD(u8 CMD)
{
    u8 count = 0;
    u16 Num = 0;
    TCLK_L();           // 先拉低时钟
    TDIN_L();           // 拉低数据线
    TCS_L();            // 选中触摸屏IC
    TP_Write_Byte(CMD); // 发送命令字
    delay_us(6);        // ADS7846的转换时间最长为6us
    TCLK_L();
    delay_us(1);
    TCLK_H(); // 给1个时钟，清除BUSY
    delay_us(1);
    TCLK_L();
    for (count = 0; count < 16; count++) // 读出16位数据,只有高12位有效
    {
        Num <<= 1;
        TCLK_L(); // 下降沿有效
        delay_us(1);
        TCLK_H();
        if (DOUT_R())
            Num++;
    }
    Num >>= 4; // 只有高12位有效.
    TCS_H();   // 释放片选
    return (Num);
}
// 读取一个坐标值(x或者y)
// 连续读取READ_TIMES次数据,对这些数据升序排列,
// 然后去掉最低和最高LOST_VAL个数,取平均值
// xy:指令（CMD_RDX/CMD_RDY）
// 返回值:读到的数据
#define READ_TIMES 5 // 读取次数
#define LOST_VAL 1   // 丢弃值
u16 TP_Read_XOY(u8 xy)
{
    u16 i, j;
    u16 buf[READ_TIMES];
    u16 sum = 0;
    u16 temp;
    for (i = 0; i < READ_TIMES; i++)
        buf[i] = TP_Read_AD(xy);
    for (i = 0; i < READ_TIMES - 1; i++) // 排序
    {
        for (j = i + 1; j < READ_TIMES; j++)
        {
            if (buf[i] > buf[j]) // 升序排列
            {
                temp = buf[i];
                buf[i] = buf[j];
                buf[j] = temp;
            }
        }
    }
    sum = 0;
    for (i = LOST_VAL; i < READ_TIMES - LOST_VAL; i++)
        sum += buf[i];
    temp = sum / (READ_TIMES - 2 * LOST_VAL);
    return temp;
}
// 读取x,y坐标
// 最小值不能少于100.
// x,y:读取到的坐标值
// 返回值:0,失败;1,成功。
u8 TP_Read_XY(u16 *x, u16 *y)
{
    u16 xtemp, ytemp;
    xtemp = TP_Read_XOY(CMD_RDX);
    ytemp = TP_Read_XOY(CMD_RDY);
    // if(xtemp<100||ytemp<100)return 0;//读数失败
    *x = xtemp;
    *y = ytemp;
    return 1; // 读数成功
}
// 连续2次读取触摸屏IC,且这两次的偏差不能超过
// ERR_RANGE,满足条件,则认为读数正确,否则读数错误.
// 该函数能大大提高准确度
// x,y:读取到的坐标值
// 返回值:0,失败;1,成功。
#define ERR_RANGE 50 // 误差范围
u8 TP_Read_XY2(u16 *x, u16 *y)
{
    u16 x1, y1;
    u16 x2, y2;
    u8 flag;
    flag = TP_Read_XY(&x1, &y1);
    if (flag == 0)
        return (0);
    flag = TP_Read_XY(&x2, &y2);
    if (flag == 0)
        return (0);
    if (((x2 <= x1 && x1 < x2 + ERR_RANGE) || (x1 <= x2 && x2 < x1 + ERR_RANGE)) // 前后两次采样在+-50内
        && ((y2 <= y1 && y1 < y2 + ERR_RANGE) || (y1 <= y2 && y2 < y1 + ERR_RANGE)))
    {
        *x = (x1 + x2) / 2;
        *y = (y1 + y2) / 2;
        return 1;
    }
    else
        return 0;
}

// 触摸按键扫描
// tp:0,屏幕坐标;1,物理坐标(校准等特殊场合用)
// 返回值:当前触屏状态.
// 0,触屏无触摸;1,触屏有触摸
u8 TP_Scan(u8 tp)
{
    if (PEN() == 0) // 有按键按下
    {
        taskENTER_CRITICAL();
        if (tp)
        {
            TP_Read_XY2(&tp_dev.x[0], &tp_dev.y[0]);      // 读取物理坐标
        }
        else if (TP_Read_XY2(&tp_dev.x[0], &tp_dev.y[0])) // 读取屏幕坐标
        {
            tp_dev.x[0] = tp_dev.xfac * tp_dev.x[0] + tp_dev.xoff; // 将结果转换为屏幕坐标
            tp_dev.y[0] = tp_dev.yfac * tp_dev.y[0] + tp_dev.yoff;
        }
        if ((tp_dev.sta & TP_PRES_DOWN) == 0) // 之前没有被按下
        {
            tp_dev.sta = TP_PRES_DOWN | TP_CATH_PRES; // 按键按下
            tp_dev.x[4] = tp_dev.x[0];                // 记录第一次按下时的坐标
            tp_dev.y[4] = tp_dev.y[0];

        }
        taskEXIT_CRITICAL();
    }
    else
    {
        if (tp_dev.sta & TP_PRES_DOWN) // 之前是被按下的
        {
            tp_dev.sta &= ~(1 << 7); // 标记按键松开
        }
        else // 之前就没有被按下
        {
            tp_dev.x[4] = 0;
            tp_dev.y[4] = 0;
            tp_dev.x[0] = 0xffff;
            tp_dev.y[0] = 0xffff;
        }
    }
    return tp_dev.sta & TP_PRES_DOWN; // 返回当前的触屏状态
}

// 触摸屏初始化
// 返回值:0,没有进行校准
//        1,进行过校准
u8 TP_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOF, ENABLE); // 使能GPIOB,C,F时钟

    // GPIOB14初始化设置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;         // PB14 设置为上拉输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;       // 输入模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       // 上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);             // 初始化

    // GPIOC5初始化设置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;          // PC5 设置为上拉输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;       // 输入模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       // 上拉
    GPIO_Init(GPIOC, &GPIO_InitStructure);             // 初始化

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15; // PB0设置为推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                          // 输出模式
    GPIO_Init(GPIOB, &GPIO_InitStructure);                                 // 初始化

    TP_Read_XY(&tp_dev.x[0], &tp_dev.y[0]); // 第一次读取初始化
    //    AT24CXX_Init();        //初始化24CXX
    //    if(TP_Get_Adjdata()) {
    //        return 0;//已经校准
    //    } else {
    //        LCD_Clear(WHITE);//清屏
    //        TP_Adjust();      //屏幕校准
    //        TP_Save_Adjdata();
    tp_dev.xfac = 0.064371f;
    tp_dev.yfac = 0.088945f;
    tp_dev.xoff = -11;
    tp_dev.yoff = -13;
    tp_dev.touchtype = 0;
    //    }

    //    TP_Get_Adjdata();
    log_i("touch init ok!\r\n");
    return 1;
}
