#include "ui_key.h"

#include "ui_key.h"
#include "lcd.h"
#include "ui_layout.h"
#include "numeric_controller.h"
#include "waveform_generator.h"
#include "ui_core.h"

#include "ui_menu.h"
#include "ui_scan.h"
#include "ui_waveform.h"
#include "ui_sync.h"
#include "ui_limit.h"
#include "ui_display.h"
#include "ui_measure.h"
#include "ui_system.h"
#include "ui_storage.h"
#include "buzzer.h"
#include "key.h"
#include "ui_mode.h"

/**
 * @brief 选择数字位
 *
 * 该函数用于选择参数的数字位，用于参数调整
 *
 * @param dir 方向，1表示向右选择，0表示向左选择
 */
void select_digit(uint8_t dir)
{
    if (dir == KEYVAL_LEFT)
    {
        // Controller[selected_index][selected_ch].selected_digit--;
        selectPrevDigit(&Controller[selected_index][selected_ch]);
    }
    else
    {
        // Controller[selected_index][selected_ch].selected_digit++;
        selectNextDigit(&Controller[selected_index][selected_ch]);
    }
}

/**
 * @brief 扫描左右选择数据
 *
 * 该函数用于选择扫描参数的数字位，用于参数调整
 *
 * @param dir 方向，1表示向右选择，0表示向左选择
 */
void sweep_select_digit(uint8_t dir)
{
    if (dir == KEYVAL_LEFT)
    {
        selectPrevDigit(&SWEEP_Contr[sweep_nub_index][DAC_CH1]);
    }
    else
    {
        selectNextDigit(&SWEEP_Contr[sweep_nub_index][DAC_CH1]);
    }

    sweep_slected_set(sweep_nub_index, 1);
}

/**
 * @brief 增加或减少数字值
 *
 * 该函数用于增加或减少参数的数值
 *
 * @param dir 方向，1表示增加，0表示减少
 */
void add_sub_digit(uint8_t dir)
{
    if (dir == KEYVAL_EC11_ADD)
    {
        incrementDigit(&Controller[selected_index][selected_ch]);
    }
    else
    {
        decrementDigit(&Controller[selected_index][selected_ch]);
    }
}

/**
 * @brief 波形类型切换
 *
 * 该函数用于循环切换波形类型
 *
 * @param dir 方向，1表示增加，0表示减少
 */
void add_sub_waveform(uint8_t dir)
{
    if (dir == KEYVAL_EC11_ADD)
    {
        wave_params[selected_ch].type++;
        wave_params[selected_ch].type %= WAVE_MAX;
    }
    else
    {
        if (wave_params[selected_ch].type == 0)
        {
            wave_params[selected_ch].type = WAVE_MAX - 1;
        }
        else
        {
            wave_params[selected_ch].type--;
        }
    }
}

/**
 * @brief 按键处理函数
 *
 * 该函数用于处理按键事件，根据不同的按键执行相应的操作
 *
 * @param btn_id 按键ID
 */
void keyval_process(uint8_t btn_id)
{
    static uint8_t pre_ch = 0;
    static uint8_t pre_mode = 0;

    switch (btn_id)
    {
        case KEYVAL_COUNT:
            measure_mode_init();
            break;

        case KEYVAL_SINE:
        case KEYVAL_RECT:
        case KEYVAL_ANY:
            if (signal_mode == MODE_WAVE || signal_mode == MODE_SWEP)
            {
                if (btn_id == KEYVAL_SINE)
                {
                    wave_params[selected_ch].type = WAVE_SINE;
                }
                else if (btn_id == KEYVAL_RECT)
                {
                    wave_params[selected_ch].type = WAVE_RECT;
                }
                else
                {
                    wave_params[selected_ch].type = WAVE_ARB1;
                }
                wave_start();
                DrawWaveformSelect(1);
                selected_index = SET_MAX;
            }

            if (signal_mode == MODE_SWEP)
            {
                sweep_nub_index = NUB_MAX;
                refresh_start_end_number();
            }

            if (signal_mode == MODE_WAVE)
            {

                refresh_selected_item();
                menu_all();

                // wave_params[selected_ch].type = WAVE_SINE;
                DrawWaveformDispUpdate();
            }
            break;
        case KEYVAL_SWEEP:
            sweep_mode_init();
            break;
        case KEYVAL_OK:
            if (signal_mode == MODE_SWEP)
                enable_sweep();
            break;

        case KEYVAL_LEFT:
        case KEYVAL_RIGHT:
            if (signal_mode == MODE_WAVE)
                select_digit(btn_id);
            else
                sweep_select_digit(btn_id);

            break;

        case KEYVAL_SYS:
            sys_mode_init();
            break;

        case KEYVAL_CH1:
            // break;
        case KEYVAL_CH2:

            if (pre_mode != signal_mode)
            {
                signal_mode = MODE_WAVE;
                wave_mode_init();
                pre_mode = signal_mode;
                return;
            }

            if (btn_id == KEYVAL_CH1)
            {
                selected_ch = DAC_CH1;
            }
            else
            {
                selected_ch = DAC_CH2;
            }

            DrawCurrentMenuSelected(selected_index);

            if (pre_ch == selected_ch)
            {
                ch_en[selected_ch] = !ch_en[selected_ch];
                disp1_en(ch_en[selected_ch]);
                DAC_CH_en(selected_ch, ch_en[selected_ch]);
                return;
            }
            // disp_slected_ch_set();
            // else
            // {
            // }
            pre_ch = selected_ch;
            disp_up_rect(selected_ch);
            disp_slected_ch_set();
            DrawWaveformSelect(selected_ch);
            disp_down();
            break;
            // case KEYVAL_RIGHT:
            //     select_digit(KEYVAL_RIGHT);
        case KEYVAL_EC11_ADD:
        case KEYVAL_EC11_SUB:

            if (signal_mode == MODE_WAVE)
            {
                if (selected_index == SET_MAX)
                {
                    add_sub_waveform(btn_id);
                    DrawWaveformSelect(1);
                }
                else
                {
                    add_sub_digit(btn_id);
                }

                sync_copy();

                limit_amp_offset();

                // DrawWaveformDispUpdate(1);

                // DrawWaveformDispUpdate(0);
                DrawWaveformDispUpdate();
                wave_start();
                if (wave_params[selected_ch].type == WAVE_AM)
                {
                    Waveform_ScanInit(selected_ch);
                    Waveform_ScanSetParams(selected_ch, SCAN_PARAM_AMP, 0, wave_params[selected_ch].amp,
                                        100, SCAN_MODE_LINEAR, SCAN_DIR_BIDIR);
                    Waveform_ScanStart(selected_ch);
                }
                else if (wave_params[selected_ch].type == WAVE_FM)
                {
                    Waveform_ScanInit(selected_ch);
                    Waveform_ScanSetParams(selected_ch, SCAN_PARAM_FREQ, wave_params[selected_ch].freq / 5,
                                        wave_params[selected_ch].freq, 100, SCAN_MODE_LOG, SCAN_DIR_BIDIR);
                    Waveform_ScanStart(selected_ch);
                }
                else if (wave_params[selected_ch].type == WAVE_CHIRP)
                {
                    Waveform_ScanInit(selected_ch);
                    Waveform_ScanSetParams(selected_ch, SCAN_PARAM_FREQ, wave_params[selected_ch].freq / 5,
                                        wave_params[selected_ch].freq, 100, SCAN_MODE_LINEAR, SCAN_DIR_BIDIR);
                    Waveform_ScanStart(selected_ch);
                }
                else
                {
                    Waveform_ScanStop(selected_ch);
                }
            }

            if (signal_mode == MODE_SWEP)
            {
                sweep_ec11_handle(btn_id);
            }

            break;

        default:
            selected_index = btn_id;

            if (signal_mode == MODE_WAVE)
            {
                DrawCurrentMenuSelected(btn_id);
            }
            else if (signal_mode == MODE_SWEP)
            {
                sweep_index_handle(btn_id);
            }
            else if (signal_mode == MODE_MEAS)
            {
                measure_index_handle(btn_id);
            }
            else if (signal_mode == MODE_SYNC)
            {
                sync_menu_set_refresh(btn_id);
            }
            else if (signal_mode == MODE_SAVE)
            {
                save_set_refresh(btn_id);
            }
            else if (signal_mode == MODE_LOAD)
            {
                save_set_refresh(btn_id);
            }
            else if (signal_mode == MODE_SYS)
            {
                sys_index_handle(btn_id);
            }

            // sweep_menu_index = btn_id;
            break;
    }

    if (signal_mode == MODE_WAVE)
    {
        if (selected_index == SET_MAX)
        {
            DrawWaveformSelect(1);
        }
        else
        {
            DrawWaveformSelect(0);
        }
    }
}




