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
    {0},         // x[OTT_MAX_TOUCH]������Ԫ�س�ʼ��Ϊ0��
    {0},         // y[OTT_MAX_TOUCH]
    0,           // sta
    0.0f,        // xfac
    0.0f,        // yfac
    0,           // xoff
    0,           // yoff
    0            // touchtype
};

// Ĭ��Ϊtouchtype=0������.
u8 CMD_RDX = 0XD0;
u8 CMD_RDY = 0X90;

// SPIд����
// ������ICд��1byte����
// num:Ҫд�������
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
        TCLK_H(); // ��������Ч
    }
}

// SPI������
// �Ӵ�����IC��ȡadcֵ
// CMD:ָ��
// ����ֵ:����������
u16 TP_Read_AD(u8 CMD)
{
    u8 count = 0;
    u16 Num = 0;
    TCLK_L();           // ������ʱ��
    TDIN_L();           // ����������
    TCS_L();            // ѡ�д�����IC
    TP_Write_Byte(CMD); // ����������
    delay_us(6);        // ADS7846��ת��ʱ���Ϊ6us
    TCLK_L();
    delay_us(1);
    TCLK_H(); // ��1��ʱ�ӣ����BUSY
    delay_us(1);
    TCLK_L();
    for (count = 0; count < 16; count++) // ����16λ����,ֻ�и�12λ��Ч
    {
        Num <<= 1;
        TCLK_L(); // �½�����Ч
        delay_us(1);
        TCLK_H();
        if (DOUT_R())
            Num++;
    }
    Num >>= 4; // ֻ�и�12λ��Ч.
    TCS_H();   // �ͷ�Ƭѡ
    return (Num);
}
// ��ȡһ������ֵ(x����y)
// ������ȡREAD_TIMES������,����Щ������������,
// Ȼ��ȥ����ͺ����LOST_VAL����,ȡƽ��ֵ
// xy:ָ�CMD_RDX/CMD_RDY��
// ����ֵ:����������
#define READ_TIMES 5 // ��ȡ����
#define LOST_VAL 1   // ����ֵ
u16 TP_Read_XOY(u8 xy)
{
    u16 i, j;
    u16 buf[READ_TIMES];
    u16 sum = 0;
    u16 temp;
    for (i = 0; i < READ_TIMES; i++)
        buf[i] = TP_Read_AD(xy);
    for (i = 0; i < READ_TIMES - 1; i++) // ����
    {
        for (j = i + 1; j < READ_TIMES; j++)
        {
            if (buf[i] > buf[j]) // ��������
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
// ��ȡx,y����
// ��Сֵ��������100.
// x,y:��ȡ��������ֵ
// ����ֵ:0,ʧ��;1,�ɹ���
u8 TP_Read_XY(u16 *x, u16 *y)
{
    u16 xtemp, ytemp;
    xtemp = TP_Read_XOY(CMD_RDX);
    ytemp = TP_Read_XOY(CMD_RDY);
    // if(xtemp<100||ytemp<100)return 0;//����ʧ��
    *x = xtemp;
    *y = ytemp;
    return 1; // �����ɹ�
}
// ����2�ζ�ȡ������IC,�������ε�ƫ��ܳ���
// ERR_RANGE,��������,����Ϊ������ȷ,�����������.
// �ú����ܴ�����׼ȷ��
// x,y:��ȡ��������ֵ
// ����ֵ:0,ʧ��;1,�ɹ���
#define ERR_RANGE 50 // ��Χ
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
    if (((x2 <= x1 && x1 < x2 + ERR_RANGE) || (x1 <= x2 && x2 < x1 + ERR_RANGE)) // ǰ�����β�����+-50��
        && ((y2 <= y1 && y1 < y2 + ERR_RANGE) || (y1 <= y2 && y2 < y1 + ERR_RANGE)))
    {
        *x = (x1 + x2) / 2;
        *y = (y1 + y2) / 2;
        return 1;
    }
    else
        return 0;
}

// ��������ɨ��
// tp:0,��Ļ����;1,��������(У׼�����ⳡ����)
// ����ֵ:��ǰ����״̬.
// 0,�����޴���;1,�����д���
u8 TP_Scan(u8 tp)
{
    if (PEN() == 0) // �а�������
    {
        taskENTER_CRITICAL();
        if (tp)
        {
            TP_Read_XY2(&tp_dev.x[0], &tp_dev.y[0]);      // ��ȡ��������
        }
        else if (TP_Read_XY2(&tp_dev.x[0], &tp_dev.y[0])) // ��ȡ��Ļ����
        {
            tp_dev.x[0] = tp_dev.xfac * tp_dev.x[0] + tp_dev.xoff; // �����ת��Ϊ��Ļ����
            tp_dev.y[0] = tp_dev.yfac * tp_dev.y[0] + tp_dev.yoff;
        }
        if ((tp_dev.sta & TP_PRES_DOWN) == 0) // ֮ǰû�б�����
        {
            tp_dev.sta = TP_PRES_DOWN | TP_CATH_PRES; // ��������
            tp_dev.x[4] = tp_dev.x[0];                // ��¼��һ�ΰ���ʱ������
            tp_dev.y[4] = tp_dev.y[0];

        }
        taskEXIT_CRITICAL();
    }
    else
    {
        if (tp_dev.sta & TP_PRES_DOWN) // ֮ǰ�Ǳ����µ�
        {
            tp_dev.sta &= ~(1 << 7); // ��ǰ����ɿ�
        }
        else // ֮ǰ��û�б�����
        {
            tp_dev.x[4] = 0;
            tp_dev.y[4] = 0;
            tp_dev.x[0] = 0xffff;
            tp_dev.y[0] = 0xffff;
        }
    }
    return tp_dev.sta & TP_PRES_DOWN; // ���ص�ǰ�Ĵ���״̬
}

// ��������ʼ��
// ����ֵ:0,û�н���У׼
//        1,���й�У׼
u8 TP_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOF, ENABLE); // ʹ��GPIOB,C,Fʱ��

    // GPIOB14��ʼ������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;         // PB14 ����Ϊ��������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;       // ����ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     // �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       // ����
    GPIO_Init(GPIOB, &GPIO_InitStructure);             // ��ʼ��

    // GPIOC5��ʼ������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;          // PC5 ����Ϊ��������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;       // ����ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     // �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       // ����
    GPIO_Init(GPIOC, &GPIO_InitStructure);             // ��ʼ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15; // PB0����Ϊ�������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                          // ���ģʽ
    GPIO_Init(GPIOB, &GPIO_InitStructure);                                 // ��ʼ��

    TP_Read_XY(&tp_dev.x[0], &tp_dev.y[0]); // ��һ�ζ�ȡ��ʼ��
    //    AT24CXX_Init();        //��ʼ��24CXX
    //    if(TP_Get_Adjdata()) {
    //        return 0;//�Ѿ�У׼
    //    } else {
    //        LCD_Clear(WHITE);//����
    //        TP_Adjust();      //��ĻУ׼
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
