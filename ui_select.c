#include "ui_select.h"

#include "lcd.h"
#include "ui_layout.h"
#include "numeric_controller.h"
#include "waveform_generator.h"
#include "ui_menu.h"
#include "ui_scan.h"
#include "ui_waveform.h"
#include "ui_limit.h"
#include "key.h"
#include "ui_core.h"

/**
 * @brief 绘制未选中的下面数据
 *
 * 该函数用于绘制未选中通道的参数数据
 *
 * @param ch 通道号，DAC_CH1或DAC_CH2
 * @param index 菜单索引，0=频率, 1=幅度, 2=偏置, 3=占空, 4=相位
 */
void drawUnselected(uint8_t ch, uint8_t index)
{
    uint16_t x = LEFT_STR_X_START + 2 * FONT_SIZE + 8;
    // uint16_t y = STR1_Y_START;
    uint16_t y = menu_str_y[index + 1];

    uint8_t d, max_digits;
    uint16_t c;
    uint32_t data;
    char str[2];

    if (index == SET_FREQ)
    {
        y = STR7_Y_START;
    }
    else if (index == SET_DUTY || index == SET_AMP)
    {
        y = STR8_Y_START;
    }
    else
    {
        y = STR9_Y_START;
    }

    if (index == SET_DUTY || index == SET_PHASE)
    {
        x = STR2_X_START + 2 * FONT_SIZE + 8;
    }

    if (ch == DAC_CH1)
    {
        c = YELLOW;
    }
    else
    {
        c = CYAN;
    }

    max_digits = Controller[index][ch].max_digits;

    for (uint8_t i = 0; i < max_digits; i++)
    {
        data = getControllerValue(&Controller[index][ch]);
        d = getDigitAtPosition(data, i, max_digits);

        sprintf(str, "%d", d);
        Show_Str_Color(x, y, (uint8_t *)str, c, BLACK);
        x += 8;
    }
}

/**
 * @brief 波形模式绘制选中设置数值
 *
 * @param index 菜单索引
 * @param ch 通道索引
 * @param selected 是否选中
 */
void select_set(uint8_t index, uint8_t ch, uint8_t selected)
{
    uint16_t x = LEFT_STR_X_START + 2 * FONT_SIZE + 8;
    // uint16_t y = STR1_Y_START;
    uint16_t y = menu_str_y[index + 1];

    uint8_t d, max_digits;
    uint16_t c, b;
    uint32_t data;
    char str[2];

    if (index >= SET_MAX)
        return;

    max_digits = Controller[index][ch].max_digits;
    // printf("max_digits: %d\n", max_digits);
    if (selected)
    {
        b = GRAY;
        c = WHITE;
    }
    else
    {
        b = BLACK;
        if (ch == DAC_CH1)
        {
            c = YELLOW;
        }
        else
        {
            c = CYAN;
        }
    }

    for (uint8_t i = 0; i < max_digits; i++)
    {
        data = getControllerValue(&Controller[index][ch]);
        d = getDigitAtPosition(data, i, max_digits);

        sprintf(str, "%d", d);
        // printf("str: %s\n", str);
        Show_Str_Color(x, y, (uint8_t *)str, c, b);
        x += 8;
    }

    if (selected)
    {

        d = getDigitAtPosition(data,
                               Controller[index][ch].selected_digit, max_digits);
        sprintf(str, "%d", d);

        c = WHITE;
        b = RED;
        x = LEFT_STR_X_START + 2 * FONT_SIZE + 8;
        x += 8 * Controller[index][ch].selected_digit;
        // printf("selected_digit = %d\n", Controller[index][ch].selected_digit);
        Show_Str_Color(x, y, (uint8_t *)str, c, b);
    }
}

/**
 * @brief 绘制扫频菜单的选中项
 *
 * 该函数用于绘制扫频菜单中选中的参数项，包括起始值、截止值和时间
 *
 * @param index 扫频菜单的索引 NUB_BEGIN 或 NUB_END NUB_TIME
 * @param selected 是否选中
 */
/**
 * @brief 显示扫描参数的选中状态
 * @param index 参数索引
 * @param selected 是否被选中
 */
void sweep_slected_set(uint8_t index, uint8_t selected)
{
    uint16_t x = LEFT_STR_X_START + 5 * FONT_SIZE; // 设置显示的x坐标位置
    // uint16_t y = STR1_Y_START;  // 注释掉的y坐标初始值
    uint16_t y = menu_str_y[index + 3]; // 根据索引计算y坐标位置

    uint8_t d, max_digits; // d存储数字位，max_digits存储最大位数
    uint16_t c, b;         // c存储文字颜色，b存储背景颜色
    uint32_t data;         // 存储控制器数据
    char str[8];           // 用于存储显示的字符串

    // 检查索引是否超出范围
    if (index >= NUB_MAX)
        return;

    // 获取当前索引对应的最大位数
    max_digits = SWEEP_Contr[index][DAC_CH1].max_digits;
    // printf("max_digits: %d\n", max_digits);  // 调试信息
    if (selected)
    {
        b = GRAY;  // 选中状态使用灰色背景
        c = WHITE; // 白色文字
    }
    else
    {
        b = BLACK; // 未选中状态使用黑色背景
        c = WHITE; // 白色文字
    }

    // 循环显示每一位数字
    for (uint8_t i = 0; i < max_digits; i++)
    {
        data = getControllerValue(&SWEEP_Contr[index][DAC_CH1]); // 获取控制器值
        d = getDigitAtPosition(data, i, max_digits);             // 获取指定位数的数字

        sprintf(str, "%d", d); // 将数字转换为字符串
        // printf("str: %s\n", str);  // 调试信息
        Show_Str_Color(x, y, (uint8_t *)str, c, b); // 显示数字
        x += 8;                                     // x坐标递增
    }

    x += 8; // 添加额外间距

    // 如果是开始或结束索引，显示单位
    if (NUB_BEGIN == index || NUB_END == index)
    {
        if (scan_params[DAC_CH1].parameter == SCAN_PARAM_FREQ)
        {
            sprintf(str, "%s", "Hz"); // 频率单位
        }

        if (scan_params[DAC_CH1].parameter == SCAN_PARAM_AMP ||
            scan_params[DAC_CH1].parameter == SCAN_PARAM_OFFSET)
        {
            sprintf(str, "%s", "mv"); // 幅度或偏移量单位
        }

        if (scan_params[DAC_CH1].parameter == SCAN_PARAM_DUTY)
        {
            sprintf(str, "%s", "%"); // 占空比单位
        }

        // printf("str: %s\n", str);  // 调试信息
        // Show_Str_Color(x, y, str, WHITE, BLACK);  // 注释掉的显示语句
        Show_Str_Color(x, y, (uint8_t *)str, WHITE, BLACK); // 显示单位
    }

    // 如果处于选中状态，高亮显示当前选中的数字
    if (selected)
    {

        d = getDigitAtPosition(data,
                               SWEEP_Contr[index][DAC_CH1].selected_digit, max_digits);
        sprintf(str, "%d", d); // 将选中的数字转换为字符串

        c = WHITE;                                           // 白色文字
        b = RED;                                             // 红色背景
        x = LEFT_STR_X_START + 5 * FONT_SIZE;                // 重置x坐标
        x += 8 * SWEEP_Contr[index][DAC_CH1].selected_digit; // 定位到选中的数字位置
        // printf("selected_digit = %d\n", Controller[index][ch].selected_digit);  // 调试信息
        Show_Str_Color(x, y, (uint8_t *)str, c, b); // 高亮显示选中的数字
    }

    // // 如果现在值不等与初始值且是起始索引，显示的数值改为现在
    // if (NUB_BEGIN == index)
    // {
    //     if (scan_params[DAC_CH1].current_value != scan_params[DAC_CH1].start_value)
    //     {
    //         // 循环显示每一位数字
    //         for (uint8_t i = 0; i < max_digits; i++)
    //         {
    //             // data = getControllerValue(&scan_params[DAC_CH1].current_value); // 获取控制器值
    //             // d = getDigitAtPosition(data, i, max_digits);                    // 获取指定位数的数字

    //             sprintf(str, "%d", scan_params[DAC_CH1].current_value); // 将数字转换为字符串
    //             // printf("str: %s\n", str);  // 调试信息
    //             Show_Str_Color(x, y, (uint8_t *)str, BRED, BLACK); // 显示数字
    //             x += 8;                                            // x坐标递增
    //         }
    //     }
    // }
}

// 刷新起始值
#include "ui_mode.h"

void Refresh_Start_Value(void)
{
    uint16_t x = LEFT_STR_X_START + 5 * FONT_SIZE; // 设置显示的x坐标位置
    // uint16_t y = STR1_Y_START;  // 注释掉的y坐标初始值
    uint16_t y = menu_str_y[NUB_BEGIN + 3]; // 根据索引计算y坐标位置
    char str[16];

    if (scan_params[DAC_CH1].current_value != scan_params[DAC_CH1].start_value && signal_mode == MODE_SWEP)
    {
        if (scan_params[DAC_CH1].parameter == SCAN_PARAM_FREQ)
        {
            sprintf(str, "%06d %s", scan_params[DAC_CH1].current_value, "Hz"); // 频率单位
        }

        if (scan_params[DAC_CH1].parameter == SCAN_PARAM_AMP ||
            scan_params[DAC_CH1].parameter == SCAN_PARAM_OFFSET)
        {
            sprintf(str, "%04d %s", scan_params[DAC_CH1].current_value, "mv"); // 幅度或偏移量单位
        }

        if (scan_params[DAC_CH1].parameter == SCAN_PARAM_DUTY)
        {
            sprintf(str, "%02d %s", scan_params[DAC_CH1].current_value, "%"); // 占空比单位
        }

        // sprintf(str, "%d", scan_params[DAC_CH1].current_value); // 将数字转换为字符串
        // printf("str: %s\n", str);  // 调试信息
        Show_Str_Color(x, y, (uint8_t *)str, BRED, BLACK); // 显示数字
    }
}

/**
 * @brief 闪一下当前选中的菜单项，然后灭
 *
 * 该函数用于闪烁显示当前选中的扫频菜单项，增强用户交互体验
 */
void flash_selected_item(void)
{
    static uint8_t pre_index = SWEEP_MAX;

    if (pre_index == SWEEP_TIME && selected_index != SWEEP_TIME)
    {
        sweep_slected_set(NUB_TIME, 0);

        sweep_set(SWEEP_TIME, 0);
    }

    if (selected_index == SWEEP_TIME)
    {
        sweep_set(selected_index, 1);
    }
    else if (selected_index < SWEEP_MAX && selected_index != SWEEP_TIME)
    {
        sweep_set(selected_index, 1);
        osDelay(100);
        sweep_set(selected_index, 0);

        selected_index = SWEEP_MAX;
    }

    pre_index = selected_index;
}

/**
 * @brief 处理扫频菜单的索引
 *
 * @param index 扫频菜单的索引
 */
void sweep_index_handle(uint8_t index)
{
    printf("sweep_index_handle=%d\n", index);
    switch (index)
    {
    case SWEEP_OBJE:
        add_sweep_parameter();

        copy_start_end(1, scan_params[DAC_CH1].parameter);

        Sweep_Limit();

        refresh_start_end_number();
        break;
    case SWEEP_END:
        start_or_end();
        // refresh_start_end_menu(1);

        break;
    case SWEEP_TIME:
        sweep_slected_set(NUB_TIME, 1);
        sweep_nub_index = NUB_TIME;
        break;
    case SWEEP_MODE:
        add_sweep_mode();
        break;
    case SWEEP_DIRC:
        add_sweep_dir();
        break;
    }

    static uint8_t pre;
    sweep_slected_set(pre, 0);
    sweep_slected_set(sweep_nub_index, 1);
    pre = sweep_nub_index;

    flash_selected_item();
}

/**
 * @brief 处理扫频菜单的ec11按键
 *
 * @param btn_id ec11按键的id
 */
void sweep_ec11_handle(uint8_t btn_id)
{

    if (NUB_MAX == sweep_nub_index)
    {
        add_sub_waveform(btn_id);
        DrawWaveformSelect(1);
    }
    else
    {
        if (btn_id == KEYVAL_EC11_ADD)
        {
            incrementDigit(&SWEEP_Contr[sweep_nub_index][DAC_CH1]);
        }
        else if (btn_id == KEYVAL_EC11_SUB)
        {
            decrementDigit(&SWEEP_Contr[sweep_nub_index][DAC_CH1]);
        }

        sweep_slected_set(sweep_nub_index, 1);
        uint32_t value = getControllerValue(&SWEEP_Contr[sweep_nub_index][DAC_CH1]);

        copy_start_end(0, scan_params[DAC_CH1].parameter);
    }
}

/**
 * @brief 显示选中通道的设置
 *
 * 该函数用于显示当前选中通道的所有设置项，设置为默认的未选中状态
 */
void disp_slected_ch_set(void)
{
    for (uint8_t i = 0; i < SET_MAX; i++)
    {
        select_set(i, selected_ch, 0);
    }
    // select_set(selected_index, selected_ch, 1);
}

/**
 * @brief 刷新当前选中的数值设置项
 *
 * 该函数用于刷新显示当前选中的设置项，确保只有当前选中的项被高亮显示
 */
void refresh_selected_item(void)
{
    static uint8_t pre;

    if (selected_index == SET_MAX)
    {
        for (uint8_t i = 0; i < SET_MAX; i++)
        {
            select_set(i, selected_ch, 0);
        }
        return;
    }

    if (pre != selected_index)
    {
        select_set(pre, selected_ch, 0);
    }
    // select_set(pre, DAC_CH1, 0);
    select_set(selected_index, selected_ch, 1);

    pre = selected_index;
}