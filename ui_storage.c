#include "ui_storage.h"
#include "lcd.h"
#include "ui_layout.h"
#include "ui_draw.h"
#include "ui_menu.h"
#include "save_load.h"
#include "ui_mode.h"
#include "ui_core.h"

/**
 * @brief 存储页面索引
 *
 * 当前存储页面的索引
 */
uint8_t save_page;

/**
 * @brief 存储菜单设置
 *
 * 该函数用于设置存储菜单的显示状态
 *
 * @param index 菜单项索引
 * @param en 使能状态，1表示选中，0表示未选中
 */
void save_menu_set(uint8_t index, uint8_t en)
{
    char str[16];

    uint16_t c;

    if (en)
    {
        c = GREEN;
    }
    else
    {
        c = GRAY;
    }

    DrawMenuSelected(index, c);

    if (index == INDEX_4)
    {
        DrawMenuSelected(4, GRAY);
        Show_StrCenter_Color(MENU_CENTER_X, menu_pos[4], "更多", c, BLACK);
    }
    else
    {
        if (signal_mode == MODE_SAVE)
            sprintf(str, "存%02d", save_page * 4 + index + 1);
        else
            sprintf(str, "载%02d", save_page * 4 + index + 1);

        printf("%s\n", str);
        Show_StrCenter_Color(MENU_CENTER_X, menu_pos[index], (uint8_t *)str, c, BLACK);
    }
}

/**
 * @brief 存储菜单
 *
 * 该函数用于初始化存储菜单的显示，设置为默认的未选中状态
 */
void save_menu(void)
{
    for (uint8_t index = 0; index < INDEX_MAX; index++)
    {
        save_menu_set(index, 0);
    }

    // DrawMenuSelected(4, GRAY);
    // Show_StrCenter_Color(MENU_CENTER_X, menu_pos[4], "更多", GRAY, BLACK);
}

/**
 * @brief 初始化存储模式
 *
 * 该函数用于初始化存储模式的界面和参数
 */
void save_mode_init(void)
{
    wave_mode_init();
    // sync_menu();
    save_menu();
    DrawTopMiddleRect(YELLOW);
    if (signal_mode == MODE_SAVE)
        Show_Str_Color(TOP_MIDDLE_RECT_X_START + SMALL_ROUND_RADIUS + 1, STR0_Y_START, "存储", BLUE, BLACK);
    else
        Show_Str_Color(TOP_MIDDLE_RECT_X_START + SMALL_ROUND_RADIUS + 1, STR0_Y_START, "加载", BLUE, BLACK);
}

/**
 * @brief 存储设置刷新
 *
 * 该函数用于刷新存储菜单的选中状态，并执行存储或加载操作
 *
 * @param index 菜单项索引
 */
void save_set_refresh(uint8_t index)
{
    save_menu_set(index, 1);
    osDelay(100);
    save_menu_set(index, 0);

    if (signal_mode == MODE_SAVE)
    {
        save_wave(save_page * 4 + index);
    }
    else
    {
        load_wave(save_page * 4 + index);

        // save_mode_init();
        // disp_slected_ch_set();
        for (uint8_t i = 0; i < SET_MAX; i++)
        {
            select_set(i, selected_ch, 0);
        }
        DrawWaveformDispUpdate();
        wave_start();
    }

    if (index == INDEX_4)
    {
        save_page++;
        if (save_page > 4)
        {
            save_page = 0;
        }
        save_menu();
    }
}