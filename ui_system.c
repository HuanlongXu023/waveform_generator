#include "ui_system.h"

#include "ui_layout.h"

#include "lcd.h"


#include "ui_menu.h"

#include "key.h"

#include "ui_mode.h"

#include "ui_core.h"

/**
 * @brief 蜂鸣器开关
 *
 * 该函数用于显示蜂鸣器的开关状态
 *
 * @param onoff 开关状态，1表示开启，0表示关闭
 */
void beep_onoff(uint8_t onoff)
{
    if (onoff)
        Show_Str_Color(LEFT_STR_X_START + FONT_SIZE * 4, STR4_Y_START,
                       "开", GREEN, BLACK);
    else
        Show_Str_Color(LEFT_STR_X_START + FONT_SIZE * 4, STR4_Y_START,
                       "关", RED, BLACK);
}

/**
 * @brief 系统菜单处理
 *
 * @param index 系统菜单索引
 */
void sys_index_handle(uint8_t index)
{
    if (index == INDEX_0)
    {
        // 切换存储模式
        signal_mode = signal_mode == MODE_SAVE ? MODE_LOAD : MODE_SAVE;
        save_mode_init();
    }
    else if (index == INDEX_1)
    {
        // 切换加载模式
        signal_mode = signal_mode == MODE_LOAD ? MODE_SAVE : MODE_LOAD;
        save_mode_init();
    }
    else if (index == INDEX_2)
    {
        signal_mode = MODE_SYNC;
        sync_mode_init();
        // load_wave();
    }
}

/**
 * @brief 系统模式初始化
 *
 * 该函数用于初始化系统模式的界面和参数
 */
void sys_mode_init(void)
{
    signal_mode = MODE_SYS;

    LCD_Clear(BLACK); //
    // DrawLeftTopRect(c);
    // LeftBigRect_set(BLUE);
    DrawLeftBigRect(BLUE);
    // DrawTopMiddleRect(BLUE);
    DrawLeftTopRect(BLUE);

    Show_Str_Color(LEFT_STR_X_START + FONT_SIZE / 2 * 3, STR0_Y_START,
                   "***** 系统 *****", BLUE, BLACK);

    Show_Str_Color(LEFT_STR_X_START, STR1_Y_START,
                   "型  号:", YELLOW, BLACK);

    Show_Str_Color(LEFT_STR_X_START + FONT_SIZE * 4, STR1_Y_START,
                   "SG-28", GREEN, BLACK);

    Show_Str_Color(LEFT_STR_X_START, STR2_Y_START,
                   "版  本:", YELLOW, BLACK);

    Show_Str_Color(LEFT_STR_X_START + FONT_SIZE * 4, STR2_Y_START,
                   "V1.0.2", GREEN, BLACK);

    Show_Str_Color(LEFT_STR_X_START, STR3_Y_START,
                   "语  言:", YELLOW, BLACK);

    Show_Str_Color(LEFT_STR_X_START + FONT_SIZE * 4, STR3_Y_START,
                   "中文", GREEN, BLACK);

    Show_Str_Color(LEFT_STR_X_START, STR4_Y_START,
                   "蜂鸣器:", YELLOW, BLACK);
    beep_onoff(1);

    Show_Str_Color(LEFT_STR_X_START, STR5_Y_START,
                   "同  步:", YELLOW, BLACK);

    Show_Str_Color(LEFT_STR_X_START + FONT_SIZE * 4, STR5_Y_START,
                   "无", GREEN, BLACK);

    Show_Str_Color(LEFT_STR_X_START, STR8_Y_START,
                   "更多信息请登录:", YELLOW, BLACK);

    Show_Str_Color(LEFT_STR_X_START, STR9_Y_START,
                   "oshwhub.com/youuuvuvuv/", YELLOW, BLACK);

    for (uint8_t i = 0; i < INDEX_MAX; i++)
    {
        system_set(i, 0);
    }
}