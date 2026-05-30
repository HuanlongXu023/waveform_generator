#include "ui_draw.h"

#include "ui_layout.h"

#include "lcd.h"

/**
 * @brief 绘制左侧大矩形边框
 *
 * 该函数用于在屏幕左侧绘制一个大的圆角矩形边框，作为主菜单区域的外框
 *
 * @param color 矩形边框的颜色
 */
void DrawLeftBigRect(uint16_t color)
{
    LCD_Draw_RoundRect(MENU_SPACING, GRID_Y_0_5,
                       DISP_LEN - MENU_SPACING, SCREEN_WIDTH - 1,
                       BIG_ROUND_RADIUS, color);
}

/**
 * @brief 左框顶小矩形
 *
 * @param color
 */
void DrawLeftTopRect(uint16_t color)
{
    LCD_Draw_RoundRect(TOP_LEFT_RECT_X_START, 1,
                       TOP_LEFT_RECT_X_END, GRID_HEIGHT - 1,
                       SMALL_ROUND_RADIUS, color);

    LCD_Fill_RoundRect(TOP_LEFT_RECT_X_START + 1, 1 + 1,
                       TOP_LEFT_RECT_X_END - 1, GRID_HEIGHT - 1 - 1,
                       SMALL_ROUND_RADIUS, BLACK);
}

/**
 * @brief 绘制顶部中间矩形
 *
 * 该函数用于在屏幕顶部中间位置绘制一个小的圆角矩形，可用于显示额外信息
 *
 * @param color 矩形的颜色
 */
void DrawTopMiddleRect(uint16_t color)
{

    LCD_Draw_RoundRect(TOP_MIDDLE_RECT_X_START, 1,
                       TOP_MIDDLE_RECT_X_END, GRID_HEIGHT - 1,
                       SMALL_ROUND_RADIUS, color);

    LCD_Fill_RoundRect(TOP_MIDDLE_RECT_X_START + 1, 1 + 1,
                       TOP_MIDDLE_RECT_X_END - 1, GRID_HEIGHT - 1 - 1,
                       SMALL_ROUND_RADIUS, BLACK);
}

/**
 * @brief 左框小矩形2
 *
 * @param color
 */
void DrawLeftTopRect_2(uint16_t color)
{
    LCD_Draw_RoundRect(TOP_LEFT_RECT_X_START, GRID_Y_6 + 1,
                       TOP_LEFT_RECT_X_END, GRID_Y_7 - 1,
                       SMALL_ROUND_RADIUS, color);

    LCD_Fill_RoundRect(TOP_LEFT_RECT_X_START + 1, GRID_Y_6 + 1 + 1,
                       TOP_LEFT_RECT_X_END - 1, GRID_Y_7 - 1 - 1,
                       SMALL_ROUND_RADIUS, BLACK);
}

// /**
//  * @brief 设置左侧大矩形区域
//  *
//  * 该函数用于初始化左侧大矩形区域，包括清空屏幕、绘制大矩形边框和顶部小矩形
//  *
//  * @param c 矩形的颜色
//  */
// void LeftBigRect_set(uint16_t c)
// {

//     LCD_Fill(0, 0, DISP_LEN, SCREEN_WIDTH, BLACK);
//     DrawLeftBigRect(c);
//     DrawLeftTopRect(c);
//     // DrawTopMiddleRect(c);
// }