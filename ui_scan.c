
#include "ui_layout.h"

#include "ui_scan.h"

#include "lcd.h"

#include "ui_core.h"

#include "ui_menu.h"
#include "ui_mode.h"

extern uint16_t menu_str_y[GRID_COUNT];

uint8_t sweep_nub_index = NUB_MAX;
uint8_t sweep_start_end_flag = NUB_BEGIN;

/**
 * @brief 扫描控制器数组
 *
 * 存储扫描功能的数字控制器
 */
NumericController_t SWEEP_Contr[NUB_MAX][DAC_CH_MAX];

/**
 * @brief 扫描静态菜单
 *
 * 该函数用于设置扫描菜单的静态显示内容
 */
void swap_static_set(void)
{

    Show_Str_Color(LEFT_STR_X_START, STR1_Y_START, "扫描对象: ", CYAN, BLACK);

    Show_Str_Color(LEFT_STR_X_START, STR3_Y_START, "起    始: ", CYAN, BLACK);
    Show_Str_Color(LEFT_STR_X_START, STR4_Y_START, "截    止: ", CYAN, BLACK);
    Show_Str_Color(LEFT_STR_X_START, STR5_Y_START, "时    间: ", CYAN, BLACK);
    Show_Str_Color(LEFT_STR_X_START, STR6_Y_START, "扫描模式: ", CYAN, BLACK);
    Show_Str_Color(LEFT_STR_X_START, STR7_Y_START, "扫描方向: ", CYAN, BLACK);
    Show_Str_Color(LEFT_STR_X_START, STR8_Y_START, "扫描状态: ", CYAN, BLACK);
    Show_Str_Color(LEFT_STR_X_START, STR9_Y_START, "按下<OK>按钮开始/停止!", YELLOW, BLACK);
}

/**
 * @brief 扫描对象选择
 *
 * @param parameter 扫描参数对象 0=freq, 1=amp, 2=offset, 3=duty
 * @param en 使能
 */
void sweep_parameter_set(uint8_t parameter, uint8_t en)
{
    uint16_t c = en ? WHITE : GRAY;
    uint16_t b = en ? RED : BLACK;
    uint16_t x, y;
    char *str[SCAN_PARAM_MAX] = {"频率", "幅度", "偏置", "占空比"};

    if (parameter == SCAN_PARAM_FREQ || parameter == SCAN_PARAM_OFFSET)
    {
        x = SWEEP_STR0_X_START;
    }
    else
    {
        x = SWEEP_STR1_X_START;
    }

    if (parameter == SCAN_PARAM_FREQ || parameter == SCAN_PARAM_AMP)
    {
        y = STR1_Y_START;
    }
    else
    {
        y = STR2_Y_START;
    }

    Show_Str_Color(x, y, (uint8_t *)str[parameter], c, b);
}

/**
 * @brief 刷新扫描对象选择
 *
 * 该函数用于刷新扫描对象的选择状态，确保只有当前选中的参数被高亮显示
 *
 * @param parameter 扫描参数对象 0=freq, 1=amp, 2=offset, 3=duty
 */
void refresh_parameter_set(uint8_t parameter)
{
    sweep_parameter_set(parameter, 1);

    if (parameter == SCAN_PARAM_FREQ)
    {
        parameter = SCAN_PARAM_DUTY;
    }
    else
    {
        parameter--;
    }

    sweep_parameter_set(parameter, 0);
}

/**
 * @brief 扫描参数对象切换
 *
 */
void add_sweep_parameter(void)
{
    scan_params[selected_ch].parameter++;
    if (scan_params[selected_ch].parameter >= SCAN_PARAM_MAX)
    {
        scan_params[selected_ch].parameter = SCAN_PARAM_FREQ;
    }
    refresh_parameter_set(scan_params[selected_ch].parameter);
}

/**
 * @brief 扫描模式设置
 *
 * @param mode 扫描模式 0=线性, 1=对数
 * @param en 使能
 */
void sweep_mode_set(uint8_t mode, uint8_t en)
{
    uint16_t c = en ? WHITE : GRAY;
    uint16_t b = en ? RED : BLACK;
    uint16_t x, y;
    char *str[SCAN_MODE_MAX] = {"线性", "对数"};

    y = STR6_Y_START;
    if (mode == SCAN_MODE_LINEAR)
    {
        x = SWEEP_STR0_X_START;
    }
    else
    {
        x = SWEEP_STR1_X_START;
    }

    Show_Str_Color(x, y, (uint8_t *)str[mode], c, b);
}

/**
 * @brief 刷新扫描模式设置
 *
 * 该函数用于刷新扫描模式的选择状态，确保只有当前选中的模式被高亮显示
 *
 * @param mode 扫描模式 0=线性, 1=对数
 */
void refresh_mode_set(uint8_t mode)
{
    sweep_mode_set(mode, 1);

    if (mode == SCAN_MODE_LINEAR)
    {
        mode = SCAN_MODE_LOG;
    }
    else
    {
        mode = SCAN_MODE_LINEAR;
    }

    sweep_mode_set(mode, 0);
}

/**
 * @brief 切换扫描模式
 *
 * 该函数用于循环切换扫描模式（线性/对数）
 */
void add_sweep_mode(void)
{
    scan_params[selected_ch].mode++;
    if (scan_params[selected_ch].mode >= SCAN_MODE_MAX)
    {
        scan_params[selected_ch].mode = SCAN_MODE_LINEAR;
    }
    refresh_mode_set(scan_params[selected_ch].mode);
}

/**
 * @brief 设置扫描方向
 *
 * 该函数用于设置扫描方向的显示状态
 *
 * @param dir 扫描方向 0=正向, 1=反向, 2=往返
 * @param en 使能状态，1表示选中，0表示未选中
 */
void sweep_dir_set(uint8_t dir, uint8_t en)
{
    uint16_t c = en ? WHITE : GRAY;
    uint16_t b = en ? RED : BLACK;
    uint16_t x, y;
    char *str[SCAN_DIR_MAX] = {"正向", "反向", "往返"};

    y = STR7_Y_START;
    if (dir == SCAN_DIR_FORWARD)
    {
        x = SWEEP_STR0_X_START;
    }
    else if (dir == SCAN_DIR_REVERSE)
    {
        x = SWEEP_STR1_X_START;
    }
    else
    {
        x = SWEEP_STR2_X_START;
    }

    Show_Str_Color(x, y, (uint8_t *)str[dir], c, b);
}

/**
 * @brief 刷新扫描方向设置
 *
 * 该函数用于刷新扫描方向的选择状态，确保只有当前选中的方向被高亮显示
 *
 * @param dir 扫描方向 0=正向, 1=反向, 2=往返
 */
void refresh_dir_set(uint8_t dir)
{
    sweep_dir_set(dir, 1);

    if (dir == SCAN_DIR_FORWARD)
    {
        dir = SCAN_DIR_BIDIR;
    }
    else
    {
        dir--;
    }

    sweep_dir_set(dir, 0);
}

/**
 * @brief 切换扫描方向
 *
 * 该函数用于循环切换扫描方向（正向/反向/往返）
 */
void add_sweep_dir(void)
{
    scan_params[selected_ch].direction++;
    if (scan_params[selected_ch].direction >= SCAN_DIR_MAX)
    {
        scan_params[selected_ch].direction = SCAN_DIR_FORWARD;
    }
    refresh_dir_set(scan_params[selected_ch].direction);
}

/**
 * @brief 刷新扫描状态设置
 *
 * 该函数用于刷新扫描状态的显示，根据扫描是否启用显示不同的状态文本
 *
 * @param enabled 扫描状态，1表示启用，0表示禁用
 */
void refresh_status_set(uint8_t enabled)
{
    uint16_t color = enabled ? GREEN : RED;

    char *str[2] = {"停止", "开始"};
    // printf("enabled=%d str=%s \n", enabled, str[enabled]);
    Show_Str_Color(SWEEP_STR0_X_START, STR8_Y_START, (uint8_t *)str[enabled], color, BLACK);
}

/**
 * @brief 启动扫描功能的函数
 * 该函数用于启动DAC通道1的波形扫描，并更新扫描状态
 */
void enable_sweep(void)
{
    // 注意：下面这行被注释掉了，它原本用于切换当前选中通道的启用状态
    scan_params[selected_ch].enabled = !scan_params[selected_ch].enabled;
    // Waveform_ScanStart(DAC_CH1);
    refresh_status_set(scan_params[DAC_CH1].enabled);
}

// 显示所有静态的扫描显示
/**
 * @brief 扫描所有静态参数和显示设置
 * 该函数用于初始化和显示扫描功能的静态界面元素，包括通道选择、波形类型、
 * 频率、幅度、偏置、占空比、时间、扫描类型、方向和状态等参数的显示。
 */
void sweep_all_static(void)
{
    // 设置左侧大矩形背景色为青色
    LeftBigRect_set(CYAN);
    // 设置静态交换参数
    swap_static_set();

    // 初始化所有索引位置的扫描参数
    for (uint8_t i = 0; i < INDEX_MAX; i++)
    {
        sweep_set(i, 0);
    }

    Show_Str_Color(LEFT_STR_X_START, STR0_Y_START, "CH1=", YELLOW, BLACK);

    // 显示当前波形类型
    Show_Str_Color(LEFT_STR_X_START + 8 * 5, STR0_Y_START,
                   (uint8_t *)wave_type_str[wave_params[selected_ch].type], YELLOW, BLACK);

    // 显示参数标签：频率、幅度、偏置、占空比
    Show_Str_Color(SWEEP_STR0_X_START, STR1_Y_START, "频率", WHITE, RED);
    Show_Str_Color(SWEEP_STR1_X_START, STR1_Y_START, "幅度", GRAY, BLACK);

    Show_Str_Color(SWEEP_STR0_X_START, STR2_Y_START, "偏置", GRAY, BLACK);
    Show_Str_Color(SWEEP_STR1_X_START, STR2_Y_START, "占空比", GRAY, BLACK);

    // 显示时间参数
    Show_Str_Color(SWEEP_STR0_X_START, STR5_Y_START, "02000 ms", WHITE, BLACK);

    // 显示扫描类型：线性、对数
    Show_Str_Color(SWEEP_STR0_X_START, STR6_Y_START, "线性", WHITE, RED);
    Show_Str_Color(SWEEP_STR1_X_START, STR6_Y_START, "对数", GRAY, BLACK);

    // 显示扫描方向：正向、反向、往返
    Show_Str_Color(SWEEP_STR0_X_START, STR7_Y_START, "正向", GRAY, BLACK);
    Show_Str_Color(SWEEP_STR1_X_START, STR7_Y_START, "反向", GRAY, BLACK);
    Show_Str_Color(SWEEP_STR2_X_START, STR7_Y_START, "往返", WHITE, RED);

    // 显示状态：停止
    Show_Str_Color(SWEEP_STR0_X_START, STR8_Y_START, "停止", RED, BLACK);

    // 初始化所有扫描参数
    for (uint8_t i = 0; i < SCAN_PARAM_MAX; i++)
    {
        sweep_parameter_set(i, 0);
    }

    // 刷新当前选中的通道参数设置
    refresh_parameter_set(scan_params[selected_ch].parameter);

    // 初始化所有扫描模式
    for (uint8_t i = 0; i < SCAN_MODE_MAX; i++)
    {
        sweep_mode_set(i, 0);
    }

    // 刷新当前选中的扫描模式
    refresh_mode_set(scan_params[selected_ch].mode);

    // 初始化所有扫描方向
    for (uint8_t i = 0; i < SCAN_DIR_MAX; i++)
    {
        sweep_dir_set(i, 0);
    }

    // 刷新当前选中的扫描方向
    refresh_dir_set(scan_params[selected_ch].direction);

    // 刷新当前扫描状态（启用/禁用）
    refresh_status_set(scan_params[selected_ch].enabled);

    Show_Str_Color(TOP_MIDDLE_RECT_X_START + SMALL_ROUND_RADIUS + 1, STR0_Y_START, "扫描", YELLOW, BLACK);
}

/**
 * @brief 刷新起始结束的数字, 默认状态
 *
 * 该函数用于刷新扫描参数中起始值和截止值的显示
 */
void refresh_start_end_number(void)
{
    uint16_t x = LEFT_STR_X_START + 5 * FONT_SIZE;
    uint16_t y = menu_str_y[3];

    LCD_Fill(x, y,
             TOP_LEFT_RECT_X_END, GRID_Y_5,
             BLACK);

    for (uint8_t i = 0; i < NUB_TIME; i++)
    {
        sweep_slected_set(i, 0);
    }
}

/**
 * @brief 开始截止切换
 *
 */
void start_or_end(void)
{
    sweep_start_end_flag = !sweep_start_end_flag;
    sweep_nub_index = sweep_start_end_flag;

    if (sweep_start_end_flag)
        str_chinese[END] = "起始";
    else
        str_chinese[END] = "截止";

    printf("sweep_start_end_flag = %d %s\n", sweep_start_end_flag, str_chinese[END]);
}

/**
 * @brief 扫描模式初始化
 *
 * 该函数用于初始化扫描模式的界面和参数
 */
void sweep_mode_init(void)
{
    LCD_Clear(BLACK); //

    signal_mode = MODE_SWEP;

    sweep_all_static();

    refresh_start_end_number();

    sweep_slected_set(NUB_TIME, 0);

    Waveform_ScanSetParams(DAC_CH1, SCAN_PARAM_FREQ, 1000, 2000,
                           2000, SCAN_MODE_LINEAR, SCAN_DIR_BIDIR);

    DrawTopMiddleRect(CYAN);
    Show_Str_Color(TOP_MIDDLE_RECT_X_START + SMALL_ROUND_RADIUS + 1, STR0_Y_START, "扫描", YELLOW, BLACK);
}
