#include "ui_mode.h"

#include "ui_mode.h"
#include "lcd.h"
#include "ui_layout.h"
#include "ui_draw.h"
#include "ui_menu.h"
#include "ui_scan.h"
#include "ui_waveform.h"
#include "ui_display.h"
#include "ui_core.h"

/**
 * @brief 波形模式初始化
 *
 * 该函数用于初始化波形模式的界面和参数
 */
void wave_mode_init(void)
{
    LCD_Clear(BLACK); //

    disp_up_rect(selected_ch);
    DrawWaveformDisp();

    menu_all();

    // wave_menu_set(selected_index, 1);

    disp_down();

    // for (uint8_t i = 0; i < SET_MAX; i++)
    // {
    //     select_set(i, selected_ch, 0);
    // }
    // select_set(SET_FREQ, selected_ch, 1);

    // for (uint8_t i = 0; i < SET_MAX; i++)
    // {
    //     select_set(i, selected_ch, 0);
    // }

    disp_slected_ch_set();

    DrawWaveformSelect(0);

    refresh_selected_item();
}
