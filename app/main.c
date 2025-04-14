#define LOG_TAG    "main"
#include "main.h"

// #include "lvgl.h"
// #include "lv_port_disp.h"
// #include "lv_port_indev.h"

int main(void)
{
    board_low_level_init();

    usart_init();

    elog_low_level_init();

    delay_init();

    led_init(&led0);
    led_init(&led1);
    led_init(&led2);

    // rtos
    lvgl_demo();

    // LCD_Init();
	// LCD_Clear(BLACK);
    // TP_Init();

    // 裸机
	// TIM4_Init(1000, 84); // 1ms
    // lv_init();
	// lv_port_disp_init();
	// lv_port_indev_init();
	// lv_obj_t* switch_obj = lv_switch_create(lv_scr_act());
	// lv_obj_set_size(switch_obj, 120, 60);
	// lv_obj_align(switch_obj, LV_ALIGN_CENTER, 0, 0);


	while(1)
	{
        // tp_dev.scan(0);
        // if(tp_dev.sta & TP_PRES_DOWN)
        // {
        //     LCD_ShowNum(0, 0, tp_dev.x[0], 5, 16);
        //     LCD_ShowNum(40, 0, tp_dev.y[0], 5, 16);
        //     LCD_DrawPoint(tp_dev.x[0], tp_dev.y[0]);
        //     led_toggle(&led1);
        //     led_toggle(&led2);
        // }

        // 裸机
        // delay_ms(5);
        // lv_timer_handler();


	}
}

