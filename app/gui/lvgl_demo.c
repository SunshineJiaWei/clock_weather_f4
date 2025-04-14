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
/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO     1           /* 任务优先级 */
#define START_STK_SIZE      128         /* 任务堆栈大小 */
TaskHandle_t StartTask_Handler;         /* 任务句柄 */
void start_task(void *pvParameters);    /* 任务函数 */

/* LV_DEMO_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define LV_DEMO_TASK_PRIO   4           /* 任务优先级 */
#define LV_DEMO_STK_SIZE    1024        /* 任务堆栈大小 */
TaskHandle_t LV_DEMOTask_Handler;       /* 任务句柄 */
void lv_demo_task(void *pvParameters);  /* 任务函数 */

/* LED_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define LED_TASK_PRIO       3           /* 任务优先级 */
#define LED_STK_SIZE        128         /* 任务堆栈大小 */
TaskHandle_t LEDTask_Handler;           /* 任务句柄 */
void led_task(void *pvParameters);      /* 任务函数 */
/******************************************************************************************************/

/**
 * @brief       lvgl_demo入口函数
 * @param       无
 * @retval      无
 */
void lvgl_demo(void)
{
    lv_init();                                          /* lvgl系统初始化 */
    lv_port_disp_init();                                /* lvgl显示接口初始化,放在lv_init()的后面 */
    lv_port_indev_init();                               /* lvgl输入接口初始化,放在lv_init()的后面 */

    xTaskCreate((TaskFunction_t )start_task,            /* 任务函数 */
                (const char*    )"start_task",          /* 任务名称 */
                (uint16_t       )START_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传递给任务函数的参数 */
                (UBaseType_t    )START_TASK_PRIO,       /* 任务优先级 */
                (TaskHandle_t*  )&StartTask_Handler);   /* 任务句柄 */

    vTaskStartScheduler();                              /* 开启任务调度 */
}

/**
 * @brief       start_task
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void start_task(void *pvParameters)
{
    pvParameters = pvParameters;

    taskENTER_CRITICAL();           /* 进入临界区 */

    /* 创建LVGL任务 */
    xTaskCreate((TaskFunction_t )lv_demo_task,
                (const char*    )"lv_demo_task",
                (uint16_t       )LV_DEMO_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )LV_DEMO_TASK_PRIO,
                (TaskHandle_t*  )&LV_DEMOTask_Handler);

    /* LED测试任务 */
    xTaskCreate((TaskFunction_t )led_task,
                (const char*    )"led_task",
                (uint16_t       )LED_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )LED_TASK_PRIO,
                (TaskHandle_t*  )&LEDTask_Handler);

    taskEXIT_CRITICAL();            /* 退出临界区 */
    vTaskDelete(StartTask_Handler); /* 删除开始任务 */
}

/**
 * @brief       LVGL运行例程
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void lv_demo_task(void *pvParameters)
{
    pvParameters = pvParameters;

    // lv_demo_stress();       /* 测试的demo */

    lv_obj_t* switch_obj = lv_switch_create(lv_scr_act());
	lv_obj_set_size(switch_obj, 120, 60);
	lv_obj_align(switch_obj, LV_ALIGN_CENTER, 0, 0);

    // // 按钮
    // lv_obj_t *myBtn = lv_btn_create(lv_scr_act());                               // 创建按钮; 父对象：当前活动屏幕
    // lv_obj_set_pos(myBtn, 10, 10);                                               // 设置坐标
    // lv_obj_set_size(myBtn, 120, 50);                                             // 设置大小

    // // 按钮上的文本
    // lv_obj_t *label_btn = lv_label_create(myBtn);                                // 创建文本标签，父对象：上面的btn按钮
    // lv_obj_align(label_btn, LV_ALIGN_CENTER, 0, 0);                              // 对齐于：父对象
    // lv_label_set_text(label_btn, "Test");                                        // 设置标签的文本

    // // 独立的标签
    // lv_obj_t *myLabel = lv_label_create(lv_scr_act());                           // 创建文本标签; 父对象：当前活动屏幕
    // lv_label_set_text(myLabel, "Hello world!");                                  // 设置标签的文本
    // lv_obj_align(myLabel, LV_ALIGN_CENTER, 0, 0);                                // 对齐于：父对象
    // lv_obj_align_to(myBtn, myLabel, LV_ALIGN_OUT_TOP_MID, 0, -20);               // 对齐于：某对象
    log_e("create gui ok\r\n");
    while(1)
    {
        lv_timer_handler(); /* LVGL计时器 */
        vTaskDelay(5);
    }
}

/**
 * @brief       led_task
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
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
