#include "ui_core.h"

#include "lcd.h"
#include "ui_layout.h"
#include "numeric_controller.h"
#include "waveform_generator.h"

#include "ui_menu.h"
#include "ui_scan.h"
#include "ui_waveform.h"
#include "ui_limit.h"
#include "key.h"
#include "ui_mode.h"

/**
 * @brief 选中的索引
 *
 * 当前选中的菜单项索引
 */
uint8_t selected_index = 0;

/**
 * @brief 选中的通道
 *
 * 当前选中的通道号，DAC_CH1或DAC_CH2
 */
uint8_t selected_ch = 0;

/**
 * @brief 信号模式
 *
 * 当前系统的工作模式，如波形模式、扫描模式等
 */
uint8_t signal_mode = MODE_WAVE; // 波形

/**
 * @brief 初始化用户界面
 *
 * 该函数用于初始化用户界面，包括LCD初始化、参数限制初始化和波形模式初始化
 */
void ui_init(void)
{
    HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1); //

    LCD_Init(); //

    LCD_Display_Dir(2); //
    LCD_Clear(BLACK);   //

    limit_init();

    wave_mode_init();
}

extern osMessageQueueId_t KeyVelQueueHandle;

/**
 * @brief UI任务函数
 *
 * 该函数是UI的主任务函数，用于处理按键事件和界面刷新
 */
void ui_task(void)
{
    uint8_t btn_id;
    if (osOK == osMessageQueueGet(KeyVelQueueHandle, &btn_id, NULL, 100))
    {
        printf("btn_id: %d\n", btn_id);
        Buzzer_Start(100);
        keyval_process(btn_id);

        if (signal_mode == MODE_WAVE)
        {
            refresh_selected_item();
        }
    }
    else
    {
        if (signal_mode == MODE_MEAS)
            refresh_measure_data();
    }
}



