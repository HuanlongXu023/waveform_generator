#include "ui_layout.h"

#include "lcd.h"

uint16_t menu_str_y[GRID_COUNT] = {STR0_Y_START, STR1_Y_START, STR2_Y_START,
                                   STR3_Y_START, STR4_Y_START, STR5_Y_START,
                                   STR6_Y_START, STR7_Y_START, STR8_Y_START,
                                   STR9_Y_START};

/**
 * @brief 绘制界面提示和布局
 *
 * 该函数用于绘制界面的各种提示元素和布局，包括通道显示、菜单项、波形显示等
 */
void DrawDashe_tips(void)
{
    LCD_Draw_DashedLine(0, GRID_Y_0, SCREEN_HEIGHT, GRID_Y_0, WHITE, 1, 4);
    LCD_Draw_DashedLine(0, GRID_Y_1, SCREEN_HEIGHT, GRID_Y_1, WHITE, 1, 4);
    LCD_Draw_DashedLine(0, GRID_Y_2, SCREEN_HEIGHT, GRID_Y_2, WHITE, 1, 4);
    LCD_Draw_DashedLine(0, GRID_Y_3, SCREEN_HEIGHT, GRID_Y_3, WHITE, 1, 4);
    LCD_Draw_DashedLine(0, GRID_Y_4, SCREEN_HEIGHT, GRID_Y_4, WHITE, 1, 4);
    LCD_Draw_DashedLine(0, GRID_Y_5, SCREEN_HEIGHT, GRID_Y_5, WHITE, 1, 4);
    LCD_Draw_DashedLine(0, GRID_Y_6, SCREEN_HEIGHT, GRID_Y_6, WHITE, 1, 4);
    LCD_Draw_DashedLine(0, GRID_Y_7, SCREEN_HEIGHT, GRID_Y_7, WHITE, 1, 4);
    LCD_Draw_DashedLine(0, GRID_Y_8, SCREEN_HEIGHT, GRID_Y_8, WHITE, 1, 4);
    LCD_Draw_DashedLine(0, GRID_Y_9, SCREEN_HEIGHT, GRID_Y_9, WHITE, 1, 4);

    LCD_Draw_DashedLine(DISP_LEN, 0, DISP_LEN, SCREEN_WIDTH, WHITE, 1, 4);
    LCD_Draw_DashedLine(MENU_CENTER_X, 0, MENU_CENTER_X, SCREEN_WIDTH, WHITE, 1, 4);
    LCD_Draw_DashedLine(MENU_CENTER_X, 0, MENU_CENTER_X, SCREEN_WIDTH, WHITE, 1, 4);
}