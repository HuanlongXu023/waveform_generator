#include "ui_waveform.h"

#include "ui_waveform.h"
#include "lcd.h"
#include "ui_layout.h"
#include "waveform_generator.h"
#include "host_device.h"
#include "ui_core.h"

int16_t y_buffer[DAC_CH_MAX][WAVEFORM_WIDTH];

/**
 * @brief 绘制波形线条
 *
 * 该函数用于绘制波形的线条，根据提供的Y轴数据数组绘制连续的线段
 *
 * @param y0 Y轴数据数组
 * @param color 波形线条的颜色
 * @param len 数据数组的长度
 */
void draw_line_wave(int16_t *y0, uint16_t color, uint16_t len)
{
    for (uint16_t i = 0; i < len - 1; i++)
    {
        LCD_Draw_LineColor(STR2_X_START + 1 + i, y0[i] + GRID_Y_1_5,
                           STR2_X_START + 1 + i, y0[i + 1] + GRID_Y_1_5, color);
    }
}

/**
 * @brief 绘制波形显示区
 *
 * 该函数用于绘制波形显示区域，包括坐标轴和两个通道的波形
 */
void DrawWaveformDisp(void)
{
    LCD_Draw_LineColor(STR2_X_START, GRID_Y_1_5, STR2_X_START, GRID_Y_5_5 - 1, BLUE);

    LCD_Draw_LineColor(STR2_X_START, GRID_Y_5_5 - 1, DISP_LEN - MENU_SPACING - BIG_ROUND_RADIUS, GRID_Y_5_5 - 1, BLUE);

    GenerateWaveformForDisplay(&wave_params[DAC_CH1], WAVEFORM_WIDTH, WAVEFORM_HEIGHT, y_buffer[DAC_CH1]);
    GenerateWaveformForDisplay(&wave_params[DAC_CH2], WAVEFORM_WIDTH, WAVEFORM_HEIGHT, y_buffer[DAC_CH2]);

    draw_line_wave(y_buffer[DAC_CH1], YELLOW, WAVEFORM_WIDTH);  // 把相邻两点连接成直线
    draw_line_wave(y_buffer[DAC_CH2], CYAN, WAVEFORM_WIDTH);

    wave_start();
}

// extern uint16_t Waveform[DAC_CH_MAX][MAX_WAVEFORM_POINTS];

/**
 * @brief 刷新波形显示
 *
 * 该函数用于刷新波形显示，先擦除上次的波形，然后重新生成并绘制两个通道的波形
 */
void DrawWaveformDispUpdate(void)
{
    // selected_ch
    uint8_t ch = selected_ch == DAC_CH1 ? DAC_CH2 : DAC_CH1; // 另外一个通道

    // 先擦除上次的波形
    draw_line_wave(y_buffer[DAC_CH1], BLACK, WAVEFORM_WIDTH);
    draw_line_wave(y_buffer[DAC_CH2], BLACK, WAVEFORM_WIDTH);

    if (wave_params[selected_ch].type >= WAVE_ARB1)
    {
        GenerateARBWaveformForDisplay(Waveform[selected_ch], USB_BUFFER_SIZE / 2,
                                      WAVEFORM_WIDTH, WAVEFORM_HEIGHT, y_buffer[selected_ch]);
    }
    else
    {
        GenerateWaveformForDisplay(&wave_params[selected_ch], WAVEFORM_WIDTH, WAVEFORM_HEIGHT, y_buffer[selected_ch]);
    }

    if (wave_params[ch].type >= WAVE_ARB1)
    {
        GenerateARBWaveformForDisplay(Waveform[ch], USB_BUFFER_SIZE / 2,
                                      WAVEFORM_WIDTH, WAVEFORM_HEIGHT, y_buffer[ch]);
    }
    else
    {
        GenerateWaveformForDisplay(&wave_params[ch], WAVEFORM_WIDTH, WAVEFORM_HEIGHT, y_buffer[ch]);
    }

    if (selected_ch == DAC_CH1)
    {
        draw_line_wave(y_buffer[DAC_CH2], CYAN, WAVEFORM_WIDTH);

        draw_line_wave(y_buffer[DAC_CH1], YELLOW, WAVEFORM_WIDTH);
    }
    else
    {
        draw_line_wave(y_buffer[DAC_CH1], YELLOW, WAVEFORM_WIDTH);

        draw_line_wave(y_buffer[DAC_CH2], CYAN, WAVEFORM_WIDTH);
    }

    // draw_line_wave(y_buffer[index], index == DAC_CH1 ? YELLOW : CYAN, WAVEFORM_WIDTH);
}
