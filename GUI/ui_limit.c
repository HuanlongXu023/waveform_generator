#include "ui_limit.h"
#include "numeric_controller.h"
#include "waveform_generator.h"
#include "ui_scan.h"
#include "ui_menu.h"
#include "ui_core.h"

// /**
//  * @brief 扫描控制器数组
//  *
//  * 存储扫描功能的数字控制器
//  */
// NumericController_t SWEEP_Contr[NUB_MAX][DAC_CH_MAX];

NumericController_t Controller[SET_MAX][DAC_CH_MAX];

/**
 * @brief 幅度和偏置互相限制
 *
 * 该函数用于限制幅度和偏置的范围，确保它们的和不超过最大电压，且偏置不小于幅度
 */
void limit_amp_offset(void)
{
    uint16_t data, min, max;

    // 幅度 + 偏置 ≤ 3300mV
    // 偏置 - 幅度 ≥ 0
    // 设置幅度时根据偏置限制
    if (selected_index == SET_AMP)
    {
        data = getControllerValue(&Controller[SET_OFFSET][selected_ch]);
        max = 3300 - data;
        max = MIN(max, data);
        Controller[SET_AMP][selected_ch].max_value = max;
        Controller[SET_AMP][selected_ch].min_value = 1;
    }

    // 设置偏置时根据幅度限制
    if (selected_index == SET_OFFSET)
    {
        data = getControllerValue(&Controller[SET_AMP][selected_ch]);
        max = 3300 - data;
        min = data;
        Controller[SET_OFFSET][selected_ch].max_value = max;
        Controller[SET_OFFSET][selected_ch].min_value = min;
    }
}

/**
 * @brief 幅度和偏置限制初始化
 *
 * 该函数用于初始化幅度和偏置的限制范围，确保它们的和不超过最大电压，且偏置不小于幅度
 */
void limit_amp_offset_init(void)
{
    uint16_t data, min, max;

    data = getControllerValue(&Controller[SET_OFFSET][selected_ch]);
    max = 3300 - data;
    max = MIN(max, data);
    Controller[SET_AMP][selected_ch].max_value = max;
    Controller[SET_AMP][selected_ch].min_value = 1;

    data = getControllerValue(&Controller[SET_AMP][selected_ch]);
    max = 3300 - data;
    min = data;
    Controller[SET_OFFSET][selected_ch].max_value = max;
    Controller[SET_OFFSET][selected_ch].min_value = min;
}

/**
 * @brief 扫描参数限制
 *
 * 该函数用于限制扫描参数的范围，根据不同的扫描参数类型设置不同的限制
 */
void Sweep_Limit(void)
{
    uint16_t data, min, max;

    switch (scan_params[DAC_CH1].parameter)
    {
    case SCAN_PARAM_DUTY:
        initController(&SWEEP_Contr[NUB_BEGIN][DAC_CH1],
                       &scan_params[DAC_CH1].start_value,
                       sizeof(scan_params[DAC_CH1].start_value),
                       1, 99, 2);

        initController(&SWEEP_Contr[NUB_END][DAC_CH1],
                       &scan_params[DAC_CH1].end_value,
                       sizeof(scan_params[DAC_CH1].end_value),
                       1, 99, 2);
        break;

        // 幅度 + 偏置 ≤ 3300mV
        // 偏置 - 幅度 ≥ 0

    case SCAN_PARAM_AMP:  
        data = getControllerValue(&Controller[SET_OFFSET][DAC_CH1]);
        max = 3300 - data;
        max = MIN(max, data);

        initController(&SWEEP_Contr[NUB_BEGIN][DAC_CH1],
                       &scan_params[DAC_CH1].start_value,
                       sizeof(scan_params[DAC_CH1].start_value),
                       1, max, 4);

        initController(&SWEEP_Contr[NUB_END][DAC_CH1],
                       &scan_params[DAC_CH1].end_value,
                       sizeof(scan_params[DAC_CH1].end_value),
                       1, max, 4);

        break;

    case SCAN_PARAM_OFFSET:

        data = getControllerValue(&Controller[SET_AMP][DAC_CH1]);
        max = 3300 - data;
        min = data;

        initController(&SWEEP_Contr[NUB_BEGIN][DAC_CH1],
                       &scan_params[DAC_CH1].start_value,
                       sizeof(scan_params[DAC_CH1].start_value),
                       min, max, 4);

        initController(&SWEEP_Contr[NUB_END][DAC_CH1],
                       &scan_params[DAC_CH1].end_value,
                       sizeof(scan_params[DAC_CH1].end_value),
                       min, max, 4);

        break;

    default:
        initController(&SWEEP_Contr[NUB_BEGIN][DAC_CH1],
                       &scan_params[DAC_CH1].start_value,
                       sizeof(scan_params[DAC_CH1].start_value),
                       1, 100000, 6);

        initController(&SWEEP_Contr[NUB_END][DAC_CH1],
                       &scan_params[DAC_CH1].end_value,
                       sizeof(scan_params[DAC_CH1].end_value),
                       1, 100000, 6);
        break;
    }
}

/**
 * @brief 初始化限制参数
 *
 * 该函数用于初始化各种参数的控制器，设置其范围和默认值
 */
void limit_init(void)
{
    initController(&Controller[SET_FREQ][DAC_CH1],
                   &wave_params[DAC_CH1].freq,
                   sizeof(wave_params[DAC_CH1].freq),
                   1, 100000, 6);
    Controller[SET_FREQ][DAC_CH1].selected_digit = 2;
    initController(&Controller[SET_FREQ][DAC_CH2],
                   &wave_params[DAC_CH2].freq,
                   sizeof(wave_params[DAC_CH2].freq),
                   1, 100000, 6);
    Controller[SET_FREQ][DAC_CH2].selected_digit = 2;

    initController(&Controller[SET_AMP][DAC_CH1],
                   &wave_params[DAC_CH1].amp,
                   sizeof(wave_params[DAC_CH1].amp),
                   1, 3300, 4);

    initController(&Controller[SET_AMP][DAC_CH2],
                   &wave_params[DAC_CH2].amp,
                   sizeof(wave_params[DAC_CH2].amp),
                   1, 3300, 4);

    initController(&Controller[SET_OFFSET][DAC_CH1],
                   &wave_params[DAC_CH1].offset,
                   sizeof(wave_params[DAC_CH1].offset),
                   1, 3300, 4);

    initController(&Controller[SET_OFFSET][DAC_CH2],
                   &wave_params[DAC_CH2].offset,
                   sizeof(wave_params[DAC_CH2].offset),
                   1, 3300, 4);

    initController(&Controller[SET_DUTY][DAC_CH1],
                   &wave_params[DAC_CH1].duty,
                   sizeof(wave_params[DAC_CH1].duty),
                   1, 99, 2);

    initController(&Controller[SET_DUTY][DAC_CH2],
                   &wave_params[DAC_CH2].duty,
                   sizeof(wave_params[DAC_CH2].duty),
                   1, 99, 2);

    initController(&Controller[SET_PHASE][DAC_CH1],
                   &wave_params[DAC_CH1].phase,
                   sizeof(wave_params[DAC_CH1].phase),
                   0, 360, 3);

    initController(&Controller[SET_PHASE][DAC_CH2],
                   &wave_params[DAC_CH2].phase,
                   sizeof(wave_params[DAC_CH2].phase),
                   0, 360, 3);

    initController(&SWEEP_Contr[NUB_TIME][DAC_CH1],
                   &scan_params[DAC_CH1].duration_ms,
                   sizeof(scan_params[DAC_CH1].duration_ms),
                   100, 50000, 5);
    SWEEP_Contr[NUB_TIME][DAC_CH1].selected_digit = 1;

    initController(&SWEEP_Contr[NUB_BEGIN][DAC_CH1],
                   &scan_params[DAC_CH1].start_value,
                   sizeof(scan_params[DAC_CH1].start_value),
                   122, 100000, 6);
    SWEEP_Contr[NUB_BEGIN][DAC_CH1].selected_digit = 1;

    initController(&SWEEP_Contr[NUB_END][DAC_CH1],
                   &scan_params[DAC_CH1].end_value,
                   sizeof(scan_params[DAC_CH1].end_value),
                   122, 100000, 6);
    SWEEP_Contr[NUB_END][DAC_CH1].selected_digit = 1;

    limit_amp_offset_init();
}


