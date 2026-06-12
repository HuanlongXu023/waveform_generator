#include "ui_sync.h"

#include "lcd.h"
#include "ui_layout.h"
#include "ui_draw.h"
#include "ui_menu.h"
#include "waveform_generator.h"
#include "ui_core.h"

/**
 * @brief 同步菜单
 *
 * 该函数用于初始化并显示所有同步菜单项，设置为默认的未选中状态
 */
void sync_menu(void)
{
    for (uint8_t i = 0; i < SET_MAX; i++)
    {
        sync_menu_set(i, 0);
    }
}

SyncData_t sync_data;

/**
 * @brief 同步菜单设置刷新
 *
 * 该函数用于切换同步菜单中各个选项的状态，并刷新显示
 *
 * @param index 菜单项索引，0=波形, 1=频率, 2=幅度, 3=偏置, 4=占空
 */
void sync_menu_set_refresh(uint8_t index)
{
    switch (index)
    {
    case 0:
        sync_data.wave ^= 1;
        sync_menu_set(index, sync_data.wave);

        break;
    case 1:
        sync_data.freq ^= 1;
        sync_menu_set(index, sync_data.freq);
        break;
    case 2:
        sync_data.amp ^= 1;
        sync_menu_set(index, sync_data.amp);
        break;
    case 3:
        sync_data.offset ^= 1;
        sync_menu_set(index, sync_data.offset);
        break;
    case 4:
        sync_data.duty ^= 1;
        sync_menu_set(index, sync_data.duty);
        break;
    default:
        break;
    }
}

/**
 * @brief 同步复制
 *
 * 该函数用于在同步模式下复制通道参数，根据同步标志位决定是否复制当前选中通道的参数到另一个通道
 */
void sync_copy(void)
{
    // uint8_t selected_index = 0;
    // // 选中的通道
    // uint8_t selected_ch = 0;
    // if (sync_data.wave)
    // {
    // }
    uint8_t ch = !selected_ch;
    switch (selected_index)
    {
    case SET_FREQ:
        if (sync_data.freq)
        {
            wave_params[ch].freq = wave_params[selected_ch].freq;
            drawUnselected(ch, selected_index);
        }
        break;
    case SET_AMP:
        if (sync_data.amp)
        {
            wave_params[ch].amp = wave_params[selected_ch].amp;
            drawUnselected(ch, selected_index);
        }
        break;
    case SET_OFFSET:
        if (sync_data.offset)
        {
            wave_params[ch].offset = wave_params[selected_ch].offset;
            drawUnselected(ch, selected_index);
        }
        break;
    case SET_DUTY:
        if (sync_data.duty)
        {
            wave_params[ch].duty = wave_params[selected_ch].duty;
            drawUnselected(ch, selected_index);
        }
        break;
    case SET_MAX:
        if (sync_data.wave)
        {
            uint16_t c;
            if (selected_ch == DAC_CH2)
            {
                c = YELLOW;
            }
            else
            {
                c = CYAN;
            }
            wave_params[ch].type = wave_params[selected_ch].type;
            // drawUnselected(ch, selected_index);
            Show_Str_Color(LEFT_STR_X_START + 8 * 5, STR6_Y_START,
                           (uint8_t *)wave_type_str[wave_params[ch].type], c, BLACK);
        }
        break;
    default:
        break;
    }
}

/**
 * @brief 同步模式初始化
 *
 * 该函数用于初始化同步模式的界面和参数
 */
void sync_mode_init(void)
{
    wave_mode_init();
    sync_menu();
    DrawTopMiddleRect(YELLOW);
    Show_Str_Color(TOP_MIDDLE_RECT_X_START + SMALL_ROUND_RADIUS + 1, STR0_Y_START, "同步", BLUE, BLACK);
}
