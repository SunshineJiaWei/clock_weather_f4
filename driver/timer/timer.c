#include "timer.h"
#include "lvgl.h"


/******************************************************************************
 * @brief     ͨ�ö�ʱ��4�жϳ�ʼ��
 *            ��ʱ�����ʱ����㷽��:Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *            Ft = ��ʱ������Ƶ��, ��λ: Mhz
 *
 * @param[in]  arr   :  �Զ���װֵ
 * @param[in]  psc   :  ʱ��Ԥ��Ƶ��
 *
 * @return     ��
 *
 ******************************************************************************/
void TIM4_Init(uint16_t arr, uint16_t psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    TIM_TimeBaseInitStructure.TIM_Prescaler      = psc - 1;                   //��ʱ����Ƶ
    TIM_TimeBaseInitStructure.TIM_CounterMode    = TIM_CounterMode_Up;    //���ϼ���ģʽ
    TIM_TimeBaseInitStructure.TIM_Period         = arr - 1;                   //�Զ���װ��ֵ
    TIM_TimeBaseInitStructure.TIM_ClockDivision  = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);                   //��ʼ����ʱ��4

    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);                             //����ʱ��4�����ж�
    TIM_Cmd(TIM4, ENABLE);                                                //ʹ�ܶ�ʱ��4

    NVIC_InitStructure.NVIC_IRQChannel                    = TIM4_IRQn;    //��ʱ��4�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0x02;         //��ռ���ȼ�1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0x03;         //�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd                 = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

//��ʱ��4�жϷ�����
void TIM4_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)  //����ж�
    {

		// lv_tick_inc(1);
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  //����жϱ�־λ
    }

}

