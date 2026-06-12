#include "ui_display.h"

#include "ui_draw.h"

#include "lcd.h"

#include "ui_layout.h"

#include "ui_menu.h"

#include "ui_core.h"
#include "waveform_generator.h"


uint8_t ch_en[DAC_CH_MAX] = {1, 1};

/**
 * 显示通道1的使能状态
 * @param en 使能状态标志，0表示禁用，非0表示使能
 * 当en为真时，在屏幕指定位置显示"ON"字符串，使用绿色前景色和黑色背景色
 */
void disp1_en(uint8_t en)
{
    uint16_t color = en ? GREEN : RED;
    char *str = en ? "ON" : "OFF";
    LCD_Fill(TOP_LEFT_RECT_X_END - FONT_SIZE / 2 * 3 - SMALL_ROUND_RADIUS, STR0_Y_START,
             TOP_LEFT_RECT_X_END - FONT_SIZE / 2 * 3 - SMALL_ROUND_RADIUS + 3 * 8, STR0_Y_START + FONT_SIZE,
             BLACK);
    Show_Str_Color(TOP_LEFT_RECT_X_END - FONT_SIZE / 2 * 3 - SMALL_ROUND_RADIUS,
                   STR0_Y_START, (uint8_t *)str, color, BLACK);
}

/**
 * @brief 显示通道2的使能状态
 *
 * 当en为真时，在屏幕指定位置显示"ON"字符串，使用绿色前景色和黑色背景色
 * 当en为假时，显示"OFF"字符串，使用红色前景色和黑色背景色
 *
 * @param en 使能状态标志，0表示禁用，非0表示使能
 */
void disp2_en(uint8_t en)
{
    uint16_t color = en ? GREEN : RED;
    char *str = en ? "ON" : "OFF";

    Show_Str_Color(TOP_LEFT_RECT_X_END - FONT_SIZE / 2 * 3 - SMALL_ROUND_RADIUS,
                   STR6_Y_START, (uint8_t *)str, color, BLACK);
}

/**
 * @brief 显示上方矩形区域
 *
 * 该函数用于在屏幕上方显示一个矩形区域，用于显示通道信息和参数
 *
 * @param ch 通道号，DAC_CH1或DAC_CH2
 */
void disp_up_rect(uint8_t ch)
{
    uint16_t color = ch == DAC_CH1 ? YELLOW : CYAN;

    LCD_Draw_RoundRect(0, GRID_Y_0_5,
                       DISP_LEN - MENU_SPACING, CH_DEMARCATION - 1,
                       BIG_ROUND_RADIUS, color);
    DrawLeftTopRect(color);

    // printf("ch_en[%d] = %d\n", ch, ch_en[ch]);
    // disp1_en(ch_en[ch]);

    if (ch == DAC_CH1)
    {
        Show_Str_Color(LEFT_STR_X_START, STR0_Y_START, "CH1=", color, BLACK);
        ch = DAC_CH2;
    }
    else
    {
        Show_Str_Color(LEFT_STR_X_START, STR0_Y_START, "CH2=", color, BLACK);
        ch = DAC_CH1;
    }

    Show_Str_Color(LEFT_STR_X_START, STR1_Y_START, "频率        Hz", color, BLACK);
    Show_Str_Color(LEFT_STR_X_START, STR2_Y_START, "幅度      mV", color, BLACK);
    Show_Str_Color(LEFT_STR_X_START, STR3_Y_START, "偏置      mV", color, BLACK);
    Show_Str_Color(LEFT_STR_X_START, STR4_Y_START, "占空    %", color, BLACK);
    Show_Str_Color(LEFT_STR_X_START, STR5_Y_START, "相位   ", color, BLACK);

    disp1_en(ch_en[ch]);
}

/**
 * @brief 显示下方矩形区域
 *
 * 该函数用于在屏幕下方显示一个矩形区域，用于显示通道信息和参数
 *
 * @param ch 通道号，DAC_CH1或DAC_CH2
 */
void disp_down_rect(uint8_t ch)
{
    uint16_t color = ch == DAC_CH1 ? YELLOW : CYAN;

    LCD_Draw_RoundRect(0, GRID_Y_6_5,
                       DISP_LEN - MENU_SPACING, SCREEN_WIDTH - 1,
                       BIG_ROUND_RADIUS, color);

    DrawLeftTopRect_2(color);
 
    if (ch == DAC_CH1)
    {
        Show_Str_Color(LEFT_STR_X_START, STR6_Y_START, "CH1=", color, BLACK);
    }
    else
    {
        Show_Str_Color(LEFT_STR_X_START, STR6_Y_START, "CH2=", color, BLACK);
    }

    // Show_Str_Color(LEFT_STR_X_START, STR6_Y_START, "CH2= 正弦波", color, BLACK);
    Show_Str_Color(LEFT_STR_X_START, STR7_Y_START, "频率         Hz", color, BLACK);
    Show_Str_Color(LEFT_STR_X_START, STR8_Y_START, "幅度      mV", color, BLACK);
    Show_Str_Color(LEFT_STR_X_START, STR9_Y_START, "偏置      mV", color, BLACK);
    Show_Str_Color(STR2_X_START, STR8_Y_START, "占空    %", color, BLACK);
    Show_Str_Color(STR2_X_START, STR9_Y_START, "相位    ", color, BLACK);
}

/**
 * @brief 显示下方通道信息
 *
 * 该函数用于显示下方通道的信息，包括通道参数和波形类型
 */
void disp_down(void)
{
    uint8_t ch;
    uint16_t c;

    if (selected_ch == DAC_CH2)
    {
        ch = DAC_CH1;
        c = YELLOW;
    }
    else
    {
        ch = DAC_CH2;
        c = CYAN;
    }

    printf("disp_down ch = %d\n", ch);
    disp_down_rect(ch);
    disp2_en(ch_en[ch]);

    for (uint8_t i = 0; i < SET_MAX; i++)
    {
        drawUnselected(ch, i);
    }
    if (wave_params[ch].type >= WAVE_ARB1)
    {
        char str[10];
        sprintf(str, "任意波%d", wave_params[ch].type - WAVE_ARB1);

        Show_Str_Color(LEFT_STR_X_START + 8 * 5, STR6_Y_START,
                       (uint8_t *)str, c, BLACK);
    }
    else
        Show_Str_Color(LEFT_STR_X_START + 8 * 5, STR6_Y_START,
                       (uint8_t *)wave_type_str[wave_params[ch].type], c, BLACK);
}

