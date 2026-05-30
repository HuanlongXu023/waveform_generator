#include "ui_measure.h"

#include "ui_layout.h"

#include "lcd.h"


#include "ui_menu.h"

//#include "key.h"

#include "measure.h"

#include "ui_mode.h"
#include "ui_core.h"

/**
 * @brief 测量设置
 *
 * 存储测量功能的设置参数
 */
MeasureSetting_t measure_setting;

/**
 * @brief 测量占空比显示
 *
 * 该函数用于显示测量的占空比
 *
 * @param duty 占空比数值
 */
void measure_duty(uint8_t duty)
{
    if (measure_setting.pause)
        return;

    char str[4];
    sprintf(str, "%d%%", duty);
    Show_Str_Color(LEFT_STR_X_START + FONT_SIZE * 4, STR5_Y_START, (uint8_t *)str, BLUE, BLACK);
}

/**
 * @brief 测量频率显示
 *
 * 该函数用于显示测量的频率
 *
 * @param frequency 频率数值
 */
void measure_frequency(uint32_t frequency)
{
    static uint8_t pre_count = 0;

    if (measure_setting.pause)
        return;

    uint8_t count = getDigitCount(frequency);
    if (count != pre_count)
    {
        LCD_Fill(LEFT_STR_X_START + FONT_SIZE * 4, STR1_Y_START,
                 LEFT_STR_X_START + FONT_SIZE * 8, GRID_Y_2, BLACK);
    }

    uint8_t len = Show_Number_With_Separator(LEFT_STR_X_START + FONT_SIZE * 4, STR1_Y_START,
                                             frequency, WHITE, BLACK);

    if (count != pre_count)
    {
        Show_Str_Color(LEFT_STR_X_START + FONT_SIZE * 4 + len * FONT_SIZE / 2, STR1_Y_START,
                       "Hz", WHITE, BLACK);
    }
    pre_count = count;
}

/**
 * @brief 测量计数显示
 *
 * 该函数用于显示测量的计数
 *
 * @param count 计数数值
 */
void measure_count(uint32_t count)
{
    static uint8_t pre_count = 0;

    if (measure_setting.pause)
        return;

    uint8_t c = getDigitCount(count);
    if (c != pre_count)
    {
        LCD_Fill(LEFT_STR_X_START + FONT_SIZE * 4, STR1_Y_START,
                 LEFT_STR_X_START + FONT_SIZE * 8, GRID_Y_2, BLACK);
    }

    uint8_t len = Show_Number_With_Separator(LEFT_STR_X_START + FONT_SIZE * 4, STR1_Y_START,
                                             count, WHITE, BLACK);

    // if (count != pre_count)
    // {
    //     Show_Str_Color(LEFT_STR_X_START + FONT_SIZE * 4 + len * FONT_SIZE / 2, STR1_Y_START,
    //                    "us", WHITE, BLACK);
    // }
    pre_count = c;
}

/**
 * @brief 测量周期显示
 *
 * 该函数用于显示测量的周期
 *
 * @param period 周期数值（微秒）
 */
void measure_period(uint32_t period)
{
    static uint8_t pre_count = 0;

    if (measure_setting.pause)
        return;

    uint8_t count = getDigitCount(period);
    if (count != pre_count)
    {
        LCD_Fill(LEFT_STR_X_START + FONT_SIZE * 4, STR2_Y_START,
                 LEFT_STR_X_START + FONT_SIZE * 10, GRID_Y_3, BLACK);
    }
    uint8_t len = Show_Number_With_Separator(LEFT_STR_X_START + FONT_SIZE * 4, STR2_Y_START,
                                             period, GREEN, BLACK);
    if (count != pre_count)
    {
        Show_Str_Color(LEFT_STR_X_START + FONT_SIZE * 4 + len * FONT_SIZE / 2, STR2_Y_START,
                       "us", GREEN, BLACK);
    }
    pre_count = count;
}

/**
 * @brief 测量正脉冲宽度显示
 *
 * 该函数用于显示测量的正脉冲宽度
 *
 * @param positive 正脉冲宽度数值（微秒）
 */
void measure_Positive(uint32_t positive)
{
    static uint8_t pre_count = 0;
    if (measure_setting.pause)
        return;

    uint8_t count = getDigitCount(positive);
    if (count != pre_count)
    {
        LCD_Fill(LEFT_STR_X_START + FONT_SIZE * 4, STR3_Y_START,
                 LEFT_STR_X_START + FONT_SIZE * 10, GRID_Y_4, BLACK);
    }
    uint8_t len = Show_Number_With_Separator(LEFT_STR_X_START + FONT_SIZE * 4, STR3_Y_START,
                                             positive, GREEN, BLACK);
    if (count != pre_count)
    {
        Show_Str_Color(LEFT_STR_X_START + FONT_SIZE * 4 + len * FONT_SIZE / 2, STR3_Y_START,
                       "us", GREEN, BLACK);
    }
    pre_count = count;
}

/**
 * @brief 测量负脉冲宽度显示
 *
 * 该函数用于显示测量的负脉冲宽度
 *
 * @param negative 负脉冲宽度数值（微秒）
 */
void measure_Negative(uint32_t negative)
{
    static uint8_t pre_count = 0;
    uint8_t count = getDigitCount(negative);
    if (measure_setting.pause)
        return;

    if (count != pre_count)
    {
        LCD_Fill(LEFT_STR_X_START + FONT_SIZE * 4, STR4_Y_START,
                 LEFT_STR_X_START + FONT_SIZE * 8, GRID_Y_5, BLACK);
    }
    uint8_t len = Show_Number_With_Separator(LEFT_STR_X_START + FONT_SIZE * 4, STR4_Y_START,
                                             negative, GREEN, BLACK);

    if (count != pre_count)
    {
        Show_Str_Color(LEFT_STR_X_START + FONT_SIZE * 4 + len * FONT_SIZE / 2, STR4_Y_START,
                       "us", GREEN, BLACK);
    }
    pre_count = count;
}

/**
 * @brief 闸门时间设置
 *
 * 该函数用于显示闸门时间的设置
 *
 * @param index 时间索引，0表示1S，1表示10S，2表示100S
 * @param en 使能状态，1表示选中，0表示未选中
 */
void GATE_time_set(uint8_t index, uint8_t en)
{
    char str[5];
    uint16_t c = en ? WHITE : GRAY;
    uint16_t b = en ? RED : BLACK;
    uint16_t x = LEFT_STR_X_START + FONT_SIZE * 5; // 初始化x值

    if (index == 1)
    {
        sprintf(str, "10S");
    }
    else if (index == 2)
    {
        sprintf(str, "100S");
    }
    else
    {
        sprintf(str, "1S");
    }

    x += FONT_SIZE * 3 * index;
    printf("GATE_time_set=%d %s %d %d\n", index, str, c, b);
    Show_Str_Color(x, STR6_Y_START, (uint8_t *)str, c, b);
}

/**
 * @brief 设置测量闸门时间
 *
 * 该函数用于循环切换测量闸门时间
 */
void set_measure_gate(void)
{
    volatile static uint8_t pre_gate = 0;
    measure_setting.gate++;
    if (measure_setting.gate > 2)
    {
        measure_setting.gate = 0;
    }
    printf("set_measure_gate=%d, pre_gate=%d\n", measure_setting.gate, pre_gate);
    GATE_time_set(pre_gate, 0);
    GATE_time_set(measure_setting.gate, 1);

    pre_gate = measure_setting.gate;
}

/**
 * @brief 清除测量数据
 *
 * 该函数用于清除测量数据，将计数和频率重置为0
 */
void measure_clear(void)
{
    MeasureData.Count = 0;
    MeasureData.Frequency = 0;
}

/**
 * @brief 获取测量闸门时间
 *
 * 该函数用于获取当前的测量闸门时间
 *
 * @return 闸门时间，单位为秒
 */
uint8_t get_measure_gate(void)
{
    uint8_t gate = 0;
    if (measure_setting.gate == 1)
    {
        gate = 10;
    }
    else if (measure_setting.gate == 2)
    {
        gate = 100;
    }
    else
    {
        gate = 1;
    }

    return gate;
}

/**
 * @brief 显示频率或者计数
 *
 * 该函数用于根据测量模式显示频率或计数，并更新相应的菜单显示
 */
void freq_or_count(void)
{
    if (measure_setting.mode)
    {

        Show_Str_Color(LEFT_STR_X_START, STR1_Y_START,
                       "计  数:", GREEN, BLACK);

        Show_StrCenter_Color(MENU_CENTER_X, GRID_Y_1,
                             "频率", GREEN, BLACK);
        osDelay(100);
        Show_StrCenter_Color(MENU_CENTER_X, GRID_Y_1,
                             "频率", GRAY, BLACK);
    }
    else
    {
        Show_Str_Color(LEFT_STR_X_START, STR1_Y_START,
                       "频  率:", GREEN, BLACK);

        Show_StrCenter_Color(MENU_CENTER_X, GRID_Y_1,
                             "计数", GREEN, BLACK);
        osDelay(100);
        Show_StrCenter_Color(MENU_CENTER_X, GRID_Y_1,
                             "计数", GRAY, BLACK);
    }
}

/**
 * @brief 暂停或开始测量
 *
 * 该函数用于切换测量的暂停和开始状态
 */
void pause_or_start(void)
{
    if (measure_setting.pause)
    {
        DrawMenuSelected(1, GREEN);
        Show_StrCenter_Color(MENU_CENTER_X, GRID_Y_3,
                             "开始", GREEN, BLACK);
    }
    else
    {
        Show_StrCenter_Color(MENU_CENTER_X, GRID_Y_3,
                             "暂停", GRAY, BLACK);
    }
}

/**
 * @brief 刷新测量数据
 *
 * 该函数用于刷新显示所有测量数据，包括频率/计数、周期、正脉冲宽度、负脉冲宽度和占空比
 */
void refresh_measure_data(void)
{
    // LCD_Fill(LEFT_STR_X_START + FONT_SIZE * 4, STR1_Y_START,
    //          LEFT_STR_X_START + FONT_SIZE * 8, GRID_Y_5, BLACK);

    // Show_Number_With_Separator(LEFT_STR_X_START + FONT_SIZE * 4, STR1_Y_START,
    //                            MeasureData.Frequency, WHITE, BLACK);
    if (measure_setting.mode)
    {
        measure_count((uint32_t)MeasureData.Count);
    }
    else
    {
        measure_frequency((uint32_t)MeasureData.Frequency);
    }

    // Show_Number_With_Separator(LEFT_STR_X_START + FONT_SIZE * 4, STR2_Y_START,
    //                            (uint32_t)MeasureData.Period, GREEN, BLACK);

    measure_period((uint32_t)MeasureData.Period);

    // Show_Number_With_Separator(LEFT_STR_X_START + FONT_SIZE * 4, STR3_Y_START,
    //                            (uint32_t)MeasureData.Positive, GREEN, BLACK);
    measure_Positive((uint32_t)MeasureData.Positive);

    // Show_Number_With_Separator(LEFT_STR_X_START + FONT_SIZE * 4, STR4_Y_START,
    //                            (uint32_t)MeasureData.Negative, GREEN, BLACK);

    measure_Negative((uint32_t)MeasureData.Negative);

    measure_duty(MeasureData.duty);
}

/**
 * @brief 测量菜单索引处理
 *
 * 该函数用于处理测量菜单的索引操作
 *
 * @param index 测量菜单索引
 */
void measure_index_handle(uint8_t index)
{
    printf("measure_index_handle=%d\n", index);

    measure_set(index, 1);
    osDelay(100);
    measure_set(index, 0);

    switch (index)
    {
    case INDEX_0:
        LCD_Fill(LEFT_STR_X_START + FONT_SIZE * 4, STR1_Y_START,
                 LEFT_STR_X_START + FONT_SIZE * 8, GRID_Y_2, BLACK);
        measure_clear();
        measure_setting.mode = !measure_setting.mode;
        freq_or_count();
        refresh_measure_data();

        break;

    case INDEX_1:
        measure_setting.pause = !measure_setting.pause;
        pause_or_start();
        break;

    case INDEX_2:
        // Show_StrCenter_Color(MENU_CENTER_X, GRID_Y_5,
        //                      "清零", GREEN, BLACK);
        // measure_set(2, 1);
        // osDelay(100);
        // measure_set(2, 0);

        // Show_StrCenter_Color(MENU_CENTER_X, GRID_Y_5,
        //                      "清零", GRAY, BLACK);
        measure_clear();
        refresh_measure_data();

        break;

    case INDEX_3:
        // measure_set(3, 1);
        // osDelay(100);
        // measure_set(3, 0);
        set_measure_gate();
        break;
    }
}

/**
 * @brief 测量模式初始化
 *
 * 该函数用于初始化测量模式的界面和参数
 */
void measure_mode_init(void)
{
    signal_mode = MODE_MEAS;
    LCD_Clear(BLACK); //

    DrawLeftBigRect(GREEN);
    DrawLeftTopRect(GREEN);

    Show_Str_Color(LEFT_STR_X_START + FONT_SIZE / 2 * 3, STR0_Y_START,
                   "***** 测量 *****", GREEN, BLACK);

    freq_or_count();
    Show_Number_With_Separator(LEFT_STR_X_START + FONT_SIZE * 4, STR1_Y_START,
                               1000, WHITE, BLACK);

    Show_Str_Color(LEFT_STR_X_START, STR2_Y_START,
                   "周  期:", GREEN, BLACK);

    Show_Number_With_Separator(LEFT_STR_X_START + FONT_SIZE * 4, STR2_Y_START,
                               971, GREEN, BLACK);
    measure_duty(50);

    Show_Str_Color(LEFT_STR_X_START, STR3_Y_START,
                   "正脉宽:", GREEN, BLACK);
    Show_Number_With_Separator(LEFT_STR_X_START + FONT_SIZE * 4, STR3_Y_START,
                               500000, GREEN, BLACK);

    Show_Str_Color(LEFT_STR_X_START, STR4_Y_START,
                   "负脉宽:", GREEN, BLACK);
    // beep_onoff(1);
    Show_Number_With_Separator(LEFT_STR_X_START + FONT_SIZE * 4, STR4_Y_START,
                               500000, GREEN, BLACK);
    Show_Str_Color(LEFT_STR_X_START, STR5_Y_START,
                   "占空比:", GREEN, BLACK);

    Show_Str_Color(LEFT_STR_X_START, STR6_Y_START,
                   "闸门时间:", GREEN, BLACK);

    Show_Str_Color(LEFT_STR_X_START, STR8_Y_START,
                   "警告 最大安全输入电压:", YELLOW, BLACK);
    Show_Str_Color(LEFT_STR_X_START + FONT_SIZE * 9, STR9_Y_START,
                   "3.3V DA+AC", RED, BLACK);

    for (uint8_t i = 0; i < INDEX_MAX - 1; i++)
    {
        measure_set(i, 0);
    }

    GATE_time_set(0, 0);
    GATE_time_set(1, 0);
    GATE_time_set(2, 0);

    GATE_time_set(0, 1);
}