#ifndef __UI_MENU_H
#define __UI_MENU_H

#include "main.h"

#include "lcd.h"

#include "waveform_generator.h"

enum
{
    INDEX_0,   // 第一个菜单项
    INDEX_1,   // 第二个菜单项
    INDEX_2,   // 第三个菜单项
    INDEX_3,   // 第四个菜单项
    INDEX_4,   // 第五个菜单项
    INDEX_MAX, // 菜单项最大数量
};

enum
{
    SET_FREQ,   // 频率设置
    SET_AMP,    // 幅度设置
    SET_OFFSET, // 偏置设置
    SET_DUTY,   // 占空比设置
    SET_PHASE,  // 相位设置
    SET_MAX,    // 设置项最大数量
};

//typedef struct
//{
//    uint8_t wave : 1;   // 波形
//    uint8_t freq : 1;   // 频率
//    uint8_t amp : 1;    // 幅度
//    uint8_t offset : 1; // 偏置
//    uint8_t duty : 1;   // 占空
//} SyncData_t;
// 同步标志位

/**
 * @brief 显示字符枚举
 *
 * 用于标识界面上显示的各种参数和功能选项
 */
enum
{
    FREQ, // 频率
    AMPL, // 幅度
    OFFS, // 偏置
    DUTY, // 占空
    PHAS, // 相位

    OBJE, // 对象
    END,  // 截止
    TIME, // 时间
    MODE, // 模式
    DIRC, // 方向

    // "存储", "加载", "同步", "配置", "更多"

    SAVE, // 存储
    LOAD, // 加载
    SYNC, // 同步
    CONF, // 配置
    MORE, // 更多

    COUN, // 计数
    STOP, // 暂停
    ZERO, // 清零
    GATE, // 闸门
    WAVE, // 波形

    MAX_STR, // 最大字符串数量
};

extern char *str_chinese[MAX_STR];
extern uint16_t menu_pos[INDEX_MAX];

extern const char *wave_type_str[WAVE_ARB1];

//extern SyncData_t sync_data;

// 菜单数据处理函数
void wave_menu_set(uint8_t index, uint8_t en);
void sync_menu_set(uint8_t index, uint8_t en);
void sweep_set(uint8_t index, uint8_t en);
void system_set(uint8_t index, uint8_t en);
void measure_set(uint8_t index, uint8_t en);

// 菜单状态管理函数
void DrawMenuSelected(uint8_t index, uint16_t color);
void ClearCurrentMenuSelected(uint8_t index);
void DrawCurrentMenuSelected(uint8_t index);
void menu_all(void);
void sync_menu(void);

// 菜单交互处理函数
void sync_menu_set_refresh(uint8_t index);
void add_sweep_parameter(void);
void add_sweep_mode(void);
void add_sweep_dir(void);
void enable_sweep(void);

// 菜单权限控制函数
void sweep_parameter_set(uint8_t parameter, uint8_t en);
void sweep_mode_set(uint8_t mode, uint8_t en);
void sweep_dir_set(uint8_t dir, uint8_t en);

// 菜单样式控制函数
void LeftBigRect_set(uint16_t c);
// void DrawDashe_tips(void);
void DrawWaveformSelect(uint8_t selected);

// 菜单辅助工具函数
void refresh_parameter_set(uint8_t parameter);
void refresh_mode_set(uint8_t mode);
void refresh_dir_set(uint8_t dir);
void refresh_status_set(uint8_t enabled);

#endif
