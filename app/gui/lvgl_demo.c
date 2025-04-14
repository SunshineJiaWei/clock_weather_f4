#include "lvgl_demo.h"
#include "led.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_demo_stress.h"

#define LOG_TAG    "lvgl_demo"
#include <elog.h>

extern const led_desc_t led0;

/******************************************************************************************************/
/*FreeRTOS����*/

/* START_TASK ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define START_TASK_PRIO     1           /* �������ȼ� */
#define START_STK_SIZE      128         /* �����ջ��С */
TaskHandle_t StartTask_Handler;         /* ������ */
void start_task(void *pvParameters);    /* ������ */

/* LV_DEMO_TASK ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define LV_DEMO_TASK_PRIO   4           /* �������ȼ� */
#define LV_DEMO_STK_SIZE    1024        /* �����ջ��С */
TaskHandle_t LV_DEMOTask_Handler;       /* ������ */
void lv_demo_task(void *pvParameters);  /* ������ */

/* LED_TASK ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define LED_TASK_PRIO       3           /* �������ȼ� */
#define LED_STK_SIZE        128         /* �����ջ��С */
TaskHandle_t LEDTask_Handler;           /* ������ */
void led_task(void *pvParameters);      /* ������ */
/******************************************************************************************************/

/**
 * @brief       lvgl_demo��ں���
 * @param       ��
 * @retval      ��
 */
void lvgl_demo(void)
{
    lv_init();                                          /* lvglϵͳ��ʼ�� */
    lv_port_disp_init();                                /* lvgl��ʾ�ӿڳ�ʼ��,����lv_init()�ĺ��� */
    lv_port_indev_init();                               /* lvgl����ӿڳ�ʼ��,����lv_init()�ĺ��� */

    xTaskCreate((TaskFunction_t )start_task,            /* ������ */
                (const char*    )"start_task",          /* �������� */
                (uint16_t       )START_STK_SIZE,        /* �����ջ��С */
                (void*          )NULL,                  /* ���ݸ��������Ĳ��� */
                (UBaseType_t    )START_TASK_PRIO,       /* �������ȼ� */
                (TaskHandle_t*  )&StartTask_Handler);   /* ������ */

    vTaskStartScheduler();                              /* ����������� */
}

/**
 * @brief       start_task
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void start_task(void *pvParameters)
{
    pvParameters = pvParameters;

    taskENTER_CRITICAL();           /* �����ٽ��� */

    /* ����LVGL���� */
    xTaskCreate((TaskFunction_t )lv_demo_task,
                (const char*    )"lv_demo_task",
                (uint16_t       )LV_DEMO_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )LV_DEMO_TASK_PRIO,
                (TaskHandle_t*  )&LV_DEMOTask_Handler);

    /* LED�������� */
    xTaskCreate((TaskFunction_t )led_task,
                (const char*    )"led_task",
                (uint16_t       )LED_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )LED_TASK_PRIO,
                (TaskHandle_t*  )&LEDTask_Handler);

    taskEXIT_CRITICAL();            /* �˳��ٽ��� */
    vTaskDelete(StartTask_Handler); /* ɾ����ʼ���� */
}

/**
 * @brief       LVGL��������
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void lv_demo_task(void *pvParameters)
{
    pvParameters = pvParameters;

    // lv_demo_stress();       /* ���Ե�demo */

    lv_obj_t* switch_obj = lv_switch_create(lv_scr_act());
	lv_obj_set_size(switch_obj, 120, 60);
	lv_obj_align(switch_obj, LV_ALIGN_CENTER, 0, 0);

    // // ��ť
    // lv_obj_t *myBtn = lv_btn_create(lv_scr_act());                               // ������ť; �����󣺵�ǰ���Ļ
    // lv_obj_set_pos(myBtn, 10, 10);                                               // ��������
    // lv_obj_set_size(myBtn, 120, 50);                                             // ���ô�С

    // // ��ť�ϵ��ı�
    // lv_obj_t *label_btn = lv_label_create(myBtn);                                // �����ı���ǩ�������������btn��ť
    // lv_obj_align(label_btn, LV_ALIGN_CENTER, 0, 0);                              // �����ڣ�������
    // lv_label_set_text(label_btn, "Test");                                        // ���ñ�ǩ���ı�

    // // �����ı�ǩ
    // lv_obj_t *myLabel = lv_label_create(lv_scr_act());                           // �����ı���ǩ; �����󣺵�ǰ���Ļ
    // lv_label_set_text(myLabel, "Hello world!");                                  // ���ñ�ǩ���ı�
    // lv_obj_align(myLabel, LV_ALIGN_CENTER, 0, 0);                                // �����ڣ�������
    // lv_obj_align_to(myBtn, myLabel, LV_ALIGN_OUT_TOP_MID, 0, -20);               // �����ڣ�ĳ����
    log_e("create gui ok\r\n");
    while(1)
    {
        lv_timer_handler(); /* LVGL��ʱ�� */
        vTaskDelay(5);
    }
}

/**
 * @brief       led_task
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void led_task(void *pvParameters)
{
    pvParameters = pvParameters;

    while(1)
    {
        led_toggle(&led0);
        vTaskDelay(1000);
    }
}
