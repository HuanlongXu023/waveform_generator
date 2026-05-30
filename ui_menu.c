#include "ui_menu.h"

#include "ui_layout.h"

#include "lcd.h"
#include "ui_core.h"


/**
 * @brief 菜单位置数组
 *
 * 存储菜单项在屏幕上的Y坐标位置
 */
uint16_t menu_pos[INDEX_MAX] = {GRID_Y_1, GRID_Y_3, GRID_Y_5, GRID_Y_7, GRID_Y_9};

char *str_chinese[MAX_STR] = {
    "频率",
    "幅度",
    "偏置",
    "占空",
    "相位",
    "对象",
    "截止",
    "时间",
    "模式",
    "方向",
    "存储",
    "加载",
    "同步",
    "配置",
    "更多",
    "计数",
    "暂停",
    "清零",
    "闸门",
    "波形",
};

// /**
//  * @brief 显示字符枚举
//  *
//  * 用于标识界面上显示的各种参数和功能选项
//  */
// enum
// {
//     FREQ, // 频率
//     AMPL, // 幅度
//     OFFS, // 偏置
//     DUTY, // 占空
//     PHAS, // 相位

//     OBJE, // 对象
//     END,  // 截止
//     TIME, // 时间
//     MODE, // 模式
//     DIRC, // 方向

//     // "存储", "加载", "同步", "配置", "更多"

//     SAVE, // 存储
//     LOAD, // 加载
//     SYNC, // 同步
//     CONF, // 配置
//     MORE, // 更多

//     COUN, // 计数
//     STOP, // 暂停
//     ZERO, // 清零
//     GATE, // 闸门
//     WAVE, // 波形

//     MAX_STR, // 最大字符串数量
// };

/**
 * @brief 菜单种类枚举
 *
 * 用于标识不同类型的菜单
 */
enum
{
    MENU_WAVE, // 波形菜单
    MENU_SWAP, // 扫描菜单
    MENU_SYS,  // 系统菜单
    MENU_MEAS, // 测量菜单

    MENU_SYNC, // 同步菜单
    MENU_MAX,  // 菜单最大数量
};

uint8_t menu_str[MENU_MAX][INDEX_MAX] =
    {{FREQ, AMPL, OFFS, DUTY, PHAS},
     {OBJE, END, TIME, MODE, DIRC},
     {SAVE, LOAD, SYNC, CONF, MORE},
     {COUN, STOP, ZERO, GATE, MORE},
     {WAVE, FREQ, AMPL, OFFS, DUTY}};

void wave_menu_set(uint8_t index, uint8_t en)
{
    // 根据使能状态设置颜色：使能时为绿色，禁用时为灰色
    uint16_t color = en ? GREEN : GRAY;
    if (index >= SET_MAX)
        return;

    // 绘制菜单项的选中状态
    DrawMenuSelected(index, color);
    // 注释掉的代码：显示菜单项的第二行文本（已禁用）
    // Show_StrCenter_Color(MENU_CENTER_X, menu_pos[index], str_chinese[menu_str[1][index]], color, BLACK);

    // 显示菜单项的第一行文本
    Show_StrCenter_Color(MENU_CENTER_X, menu_pos[index], (uint8_t *)str_chinese[menu_str[0][index]], color, BLACK);
}

/**
 * @brief 同步菜单设置
 *
 * 该函数用于设置同步菜单的显示状态，包括绘制菜单项的选中状态和显示菜单项文本
 *
 * @param index 菜单项的索引值
 * @param en 菜单项的使能状态，1表示使能，0表示禁用
 */
void sync_menu_set(uint8_t index, uint8_t en)
{
    // 根据使能状态设置颜色：使能时为绿色，禁用时为灰色
    uint16_t color = en ? GREEN : GRAY;
    if (index >= SET_MAX)
        return;

    // 绘制菜单项的选中状态
    DrawMenuSelected(index, color);

    // 显示菜单项的第一行文本
    Show_StrCenter_Color(MENU_CENTER_X, menu_pos[index], (uint8_t *)str_chinese[menu_str[MENU_SYNC][index]], color, BLACK);
}

/**
 * @brief 扫描菜单设置
 *
 * @param index 菜单项索引
 * @param en 使能
 */
void sweep_set(uint8_t index, uint8_t en)
{
    uint16_t color = en ? GREEN : GRAY;

    DrawMenuSelected(index, color);

    Show_StrCenter_Color(MENU_CENTER_X, menu_pos[index], (uint8_t *)str_chinese[menu_str[MENU_SWAP][index]], color, BLACK);
}

/**
 * @brief 系统菜单设置
 *
 * 该函数用于设置系统菜单的显示状态，包括绘制菜单项的选中状态和显示菜单项文本
 *
 * @param index 菜单项的索引值
 * @param en 菜单项的使能状态，1表示使能，0表示禁用
 */
void system_set(uint8_t index, uint8_t en)
{
    uint16_t color = en ? GREEN : GRAY;

    DrawMenuSelected(index, color);

    Show_StrCenter_Color(MENU_CENTER_X, menu_pos[index], (uint8_t *)str_chinese[menu_str[MENU_SYS][index]], color, BLACK);
}

/**
 * @brief 测量菜单设置
 *
 * 该函数用于设置测量菜单的显示状态，包括绘制菜单项的选中状态和显示菜单项文本
 *
 * @param index 菜单项的索引值
 * @param en 菜单项的使能状态，1表示使能，0表示禁用
 */
void measure_set(uint8_t index, uint8_t en)
{
    uint16_t color = en ? GREEN : GRAY;

    DrawMenuSelected(index, color);

    Show_StrCenter_Color(MENU_CENTER_X, menu_pos[index], (uint8_t *)str_chinese[menu_str[MENU_MEAS][index]], color, BLACK);
}

/**
 * @brief 绘制菜单选中项的函数
 * @param index 菜单项的索引，从0开始
 * @param color 菜单项的填充颜色
 */
void DrawMenuSelected(uint8_t index, uint16_t color)
{
    uint16_t x1, x2, y1, y2; // 定义矩形区域的坐标变量

    // 设置矩形的x坐标范围
    x1 = DISP_LEN + MENU_SPACING;      // 左边界
    x2 = SCREEN_HEIGHT + MENU_SPACING; // 右边界

    // 根据索引设置矩形的y坐标范围
    switch (index)
    {
    case 0:
        y1 = GRID_Y_0 + MENU_SPACING; // 上边界
        y2 = GRID_Y_2 - MENU_SPACING; // 下边界
        break;
    case 1: // 第二个菜单项
        y1 = GRID_Y_2 + MENU_SPACING;
        y2 = GRID_Y_4 - MENU_SPACING;
        break;
    case 2: // 第三个菜单项
        y1 = GRID_Y_4 + MENU_SPACING;
        y2 = GRID_Y_6 - MENU_SPACING;
        break;
    case 3: // 第四个菜单项
        y1 = GRID_Y_6 + MENU_SPACING;
        y2 = GRID_Y_8 - MENU_SPACING;
        break;
    case 4: // 第五个菜单项
        y1 = GRID_Y_8 + MENU_SPACING;
        y2 = GRID_Y_10 - MENU_SPACING;
        break;
    default: // 默认情况（当前被注释掉）
        // y1 = GRID_Y_0 + MENU_SPACING;
        // y2 = GRID_Y_2 - MENU_SPACING;
        break;
    }

    // 绘制外圆角矩形
    LCD_Fill_RoundRect(x1, y1,
                       x2, y2,
                       BIG_ROUND_RADIUS, color);

    // 绘制内圆角矩形（创建边框效果）
    LCD_Fill_RoundRect(x1 + RECT_BORDER_WIDTH, y1 + RECT_BORDER_WIDTH,
                       x2, y2 - +RECT_BORDER_WIDTH,
                       BIG_ROUND_RADIUS, BLACK);
}

/**
 * @brief 清除选中菜单项
 *
 * 该函数用于清除当前选中的菜单项的选中状态
 *
 * @param index 菜单项索引
 */
void ClearCurrentMenuSelected(uint8_t index)
{
    wave_menu_set(index, 0);
}

/**
 * @brief 显示当前选中的菜单项
 *
 * @param index 菜单项的索引值
 */
void DrawCurrentMenuSelected(uint8_t index)
{
    static uint8_t pre = 0;
    // if (pre == index)
    //     return;
    ClearCurrentMenuSelected(pre);
    wave_menu_set(index, 1);
    pre = index;
}

/**
 * @brief 显示所有菜单
 *
 * 该函数用于初始化并显示所有波形菜单项，设置为默认的未选中状态
 */
void menu_all(void)
{
    for (uint8_t i = 0; i < INDEX_MAX; i++)
    {
        wave_menu_set(i, 0);
    }
}

///**
// * @brief 同步菜单
// *
// * 该函数用于初始化并显示所有同步菜单项，设置为默认的未选中状态
// */
//void sync_menu(void)
//{
//    for (uint8_t i = 0; i < SET_MAX; i++)
//    {
//        sync_menu_set(i, 0);
//    }
//}

//SyncData_t sync_data;

///**
// * @brief 同步菜单设置刷新
// *
// * 该函数用于切换同步菜单中各个选项的状态，并刷新显示
// *
// * @param index 菜单项索引，0=波形, 1=频率, 2=幅度, 3=偏置, 4=占空
// */
//void sync_menu_set_refresh(uint8_t index)
//{
//    switch (index)
//    {
//    case 0:
//        sync_data.wave ^= 1;
//        sync_menu_set(index, sync_data.wave);

//        break;
//    case 1:
//        sync_data.freq ^= 1;
//        sync_menu_set(index, sync_data.freq);
//        break;
//    case 2:
//        sync_data.amp ^= 1;
//        sync_menu_set(index, sync_data.amp);
//        break;
//    case 3:
//        sync_data.offset ^= 1;
//        sync_menu_set(index, sync_data.offset);
//        break;
//    case 4:
//        sync_data.duty ^= 1;
//        sync_menu_set(index, sync_data.duty);
//        break;
//    default:
//        break;
//    }
//}

#include "waveform_generator.h"

// /**
//  * @brief 扫描参数对象切换
//  *
//  */
// void add_sweep_parameter(void)
// {
//     scan_params[selected_ch].parameter++;
//     if (scan_params[selected_ch].parameter >= SCAN_PARAM_MAX)
//     {
//         scan_params[selected_ch].parameter = SCAN_PARAM_FREQ;
//     }
//     refresh_parameter_set(scan_params[selected_ch].parameter);
// }

// /**
//  * @brief 切换扫描模式
//  *
//  * 该函数用于循环切换扫描模式（线性/对数）
//  */
// void add_sweep_mode(void)
// {
//     scan_params[selected_ch].mode++;
//     if (scan_params[selected_ch].mode >= SCAN_MODE_MAX)
//     {
//         scan_params[selected_ch].mode = SCAN_MODE_LINEAR;
//     }
//     refresh_mode_set(scan_params[selected_ch].mode);
// }

// /**
//  * @brief 切换扫描方向
//  *
//  * 该函数用于循环切换扫描方向（正向/反向/往返）
//  */
// void add_sweep_dir(void)
// {
//     scan_params[selected_ch].direction++;
//     if (scan_params[selected_ch].direction >= SCAN_DIR_MAX)
//     {
//         scan_params[selected_ch].direction = SCAN_DIR_FORWARD;
//     }
//     refresh_dir_set(scan_params[selected_ch].direction);
// }

// /**
//  * @brief 启动扫描功能的函数
//  * 该函数用于启动DAC通道1的波形扫描，并更新扫描状态
//  */
// void enable_sweep(void)
// {
//     // 注意：下面这行被注释掉了，它原本用于切换当前选中通道的启用状态
//     scan_params[selected_ch].enabled = !scan_params[selected_ch].enabled;
//     // Waveform_ScanStart(DAC_CH1);
//     refresh_status_set(scan_params[DAC_CH1].enabled);
// }

// /**
//  * @brief 扫描对象选择
//  *
//  * @param parameter 扫描参数对象 0=freq, 1=amp, 2=offset, 3=duty
//  * @param en 使能
//  */
// void sweep_parameter_set(uint8_t parameter, uint8_t en)
// {
//     uint16_t c = en ? WHITE : GRAY;
//     uint16_t b = en ? RED : BLACK;
//     uint16_t x, y;
//     char *str[SCAN_PARAM_MAX] = {"频率", "幅度", "偏置", "占空比"};

//     if (parameter == SCAN_PARAM_FREQ || parameter == SCAN_PARAM_OFFSET)
//     {
//         x = SWEEP_STR0_X_START;
//     }
//     else
//     {
//         x = SWEEP_STR1_X_START;
//     }

//     if (parameter == SCAN_PARAM_FREQ || parameter == SCAN_PARAM_AMP)
//     {
//         y = STR1_Y_START;
//     }
//     else
//     {
//         y = STR2_Y_START;
//     }

//     Show_Str_Color(x, y, (uint8_t *)str[parameter], c, b);
// }

// /**
//  * @brief 扫描模式设置
//  *
//  * @param mode 扫描模式 0=线性, 1=对数
//  * @param en 使能
//  */
// void sweep_mode_set(uint8_t mode, uint8_t en)
// {
//     uint16_t c = en ? WHITE : GRAY;
//     uint16_t b = en ? RED : BLACK;
//     uint16_t x, y;
//     char *str[SCAN_MODE_MAX] = {"线性", "对数"};

//     y = STR6_Y_START;
//     if (mode == SCAN_MODE_LINEAR)
//     {
//         x = SWEEP_STR0_X_START;
//     }
//     else
//     {
//         x = SWEEP_STR1_X_START;
//     }

//     Show_Str_Color(x, y, (uint8_t *)str[mode], c, b);
// }

// /**
//  * @brief 设置扫描方向
//  *
//  * 该函数用于设置扫描方向的显示状态
//  *
//  * @param dir 扫描方向 0=正向, 1=反向, 2=往返
//  * @param en 使能状态，1表示选中，0表示未选中
//  */
// void sweep_dir_set(uint8_t dir, uint8_t en)
// {
//     uint16_t c = en ? WHITE : GRAY;
//     uint16_t b = en ? RED : BLACK;
//     uint16_t x, y;
//     char *str[SCAN_DIR_MAX] = {"正向", "反向", "往返"};

//     y = STR7_Y_START;
//     if (dir == SCAN_DIR_FORWARD)
//     {
//         x = SWEEP_STR0_X_START;
//     }
//     else if (dir == SCAN_DIR_REVERSE)
//     {
//         x = SWEEP_STR1_X_START;
//     }
//     else
//     {
//         x = SWEEP_STR2_X_START;
//     }

//     Show_Str_Color(x, y, (uint8_t *)str[dir], c, b);
// }

/**
 * @brief 设置左侧大矩形区域
 *
 * 该函数用于初始化左侧大矩形区域，包括清空屏幕、绘制大矩形边框和顶部小矩形
 *
 * @param c 矩形的颜色
 */
void LeftBigRect_set(uint16_t c)
{

    LCD_Fill(0, 0, DISP_LEN, SCREEN_WIDTH, BLACK);
    DrawLeftBigRect(c);
    DrawLeftTopRect(c);
    // DrawTopMiddleRect(c);
}

/**
 * @brief 波形类型字符串数组
 *
 * 存储各种波形类型的中文名称
 */
const char *wave_type_str[WAVE_ARB1] = {
    "正弦波",       // WAVE_SINE
    "矩形波",       // WAVE_RECT
    "CMOS波",       // WAVE_CMOS
    "可调脉冲波",   // WAVE_PULSE
    "直流波",       // WAVE_DC
    "三角波",       // WAVE_TRIANGLE
    "正锯齿波",     // WAVE_SAWTOOTH_RISE
    "反锯齿波",     // WAVE_SAWTOOTH_FALL
    "阶梯三角波",   // WAVE_STEP_TRIANGLE
    "正阶梯波",     // WAVE_STEP_POS
    "反阶梯波",     // WAVE_STEP_NEG
    "正指数波",     // WAVE_EXP_POS
    "反指数波",     // WAVE_EXP_NEG
    "正下降指数波", // WAVE_EXP_POS_FALL
    "反下降指数波", // WAVE_EXP_NEG_FALL
    "正对数波",     // WAVE_LOG_POS
    "反对数波",     // WAVE_LOG_NEG

    "正下降对数", // WAVE_LOG_POS_FALL
    "反下降对数", // WAVE_LOG_NEG_FALL

    "正全波", // WAVE_HALF_SINE_POS (修正了注释，更准确)
    "负全波", // WAVE_HALF_SINE_NEG

    "正半波", // WAVE_RECT_POS
    "负半波", // WAVE_RECT_NEG

    "洛仑兹脉冲波", // WAVE_LORENTZ
    "多频音波",     // WAVE_MULTITONE
    "无规则噪声波", // WAVE_NOISE
    "心电图波",     // WAVE_ECG
    "梯形脉冲波",   // WAVE_TRAPEZOID
    "辛克脉冲波",   // WAVE_SINC
    "窄脉冲波",     // WAVE_NARROW_PULSE
    "高斯白噪声波", // WAVE_GAUSSIAN_NOISE
    "调幅波形",     // WAVE_AM
    "调频波形",     // WAVE_FM
    "线性调频波",   // WAVE_CHIRP
    "四脉冲波",     // WAVE_4PULSE
    "八脉冲波"      // WAVE_8PULSE
};

/**
 * @brief 显示当前选中的波形类型字符串
 *
 * @param selected
 */
void DrawWaveformSelect(uint8_t selected)
{
    uint16_t c = YELLOW, b = CYAN;
    char str[64];

    LCD_Fill(LEFT_STR_X_START + 8 * 5, STR0_Y_START,
             LEFT_STR_X_START + 8 * 5 + FONT_SIZE * 6, STR0_Y_START + FONT_SIZE,
             BLACK);

    if (selected_ch == DAC_CH1)
        c = YELLOW;
    else
        c = CYAN;

    if (selected)
        b = RED;
    else
        b = BLACK;
    if (wave_params[selected_ch].type >= WAVE_ARB1)
    {
        uint8_t n = wave_params[selected_ch].type - WAVE_ARB1 + 1;
        sprintf(str, "任意波%d", n);
        load_arb(n, selected_ch);
    }
    else
    {
        sprintf(str, "%s", wave_type_str[wave_params[selected_ch].type]);
    }
    // printf("wave = %s\n", wave_type_str[wave_params[selected_ch].type]);
    printf("wave = %s\n", str);

    Show_Str_Color(LEFT_STR_X_START + 8 * 5, STR0_Y_START,
                   (uint8_t *)str, c, b);
}

// /**
//  * @brief 刷新扫描对象选择
//  *
//  * 该函数用于刷新扫描对象的选择状态，确保只有当前选中的参数被高亮显示
//  *
//  * @param parameter 扫描参数对象 0=freq, 1=amp, 2=offset, 3=duty
//  */
// void refresh_parameter_set(uint8_t parameter)
// {
//     sweep_parameter_set(parameter, 1);

//     if (parameter == SCAN_PARAM_FREQ)
//     {
//         parameter = SCAN_PARAM_DUTY;
//     }
//     else
//     {
//         parameter--;
//     }

//     sweep_parameter_set(parameter, 0);
// }

// /**
//  * @brief 刷新扫描模式设置
//  *
//  * 该函数用于刷新扫描模式的选择状态，确保只有当前选中的模式被高亮显示
//  *
//  * @param mode 扫描模式 0=线性, 1=对数
//  */
// void refresh_mode_set(uint8_t mode)
// {
//     sweep_mode_set(mode, 1);

//     if (mode == SCAN_MODE_LINEAR)
//     {
//         mode = SCAN_MODE_LOG;
//     }
//     else
//     {
//         mode = SCAN_MODE_LINEAR;
//     }

//     sweep_mode_set(mode, 0);
// }

// /**
//  * @brief 刷新扫描方向设置
//  *
//  * 该函数用于刷新扫描方向的选择状态，确保只有当前选中的方向被高亮显示
//  *
//  * @param dir 扫描方向 0=正向, 1=反向, 2=往返
//  */
// void refresh_dir_set(uint8_t dir)
// {
//     sweep_dir_set(dir, 1);

//     if (dir == SCAN_DIR_FORWARD)
//     {
//         dir = SCAN_DIR_BIDIR;
//     }
//     else
//     {
//         dir--;
//     }

//     sweep_dir_set(dir, 0);
// }

// /**
//  * @brief 刷新扫描状态设置
//  *
//  * 该函数用于刷新扫描状态的显示，根据扫描是否启用显示不同的状态文本
//  *
//  * @param enabled 扫描状态，1表示启用，0表示禁用
//  */
// void refresh_status_set(uint8_t enabled)
// {
//     uint16_t color = enabled ? GREEN : RED;

//     char *str[2] = {"停止", "开始"};
//     // printf("enabled=%d str=%s \n", enabled, str[enabled]);
//     Show_Str_Color(SWEEP_STR0_X_START, STR8_Y_START, (uint8_t *)str[enabled], color, BLACK);
// }