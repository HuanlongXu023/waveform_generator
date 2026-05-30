#ifndef __UI_SCAN_H
#define __UI_SCAN_H

#include "main.h"

#include "waveform_generator.h"
#include "numeric_controller.h"

/**
 * @brief 扫描数字枚举
 *
 * 用于标识扫描参数的类型
 */
enum
{
    NUB_BEGIN, // 起始值
    NUB_END,   // 截止值
    NUB_TIME,  // 时间
    NUB_MAX,   // 扫描参数最大数量
};

enum
{
    SWEEP_OBJE, // 扫描对象
    SWEEP_END,  // 扫描截止
    SWEEP_TIME, // 扫描时间
    SWEEP_MODE, // 扫描模式
    SWEEP_DIRC, // 扫描方向
    SWEEP_MAX,  // 扫描参数最大数量
};

extern NumericController_t SWEEP_Contr[NUB_MAX][DAC_CH_MAX];

extern uint8_t sweep_nub_index;
extern uint8_t sweep_start_end_flag;

#endif
