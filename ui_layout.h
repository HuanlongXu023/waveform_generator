#ifndef __UI_LAYOUT_H
#define __UI_LAYOUT_H

#include "main.h"

// 屏幕宽度
#define SCREEN_WIDTH 240
// 屏幕高度
#define SCREEN_HEIGHT 320
// 开始和结束剩余宽度
#define REMAIN_WIDTH 0

// 行数
#define GRID_COUNT 10

// 字大小
#define FONT_SIZE 16

// 每行高度
#define GRID_HEIGHT ((SCREEN_WIDTH - REMAIN_WIDTH * 2) / GRID_COUNT)

// 第0行起始Y坐标
#define GRID_Y_0 (REMAIN_WIDTH)
// 第0行中间Y坐标
#define GRID_Y_0_5 (REMAIN_WIDTH + GRID_HEIGHT / 2)
// 第0行字符串起始Y坐标
#define STR0_Y_START (GRID_Y_0_5 - FONT_SIZE / 2)

// 第一行起始Y坐标
#define GRID_Y_1 (REMAIN_WIDTH + GRID_HEIGHT)
// 第一行中间Y坐标
#define GRID_Y_1_5 (REMAIN_WIDTH + +GRID_HEIGHT * 1 + GRID_HEIGHT / 2)
// 第一行字符串起始Y坐标
#define STR1_Y_START (GRID_Y_1_5 - FONT_SIZE / 2)

// 第二行起始Y坐标
#define GRID_Y_2 (REMAIN_WIDTH + GRID_HEIGHT * 2)
// 第二行中间Y坐标
#define GRID_Y_2_5 (REMAIN_WIDTH + GRID_HEIGHT * 2 + GRID_HEIGHT / 2)
// 第二行字符串起始Y坐标
#define STR2_Y_START (GRID_Y_2_5 - FONT_SIZE / 2)

// 第三行起始Y坐标
#define GRID_Y_3 (REMAIN_WIDTH + GRID_HEIGHT * 3)
// 第三行中间Y坐标
#define GRID_Y_3_5 (REMAIN_WIDTH + GRID_HEIGHT * 3 + GRID_HEIGHT / 2)
// 第三行字符串起始Y坐标
#define STR3_Y_START (GRID_Y_3_5 - FONT_SIZE / 2)

// 第四行起始Y坐标
#define GRID_Y_4 (REMAIN_WIDTH + GRID_HEIGHT * 4)
// 第四行中间Y坐标
#define GRID_Y_4_5 (REMAIN_WIDTH + GRID_HEIGHT * 4 + GRID_HEIGHT / 2)
// 第四行字符串起始Y坐标
#define STR4_Y_START (GRID_Y_4_5 - FONT_SIZE / 2)

// 第五行起始Y坐标
#define GRID_Y_5 (REMAIN_WIDTH + GRID_HEIGHT * 5)
// 第五行中间Y坐标
#define GRID_Y_5_5 (REMAIN_WIDTH + GRID_HEIGHT * 5 + GRID_HEIGHT / 2)
// 第五行字符串起始Y坐标
#define STR5_Y_START (GRID_Y_5_5 - FONT_SIZE / 2)

// 第六行起始Y坐标
#define GRID_Y_6 (REMAIN_WIDTH + GRID_HEIGHT * 6)

// 第六行中间Y坐标
#define GRID_Y_6_5 (REMAIN_WIDTH + GRID_HEIGHT * 6 + GRID_HEIGHT / 2)
// 第六行字符串起始Y坐标
#define STR6_Y_START (GRID_Y_6_5 - FONT_SIZE / 2)

// 第七行起始Y坐标
#define GRID_Y_7 (REMAIN_WIDTH + GRID_HEIGHT * 7)
// 第七行中间Y坐标
#define GRID_Y_7_5 (REMAIN_WIDTH + GRID_HEIGHT * 7 + GRID_HEIGHT / 2)
// 第七行字符串起始Y坐标
#define STR7_Y_START (GRID_Y_7_5 - FONT_SIZE / 2)

// 第八行起始Y坐标
#define GRID_Y_8 (REMAIN_WIDTH + GRID_HEIGHT * 8)
// 第八行中间Y坐标
#define GRID_Y_8_5 (REMAIN_WIDTH + GRID_HEIGHT * 8 + GRID_HEIGHT / 2)
// 第八行字符串起始Y坐标
#define STR8_Y_START (GRID_Y_8_5 - FONT_SIZE / 2)

// 第九行起始Y坐标
#define GRID_Y_9 (REMAIN_WIDTH + GRID_HEIGHT * 9)
// 第九行中间Y坐标
#define GRID_Y_9_5 (REMAIN_WIDTH + GRID_HEIGHT * 9 + GRID_HEIGHT / 2)

// 第九行字符串起始Y坐标
#define STR9_Y_START (GRID_Y_9_5 - FONT_SIZE / 2)

// 第十行起始Y坐标
#define GRID_Y_10 (REMAIN_WIDTH + GRID_HEIGHT * 10)
// 第十行中间Y坐标
#define GRID_Y_10_5 (REMAIN_WIDTH + GRID_HEIGHT * 10 + GRID_HEIGHT / 2)
// 第十行字符串起始Y坐标
#define STR10_Y_START (GRID_Y_10_5 - FONT_SIZE / 2)

// 显示长度
#define DISP_LEN (SCREEN_HEIGHT / 10 * 8)
// 菜单中心x坐标
// #define MENU_CENTER_X (SCREEN_HEIGHT / 10 * 2 / 2 + DISP_LEN)

// 菜单长度
#define MENU_LEN (SCREEN_HEIGHT - (DISP_LEN))

// 双通道分界
#define CH_DEMARCATION (GRID_Y_6)

// 菜单x中心
#define MENU_CENTER_X (DISP_LEN + MENU_LEN / 2)

// 大圆角大小
#define BIG_ROUND_RADIUS 8

// 小圆角大小
#define SMALL_ROUND_RADIUS (BIG_ROUND_RADIUS / 2)

// 第二列字符串
#define STR2_X_START ((DISP_LEN / 2) + MENU_SPACING * 2)

// 菜单的间距
#define MENU_SPACING 6

// 顶左框开始
#define TOP_LEFT_RECT_X_START (MENU_SPACING + BIG_ROUND_RADIUS)

// 左框字符串开始
#define LEFT_STR_X_START (TOP_LEFT_RECT_X_START + SMALL_ROUND_RADIUS)

// 顶中框结束
#define TOP_MIDDLE_RECT_X_END (DISP_LEN - MENU_SPACING - BIG_ROUND_RADIUS)
// 顶中框开始
#define TOP_MIDDLE_RECT_X_START (TOP_MIDDLE_RECT_X_END - 16 * 2 - SMALL_ROUND_RADIUS * 2)

// 顶左框结束
#define TOP_LEFT_RECT_X_END (TOP_MIDDLE_RECT_X_START - MENU_SPACING)

// 扫描选项设置第一列
#define SWEEP_STR0_X_START (LEFT_STR_X_START + FONT_SIZE * 5)

// 扫描选项设置第二列
#define SWEEP_STR1_X_START (SWEEP_STR0_X_START + FONT_SIZE * 3 + FONT_SIZE / 2)

// 扫描选项设置第三列
#define SWEEP_STR2_X_START (SWEEP_STR1_X_START + FONT_SIZE * 3 + FONT_SIZE / 2)

// 波形高度
#define WAVEFORM_HEIGHT (GRID_Y_5_5 - GRID_Y_1_5 - 1)
// 波形宽度
#define WAVEFORM_WIDTH (DISP_LEN - MENU_SPACING - BIG_ROUND_RADIUS - 1 - STR2_X_START)

// 矩形边框宽度
#define RECT_BORDER_WIDTH 2

/**
 * @brief 每行字符串位置
 *
 * 存储屏幕上每行字符串的Y坐标位置
 */
// uint16_t menu_str_y[GRID_COUNT] = {STR0_Y_START, STR1_Y_START, STR2_Y_START,
//                                    STR3_Y_START, STR4_Y_START, STR5_Y_START,
//                                    STR6_Y_START, STR7_Y_START, STR8_Y_START,
//                                    STR9_Y_START};

extern uint16_t menu_str_y[GRID_COUNT];

#endif
