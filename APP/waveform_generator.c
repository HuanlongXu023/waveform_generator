#include "waveform_generator.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "tim.h"
#include "dac.h"
#include <stdio.h>
#include "host_device.h"

// --------------------------
// ???????????
// --------------------------
#define TRIGGER_FREQUENCY 1000000.0f // DAC???????(Hz)????TIM?????????168MHz/168=1MHz
#define REF_VOLTAGE_MV 3300.0f       // DAC???????(mV)
#define M_PI 3.14159265358979323846f // ?????(??????math.h????????)

// --------------------------
// ????????????
// --------------------------
#define STEP_WAVE_STEPS 30        // ???????????
#define EXP_WAVE_K 5.0f           // ???????????
#define LOG_WAVE_A 9.0f           // ????????????
#define LORENTZ_GAMMA 0.2f        // ????????????????
#define SINC_WAVE_K 10.0f         // Sinc???????????
#define CMOS_EDGE_RATIO 0.01f     // CMOS???????????
#define GAUSSIAN_NOISE_SCALE 3.0f // ??????????????(3????????99.7%????????)

// --------------------------
// ??????
// --------------------------
// ????????????[???][??????]
uint16_t Waveform[DAC_CH_MAX][MAX_WAVEFORM_POINTS];
// DAC???????????
float ref_voltage_mv = REF_VOLTAGE_MV;
// ????????????
ChannelParams_t wave_params[DAC_CH_MAX] = {
    {.freq = 1000,
     .amp = 1650,
     .offset = 1650,
     .duty = 50,
     .phase = 0,
     .type = WAVE_RECT},
    {.freq = 1000,
     .amp = 1650,
     .offset = 1650,
     .duty = 50,
     .phase = 0,
     .type = WAVE_SINE}};
// ??????????
uint32_t start_end[SCAN_PARAM_MAX][2] = {
    {1000, 2000}, // ??????Q??????
    {1, 1650},    // ??????Q??????
    {1650, 1650}, // ??????Q??????
    {50, 80}      // ???????Q??????
};
ScanParams_t scan_params[DAC_CH_MAX] = {
    {
        .enabled = 0,
        .mode = SCAN_MODE_LINEAR,
        .direction = SCAN_DIR_BIDIR,
        .parameter = SCAN_PARAM_FREQ,
        .start_value = 1000,
        .end_value = 2000,
        .duration_ms = 2000,
        .current_value = 1000,
    },
    {.enabled = 0,
     .mode = SCAN_MODE_LINEAR,
     .direction = SCAN_DIR_FORWARD,
     .parameter = SCAN_PARAM_FREQ}};

// --------------------------
// ??????????
// --------------------------
/**
 * @brief ???Box-Muller??????????????
 * @return ????????????????(???0??????1)
 */
static float generate_gaussian_noise(void)
{
    static float spare;
    static int has_spare = 0;

    // ????????????????
    if (has_spare)
    {
        has_spare = 0;
        return spare;
    }

    float u1, u2, s;
    do
    {
        // ????[0,1)??????????????????[-1,1)
        u1 = (float)rand() / RAND_MAX * 2.0f - 1.0f;
        u2 = (float)rand() / RAND_MAX * 2.0f - 1.0f;
        s = u1 * u1 + u2 * u2;
    } while (s >= 1.0f || s == 0.0f); // ?????????????log(0)

    // Box-Muller???
    s = sqrtf(-2.0f * logf(s) / s);
    spare = u2 * s;
    has_spare = 1;

    return u1 * s;
}

/**
 * @brief ???????????????????
 * ???????????????????????????????????????
 * @param type ????????
 * @param t ????????????[0, 1)????????????????????
 * @param duty_ratio ?????????(0, 1)
 * @param is_display ????????????(?????????????????????)
 * @return ??????????????????[-1.0, 1.0]
 */
static float CalculateNormalizedValue(uint8_t type, float t, float duty_ratio, bool is_display)
{
    float norm_val = 0.0f;

    switch (type)
    {
    // --------------------------
    // 1. ????????
    // --------------------------
    case WAVE_SINE:
        norm_val = sinf(2.0f * M_PI * t);
        break;

    case WAVE_RECT:
        norm_val = (t < duty_ratio) ? 1.0f : -1.0f;
        break;

    case WAVE_CMOS:
    {
        norm_val = (t < duty_ratio) ? 1.0f : -1.0f;

        break;
    }

    case WAVE_PULSE:
        norm_val = (t < duty_ratio) ? 1.0f : -1.0f;
        break;

    case WAVE_DC:
        // ?????????????????0???????????????????????
        norm_val = 0.0f;
        break;

    // --------------------------
    // 2. ???? & ?????
    // --------------------------
    case WAVE_TRIANGLE:
        if (t < duty_ratio)
        {
            norm_val = (t / duty_ratio) * 2.0f - 1.0f;
        }
        else
        {
            norm_val = 1.0f - ((t - duty_ratio) / (1.0f - duty_ratio)) * 2.0f;
        }
        break;

    case WAVE_SAWTOOTH_RISE:
        norm_val = 2.0f * t - 1.0f;
        break;

    case WAVE_SAWTOOTH_FALL:
        norm_val = 1.0f - 2.0f * t;
        break;

    // --------------------------
    // 3. ?????
    // --------------------------
    case WAVE_STEP_TRIANGLE:
    {
        float step_t = 1.0f / STEP_WAVE_STEPS;
        float t_step = floorf(t / step_t) * step_t;
        if (t_step < 0.5f)
            norm_val = 4.0f * t_step - 1.0f;
        else
            norm_val = 3.0f - 4.0f * t_step;
        break;
    }

    case WAVE_STEP_POS:
    {
        norm_val = -1.0f + 2.0f * floorf(t * STEP_WAVE_STEPS) / STEP_WAVE_STEPS;
        break;
    }

    case WAVE_STEP_NEG:
    {
        norm_val = 1.0f - 2.0f * floorf(t * STEP_WAVE_STEPS) / STEP_WAVE_STEPS;
        break;
    }

    // --------------------------
    // 4. ?????
    // --------------------------
    case WAVE_EXP_POS:
        // ????????????????????????????
        norm_val = 2.0f * (1.0f - expf(-EXP_WAVE_K * t)) - 1.0f;
        break;

    case WAVE_EXP_NEG:
        // ???????????????????????????
        norm_val = 2.0f * expf(-EXP_WAVE_K * t) - 1.0f;
        break;

    case WAVE_EXP_POS_FALL:
        // ????????????????????????????
        norm_val = 1.0f - 2.0f * expf(EXP_WAVE_K * (t - 1.0f));
        break;

    case WAVE_EXP_NEG_FALL:
        // ?????????????????????????????
        norm_val = 2.0f * expf(EXP_WAVE_K * (t - 1.0f)) - 1.0f;
        break;

    // --------------------------
    // 5. ??????
    // --------------------------
    case WAVE_LOG_POS:
    {
        float t_safe = (t < 0.001f) ? 0.001f : t; // ????log(0)
        norm_val = 2.0f * log1pf(LOG_WAVE_A * t_safe) / log1pf(LOG_WAVE_A) - 1.0f;
        break;
    }

    case WAVE_LOG_NEG:
    {
        float t_safe = (t < 0.001f) ? 0.001f : t;
        norm_val = 1.0f - 2.0f * log1pf(LOG_WAVE_A * t_safe) / log1pf(LOG_WAVE_A);
        break;
    }

    case WAVE_LOG_POS_FALL:
    {
        float t_rev = 1.0f - t;
        float t_safe = (t_rev < 0.001f) ? 0.001f : t_rev;
        norm_val = 2.0f * log1pf(LOG_WAVE_A * t_safe) / log1pf(LOG_WAVE_A) - 1.0f;
        break;
    }

    case WAVE_LOG_NEG_FALL:
    {
        float t_rev = 1.0f - t;
        float t_safe = (t_rev < 0.001f) ? 0.001f : t_rev;
        norm_val = 1.0f - (2.0f * log1pf(LOG_WAVE_A * t_safe) / log1pf(LOG_WAVE_A) - 1.0f);
        break;
    }

    // --------------------------
    // 6. ??????
    // --------------------------
    case WAVE_HALF_SINE_POS:
    {
        float s = sinf(2.0f * M_PI * t);
        norm_val = (s > 0) ? s : 0.0f;
        break;
    }

    case WAVE_HALF_SINE_NEG:
    {
        float s = sinf(2.0f * M_PI * t);
        norm_val = (s < 0) ? s : 0.0f;
        break;
    }

    case WAVE_RECT_POS: // ???????(??)
        norm_val = fabsf(sinf(2.0f * M_PI * t));
        break;

    case WAVE_RECT_NEG: // ???????(??)
        norm_val = -fabsf(sinf(2.0f * M_PI * t));
        break;

    // --------------------------
    // 7. ????????
    // --------------------------
    case WAVE_LORENTZ:
    {
        // ??????????
        float dt = t - 0.5f;
        norm_val = 2.0f / (1.0f + (dt / LORENTZ_GAMMA) * (dt / LORENTZ_GAMMA)) - 1.0f;
        break;
    }

    case WAVE_SINC:
    {
        // Sinc????
        float x = (t - 0.5f) * SINC_WAVE_K;
        if (fabsf(x) < 1e-6f)
            norm_val = 1.0f;
        else
            norm_val = sinf(M_PI * x) / (M_PI * x);
        norm_val = 2.0f * norm_val - 1.0f; // ???????[-1,1]
        break;
    }

    case WAVE_NARROW_PULSE:
    {
        // ???????????????????????
        float visible_width = 0.05f + (duty_ratio / 5.0f);
        const float p1_center = 0.25f;
        const float p2_center = 0.5f;
        const float p3_center = 0.75f;

        float d1 = fabsf(t - p1_center);
        float d2 = fabsf(t - p2_center);
        float d3 = fabsf(t - p3_center);

        norm_val = -1.0f; // ??????
        if (d1 < visible_width / 2)
            norm_val = 0.0f; // ?????????????
        else if (d2 < visible_width / 2)
            norm_val = 1.0f; // ?????????????
        else if (d3 < visible_width / 2)
            norm_val = 0.0f; // ??????????????

        break;
    }

    case WAVE_TRAPEZOID:
    {
        // ??????????-??-??-??-??
        const float segment_len = 1.0f / 5.0f; // ????20%????
        if (t < segment_len)
        {
            norm_val = -1.0f; // ???????
        }
        else if (t < 2 * segment_len)
        {
            float local_t = (t - segment_len) / segment_len;
            norm_val = 2.0f * local_t - 1.0f; // ??????
        }
        else if (t < 3 * segment_len)
        {
            norm_val = 1.0f; // ????????
        }
        else if (t < 4 * segment_len)
        {
            float local_t = (t - 3 * segment_len) / segment_len;
            norm_val = 1.0f - 2.0f * local_t; // ?????
        }
        else
        {
            norm_val = -1.0f; // ???????
        }
        break;
    }

    case WAVE_ECG:
    {
        // ?????????
        if (t < 0.1f)
            norm_val = 0.15f * sinf(2.0f * M_PI * t / 0.1f);
        else if (t < 0.16f)
            norm_val = 0.0f;
        else if (t < 0.18f)
            norm_val = -0.3f * (1.0f - (t - 0.16f) / 0.02f);
        else if (t < 0.22f)
            norm_val = 1.0f * ((t - 0.18f) / 0.04f);
        else if (t < 0.26f)
            norm_val = -0.4f * (1.0f - (t - 0.22f) / 0.04f);
        else if (t < 0.35f)
            norm_val = 0.0f;
        else if (t < 0.55f)
            norm_val = 0.3f * sinf(2.0f * M_PI * (t - 0.35f) / 0.2f);
        else
            norm_val = 0.0f;
        break;
    }
 
    // --------------------------
    // 8. ??????????
    // --------------------------
    case WAVE_MULTITONE:
        float sum = 0.0f;
        float total_amp = 0.0f;
        float decay = 0.2f + 0.6f * duty_ratio;

        // ????4??????????????????????????
        const uint8_t harmonic_cnt = 4;
        const uint8_t freqs[] = {1, 3, 5, 7};

        for (uint8_t i = 0; i < harmonic_cnt; i++)
        {
            float amp = powf(decay, i);
            sum += amp * sinf(2.0f * M_PI * freqs[i] * t);
            total_amp += amp;
        }

        norm_val = sum / total_amp;
        if (norm_val > 1.0f)
            norm_val = 1.0f;
        if (norm_val < -1.0f)
            norm_val = -1.0f;
        break;

    case WAVE_CHIRP:
    {
        if (is_display)
        {
            // ???????????????????????????????
            const float f_min = 2.0f;  // ??????????
            const float f_max = 15.0f; // ??????????
            // ?????????????t=0/1????????t=0.5????
            float freq_mod = f_min + (f_max - f_min) * (2.0f * t - 1.0f) * (2.0f * t - 1.0f);
            // ??????????
            float phi = 2.0f * M_PI * (f_min * t + (f_max - f_min) * ((4.0f / 3.0f) * t * t * t - 2.0f * t * t + t));
            norm_val = sinf(phi);
        }
        else
        {
            // ?????????????????????????????
            norm_val = sinf(2.0f * M_PI * t);
        }
        break;
    }

    case WAVE_AM:
    {
        if (is_display)
        {
            // ????????????????????????????
            float envelope = sinf(M_PI * t);
            envelope = envelope * envelope;                // ????????0->1->0
            float carrier = sinf(2.0f * M_PI * 12.0f * t); // ??????
            norm_val = envelope * carrier;
        }
        else
        {
            // ?????????????????????????????
            norm_val = sinf(2.0f * M_PI * t);
        }
        break;
    }

    case WAVE_FM:
    {
        if (is_display)
        {
            // ???????????????????????????
            const float f_min = 2.0f;  // ??????????
            const float f_max = 15.0f; // ??????????
            float freq_mod = f_min + (f_max - f_min) * (2.0f * t - 1.0f) * (2.0f * t - 1.0f);
            float phi = 2.0f * M_PI * (f_min * t + (f_max - f_min) * ((4.0f / 3.0f) * t * t * t - 2.0f * t * t + t));
            norm_val = sinf(phi);
        }
        else
        {
            // ?????????????????????????????
            norm_val = sinf(2.0f * M_PI * t);
        }
        break;
    }

    // --------------------------
    // 9. ????
    // --------------------------
    case WAVE_NOISE:
        // ????????????
        norm_val = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        break;

    case WAVE_GAUSSIAN_NOISE:
    {
        // ???????????3????????[-1,1]
        norm_val = generate_gaussian_noise() / GAUSSIAN_NOISE_SCALE;
        // ?????????????????
        if (norm_val > 1.0f)
            norm_val = 1.0f;
        if (norm_val < -1.0f)
            norm_val = -1.0f;
        break;
    }

    // --------------------------
    // 10. ??????????
    // --------------------------
    case WAVE_4PULSE:
    {
        if (t >= 0.5f)
        {
            norm_val = -1.0f; // ??????????
        }
        else
        {
            // ????????????????4??????
            float compressed_t = t * 2.0f;
            float sub_t = fmodf(compressed_t * 4.0f, 1.0f);
            float pulse_half_width = duty_ratio / 2.0f;

            if (fabsf(sub_t - 0.5f) < pulse_half_width)
                norm_val = 1.0f;
            else
                norm_val = -1.0f;
        }
        break;
    }

    case WAVE_8PULSE:
    {
        if (t >= 0.5f)
        {
            norm_val = -1.0f; // ??????????
        }
        else
        {
            // ????????????????8??????
            float compressed_t = t * 2.0f;
            float sub_t = fmodf(compressed_t * 8.0f, 1.0f);
            float pulse_half_width = duty_ratio / 2.0f;

            if (fabsf(sub_t - 0.5f) < pulse_half_width)
                norm_val = 1.0f;
            else
                norm_val = -1.0f;
        }
        break;
    }

    default:
        // ????????????0???
        norm_val = 0.0f;
        break;
    }

    return norm_val;
}

#define MAX_TRIGGER_FREQUENCY 1000000.0f // ?????????1MHz???????????

// --------------------------
// ????????????????????
// --------------------------
/**
 * @brief ???????????DAC?????????????
 * @param params  ????????
 * @param buffer  ?????????
 * @param length  ???????????????????
 */
void GenerateWaveform(ChannelParams_t *params, uint16_t *buffer, uint32_t *length)
{
    // ?????????
    if (params == NULL || buffer == NULL || length == NULL)
        return;

    // 1. ???????????????????
    uint32_t targetN = 1;
    if (params->freq > 0)
    {
        // ??????? / ??????? = ???????????????
        targetN = (uint32_t)(TRIGGER_FREQUENCY / params->freq);
    }

    float actual_trigger_freq;
    if (params->freq > 0)
    {
        // ?????????????????????????????????????????????????
        actual_trigger_freq = (float)params->freq * MAX_WAVEFORM_POINTS;
        if (actual_trigger_freq > MAX_TRIGGER_FREQUENCY)
        {
            actual_trigger_freq = MAX_TRIGGER_FREQUENCY; // ????????1MHz??????
        }
        targetN = (uint32_t)(actual_trigger_freq / params->freq);
    }

    // ?????????????????????????????
    const uint32_t minPoints = 8;
    const uint32_t maxPoints = MAX_WAVEFORM_POINTS;
    if (targetN < minPoints)
        targetN = minPoints;
    if (targetN > maxPoints)
        targetN = maxPoints;
    *length = targetN;

    // 2. ???????????????????????????
    float amp_mv = (float)params->amp;
    float offset_mv = (float)params->offset;
    float duty_ratio = (float)params->duty / 100.0f;
    float phase_shift = (float)params->phase / 360.0f;

    // ?????????????????????
    if (duty_ratio < 0.001f)
        duty_ratio = 0.001f;
    if (duty_ratio > 0.999f)
        duty_ratio = 0.999f;

    // 3. ????????????????
    for (uint32_t i = 0; i < targetN; i++)
    {
        // ??????????????????????
        float t = (float)i / (float)targetN + phase_shift;
        // ?????t??[0, 1)????
        if (t >= 1.0f)
            t -= 1.0f;
        if (t < 0.0f)
            t += 1.0f;

        // ????????????????????????
        float norm_val = CalculateNormalizedValue(params->type, t, duty_ratio, false);

        // 4. ????????????
        float voltage_mv;
        if (params->type == WAVE_DC)
        {
            // ???????????????????????????
            voltage_mv = offset_mv;
        }
        else
        {
            // ????????Offset + Amp * Norm
            voltage_mv = offset_mv + amp_mv * norm_val;
        }

        // ????????????????????DAC????????????????
        if (voltage_mv < 0)
            voltage_mv = 0;
        if (voltage_mv > ref_voltage_mv)
            voltage_mv = ref_voltage_mv;

        // ????DAC??????????????????
        buffer[i] = (uint16_t)(voltage_mv * ((float)DAC_MAX_VALUE) / ref_voltage_mv + 0.5f);
    }
}

/**
 * @brief ???????????????????????????
 * @param params ????????
 * @param htim ????????
 * @param ch_dac DAC??? (DAC_CHANNEL_1 ?? DAC_CHANNEL_2)
 * @retval uint32_t ?????????
 */
uint32_t ConfigureWaveformChannel(ChannelParams_t *params, TIM_HandleTypeDef *htim, uint32_t ch_dac)
{
    uint32_t target_freq = params->freq;
    const uint32_t timer_clk = 84000000;
    const uint32_t max_trigger = 1000000;
    const uint32_t min_points = 8;
    const uint32_t max_points = MAX_WAVEFORM_POINTS;

    uint32_t N = max_trigger / target_freq;

    if (target_freq > 0)
    {
        uint32_t f_trigger = target_freq * max_points;
        if (f_trigger > max_trigger)
        {
            f_trigger = max_trigger;
        }
        N = f_trigger / target_freq;
    }

    if (N < min_points)
        N = min_points;
    if (N > max_points)
        N = max_points;

    uint32_t f_trigger = N * target_freq;

    uint32_t div = timer_clk / f_trigger;
    if (div < 1)
        div = 1;

    uint32_t psc, arr;
    if (div <= 65536)
    {
        psc = 0;
        arr = div - 1;
    }
    else
    {
        psc = (uint32_t)sqrt(div) - 1;
        if (psc > 65535)
            psc = 65535;
        arr = div / (psc + 1) - 1;
        if (arr > 65535)
        {
            arr = 65535;
            psc = div / (arr + 1) - 1;
            if (psc > 65535)
                psc = 65535;
        }
    }

    uint8_t ch = (ch_dac == DAC_CHANNEL_1) ? 0 : 1;
    uint32_t numPoints = N;
    GenerateWaveform(params, Waveform[ch], &numPoints);

    HAL_TIM_Base_Stop(htim);
    HAL_DAC_Stop_DMA(&hdac, ch_dac);
    HAL_DAC_Stop(&hdac, ch_dac);

    __HAL_TIM_SET_PRESCALER(htim, psc);
    __HAL_TIM_SET_AUTORELOAD(htim, arr);

    if (HAL_DAC_Start_DMA(&hdac, ch_dac, (uint32_t *)Waveform[ch], numPoints, DAC_ALIGN_12B_R) != HAL_OK)
    {
        Error_Handler();
    }

    //HAL_TIM_Base_Start(htim);

    uint32_t f_trigger_actual = timer_clk / ((psc + 1) * (arr + 1));
    return f_trigger_actual / N;
}

/**
 * @brief ???????????????????????????
 * @param params ????????
 * @param ch ???????? (0??1)
 * @retval uint32_t ?????????
 */
uint32_t StartWaveformChannel(ChannelParams_t *params, uint8_t ch)
{
    if (ch == 0)
    {
        return ConfigureWaveformChannel(params, &htim6, DAC_CHANNEL_1);
    }
    else
    {
        return ConfigureWaveformChannel(params, &htim7, DAC_CHANNEL_2);
    }
}

/**
 * @brief ??????????????
 * @param params ????????
 * @param ch ????????
 * @retval uint32_t ?????????
 */
uint32_t StartWaveform(ChannelParams_t *params, uint8_t ch)
{
    return StartWaveformChannel(params, ch);
}

// --------------------------
// ??????????????????
// --------------------------
/**
 * @brief ?????????????(???????????)
 * ??????????????????????????????????
 * @param src_buffer ??????????????
 * @param src_len ????????
 * @param ch ????????
 * @retval uint32_t ?????????
 */
uint32_t StartARBWaveform_Local(uint16_t *src_buffer, uint32_t src_len, uint8_t ch)
{
    if (src_buffer == NULL || src_len == 0)
        return 0;

    ChannelParams_t *params = &wave_params[ch];
    const uint32_t timer_clk = 84000000;  // ???????????
    const uint32_t max_trigger = 1000000; // DAC??????? 1MHz
    const uint32_t min_points = 8;        // ????????????

    // 1. ??????????????
    uint32_t target_points = max_trigger / params->freq;
    if (target_points < min_points)
        target_points = min_points;
    if (target_points > MAX_WAVEFORM_POINTS)
        target_points = MAX_WAVEFORM_POINTS;

    // 2. ?????????????????????????
    uint32_t actual_points;
    if (src_len > target_points)
    {
        // ????????????????????????
        actual_points = target_points;
        float ratio = (float)src_len / (float)target_points;
        for (uint32_t i = 0; i < actual_points; i++)
        {
            uint32_t idx = (uint32_t)((float)i * ratio);
            if (idx >= src_len)
                idx = src_len - 1;
            Waveform[ch][i] = src_buffer[idx];
        }
    }
    else
    {
        // ??????????????
        actual_points = src_len;
        memcpy(Waveform[ch], src_buffer, src_len * sizeof(uint16_t));
    }

    // 3. ???????????????
    uint32_t f_trigger = actual_points * params->freq;
    if (f_trigger > max_trigger)
        f_trigger = max_trigger;

    uint32_t div = timer_clk / f_trigger;
    if (div < 1)
        div = 1;

    uint32_t psc, arr;
    if (div <= 65536)
    {
        psc = 0;
        arr = div - 1;
    }
    else
    {
        psc = (uint32_t)(div / 65536.0f);
        if (psc > 65535)
            psc = 65535;
        arr = (div / (psc + 1)) - 1;
        if (arr > 65535)
            arr = 65535;
    }

    // 4. ??????????????
    TIM_HandleTypeDef *htim = (ch == 1) ? &htim6 : &htim7;
    uint32_t ch_dac = (ch == 0) ? DAC_CHANNEL_1 : DAC_CHANNEL_2;

    // ?????????????????????
    HAL_TIM_Base_Stop(htim);
    HAL_DAC_Stop_DMA(&hdac, ch_dac);
    HAL_DAC_Stop(&hdac, ch_dac);

    // ????????????
    __HAL_TIM_SET_PRESCALER(htim, psc);
    __HAL_TIM_SET_AUTORELOAD(htim, arr);

    // ????DAC DMA????
    if (HAL_DAC_Start_DMA(&hdac, ch_dac, (uint32_t *)Waveform[ch], actual_points, DAC_ALIGN_12B_R) != HAL_OK)
    {
        Error_Handler();
    }

    // ?????????????????
    HAL_TIM_Base_Start(htim);

    // ?????????????????
    uint32_t f_trigger_actual = timer_clk / ((psc + 1) * (arr + 1));
    return f_trigger_actual / actual_points;
}

/**
 * @brief ????????DAC?????????????????????
 * @param dac_data DAC????????
 * @param num_points ???????????
 * @param width ??????????
 * @param height ?????????
 * @param y_buffer ?????Y????????
 */
void GenerateARBWaveformForDisplay(uint16_t *dac_data,
                                   uint32_t num_points,
                                   uint32_t width,
                                   uint16_t height,
                                   int16_t *y_buffer)
{
    if (dac_data == NULL || y_buffer == NULL || num_points == 0 || height < 2)
        return;

    const float dac_max = 4095.0f; // 12??DAC????

    // ???????????????????
    for (uint32_t x = 0; x < width; x++)
    {
        // ??????????????????????????
        float map_ratio = (float)num_points / (float)width;
        uint32_t wave_idx = (uint32_t)((float)x * map_ratio);
        if (wave_idx >= num_points)
            wave_idx = num_points - 1;

        // DAC?????Y???????Y?????????????????????
        uint16_t dac_val = dac_data[wave_idx];
        float normalized_dac = (float)dac_val / dac_max;
        int16_t y = (int16_t)((float)(height - 1) * (1.0f - normalized_dac));

        // ??????
        if (y < 0)
            y = 0;
        if (y >= height)
            y = height - 1;

        y_buffer[x] = y;
    }
}

/**
 * @brief ???????????????????????????
 * @param params ????????
 * @param width ??????????
 * @param height ?????????
 * @param y_buffer ?????Y????????
 */   // 
void GenerateWaveformForDisplay(ChannelParams_t *params,
                                uint32_t width,
                                uint16_t height,
                                int16_t *y_buffer)
{
    if (params == NULL || y_buffer == NULL || width == 0 || height < 2)
        return;

    // 1. ?????????
    float amp_mv = (float)params->amp;  // 幅度
    float offset_mv = (float)params->offset;  // 偏置
    float duty_ratio = (float)params->duty / 100.0f;  // 占空比
    float phase_shift = (float)params->phase / 360.0f;  // 相位

    // ???????????
    if (duty_ratio < 0.001f)
        duty_ratio = 0.001f;
    if (duty_ratio > 0.999f)
        duty_ratio = 0.999f;

    // ????????????????
    float mv_to_pixel_scale = (float)height / ref_voltage_mv;

    // 2. ??????????????
    for (uint32_t x = 0; x < width; x++)
    {
        // ??????????????????????
        float t = (float)x / (float)width + phase_shift;
        if (t >= 1.0f)
            t -= 1.0f;
        if (t < 0.0f)
            t += 1.0f;

        // ???????????????????????????????????
        float norm_val = CalculateNormalizedValue(params->type, t, duty_ratio, true);

        // ?????????
        float voltage_mv;
        if (params->type == WAVE_DC)
        {
            voltage_mv = offset_mv;
        }
        else
        {
            voltage_mv = offset_mv + amp_mv * norm_val;
        }

        // ???????Y????
        int16_t y = (int16_t)((ref_voltage_mv - voltage_mv) * mv_to_pixel_scale);

        // ??????
        if (y < 0)
            y = 0;
        if (y >= height)
            y = height - 1;

        y_buffer[x] = y;
    }
}

// --------------------------
// ????????PWM????
// --------------------------
/**
 * @brief ????PWM????????????????
 * @param htim ???????????
 * @param frequency ??????PWM??? (Hz)
 * @param duty ???? (0-100)
 * @param Channel ??????
 * @param timer_clock ??????????? (Hz)
 * @return HAL_StatusTypeDef ??????
 */
HAL_StatusTypeDef PWM_SetFrequencyDuty(TIM_HandleTypeDef *htim, uint32_t frequency, uint8_t duty, uint32_t Channel, uint32_t timer_clock)
{
    // ???????
    if (htim == NULL || frequency == 0 || duty > 100 || timer_clock == 0)
    {
        return HAL_ERROR;
    }

    HAL_TIM_PWM_Stop(htim, Channel);

    // ?????????????????????????
    uint32_t prescaler = 0;
    uint32_t period = 0;
    for (prescaler = 0; prescaler <= 0xFFFF; prescaler++)
    {
        period = (timer_clock / (prescaler + 1) / frequency) - 1;
        if (period <= 0xFFFF && period >= 1)
        {
            break; // ???????????
        }
        if (prescaler == 0xFFFF)
        {
            // ??????????????????
            prescaler = 0xFFFF;
            period = (timer_clock / (0xFFFF + 1) / frequency) - 1;
            if (period < 1)
                period = 1;
            if (period > 0xFFFF)
                period = 0xFFFF;
            break;
        }
    }

    // ???????????
    uint32_t pulse = (uint32_t)(period * duty / 100);
    if (pulse < 1)
        pulse = 1;
    if (pulse > period)
        pulse = period;

    // ????????
    __HAL_TIM_SET_PRESCALER(htim, prescaler);
    __HAL_TIM_SET_AUTORELOAD(htim, period);
    __HAL_TIM_SET_COMPARE(htim, Channel, pulse);

    HAL_TIM_PWM_Start(htim, Channel);
    HAL_TIMEx_PWMN_Start(htim, Channel);
    return HAL_OK;
}

/**
 * @brief ???PWM???????????
 * @param freq ???
 * @param duty ????
 * @param ch ?????
 */
void SetFrequencyDuty(uint32_t freq, uint8_t duty, uint8_t ch)
{
    TIM_HandleTypeDef *htim = ch == 0 ? &htim12 : &htim8;
    uint32_t ch_tim = ch == 0 ? TIM_CHANNEL_1 : TIM_CHANNEL_3;
    uint32_t timer_clock = ch == 0 ? 84000000 : 168000000;

    if (ch)
    {
        duty = 100 - duty;
    }

    PWM_SetFrequencyDuty(htim, freq, duty, ch_tim, timer_clock);
}

// --------------------------
// ????????????????/??
// --------------------------
/**
 * @brief ??????????????????
 */
void wave_start(void)
{
    HAL_TIM_Base_Stop(&htim6);
    HAL_TIM_Base_Stop(&htim7);

    for (uint8_t i = 0; i < DAC_CH_MAX; i++)
    {
        if (wave_params[i].type >= WAVE_ARB1)
        {
            // ???????????????????????
            StartARBWaveform_Local(Recv_data_dac, USB_BUFFER_SIZE / 2, i);
            StartARBWaveform_Local(Waveform[i], USB_BUFFER_SIZE / 2, i);
        }
        else
        {
            // ????????????????????
            StartWaveform(&wave_params[i], i);
        }
    }

    HAL_TIM_Base_Start(&htim6);
    HAL_TIM_Base_Start(&htim7);
}

/**
 * @brief ?????????DAC???
 * @param ch ?????
 */
void DAC_EnableChannel(uint8_t ch)
{
    StartWaveform(&wave_params[ch], ch);
}

/**
 * @brief ?????????DAC???
 * @param ch ?????
 */
void DAC_DisableChannel(uint8_t ch)
{
    uint32_t ch_dac = ch == 0 ? DAC_CHANNEL_1 : DAC_CHANNEL_2;
    HAL_DAC_Stop_DMA(&hdac, ch_dac);
}

/**
 * @brief ?????????
 * @param ch ?????
 * @param en ?????
 */
void DAC_CH_en(uint8_t ch, uint8_t en)
{
    if (en)
    {
        DAC_EnableChannel(ch);
    }
    else
    {
        DAC_DisableChannel(ch);
    }
}

// --------------------------
// ??????????????
// --------------------------
/**
 * @brief ??????????
 * @param dir ???????
 * @param param ??????????
 */
void copy_start_end(uint8_t dir, uint8_t param)
{
    printf("dir=%d, param=%d\n", dir, param);
    if (dir)
    {
        scan_params[DAC_CH1].start_value = start_end[param][0];
        scan_params[DAC_CH1].end_value = start_end[param][1];
    }
    else
    {
        start_end[param][0] = scan_params[0].start_value;
        start_end[param][1] = scan_params[0].end_value;
    }
}

/**
 * @brief ???????????
 * @param ch ?????
 */
void Waveform_ScanInit(uint8_t ch)
{
    if (ch < DAC_CH_MAX)
    {
        scan_params[ch].enabled = 0;
        scan_params[ch].mode = SCAN_MODE_LINEAR;
        scan_params[ch].direction = SCAN_DIR_FORWARD;
        scan_params[ch].parameter = 0;
        scan_params[ch].start_value = 0;
        scan_params[ch].end_value = 0;
        scan_params[ch].duration_ms = 1000;
        scan_params[ch].step_count = 100;
        scan_params[ch].current_step = 0;
        scan_params[ch].last_update_time = HAL_GetTick();
        scan_params[ch].is_reverse = 0;
    }
}

/**
 * @brief ??????????
 * @param ch ?????
 * @param parameter ??????????
 * @param start ????
 * @param end ?????
 * @param duration ??????(ms)
 * @param mode ?????
 * @param direction ??????
 */
void Waveform_ScanSetParams(uint8_t ch, uint8_t parameter, uint32_t start,
                            uint32_t end, uint32_t duration, uint8_t mode, uint8_t direction)
{
    scan_params[ch].parameter = parameter;
    scan_params[ch].start_value = start;
    scan_params[ch].end_value = end;
    scan_params[ch].duration_ms = duration;
    scan_params[ch].mode = mode;
    scan_params[ch].direction = direction;

    // ?10ms??????????O??
    scan_params[ch].step_count = duration / 10;
    if (scan_params[ch].step_count < 1)
        scan_params[ch].step_count = 1;

    scan_params[ch].current_step = 0;
    scan_params[ch].last_update_time = HAL_GetTick();
    scan_params[ch].is_reverse = 0;
}

/**
 * @brief ???????
 * @param ch ?????
 */
void Waveform_ScanStart(uint8_t ch)
{
    if (ch < DAC_CH_MAX)
    {
        scan_params[ch].enabled = 1;
        scan_params[ch].current_step = 0;
        scan_params[ch].last_update_time = HAL_GetTick();
        scan_params[ch].is_reverse = 0;
    }
}

/**
 * @brief ?????
 * @param ch ?????
 */
void Waveform_ScanStop(uint8_t ch)
{
    if (ch < DAC_CH_MAX)
    {
        scan_params[ch].enabled = 0;
    }
}

/**
 * @brief ?????????
 * ??????????????????????
 */
void Waveform_ScanUpdate(void)
{
    uint32_t current_time = HAL_GetTick();

    for (uint8_t ch = 0; ch < DAC_CH_MAX; ch++)
    {
        if (!scan_params[ch].enabled)
            continue;

        // ?10ms????????????
        if (current_time - scan_params[ch].last_update_time < 10)
            continue;

        scan_params[ch].last_update_time = current_time;

        // ??????????????????????????
        ChannelParams_t temp_params;
        memcpy(&temp_params, &wave_params[ch], sizeof(ChannelParams_t));

        // ??????????
        float progress;
        if (scan_params[ch].is_reverse)
        {
            progress = 1.0f - (float)scan_params[ch].current_step / scan_params[ch].step_count;
        }
        else
        {
            progress = (float)scan_params[ch].current_step / scan_params[ch].step_count;
        }

        // ?????????
        uint32_t current_value;
        if (scan_params[ch].mode == SCAN_MODE_LINEAR)
        {
            // ???????
            current_value = scan_params[ch].start_value +
                            (scan_params[ch].end_value - scan_params[ch].start_value) * progress;
        }
        else
        {
            // ???????
            uint32_t start = (scan_params[ch].start_value > 0) ? scan_params[ch].start_value : 1;
            uint32_t end = (scan_params[ch].end_value > 0) ? scan_params[ch].end_value : 1;
            current_value = expf(logf(start) + (logf(end) - logf(start)) * progress);
        }
        scan_params[ch].current_value = current_value;

        // ???????????
        switch (scan_params[ch].parameter)
        {
        case SCAN_PARAM_FREQ:
            temp_params.freq = current_value;
            break;
        case SCAN_PARAM_AMP:
            temp_params.amp = current_value;
            break;
        case SCAN_PARAM_OFFSET:
            temp_params.offset = current_value;
            break;
        case SCAN_PARAM_DUTY:
            if (current_value > 100)
                current_value = 100;
            temp_params.duty = current_value;
            break;
        }

        // ???????????
        StartWaveform(&temp_params, ch);

        // ???????
        scan_params[ch].current_step++;

        // ?????????????
        if (scan_params[ch].current_step >= scan_params[ch].step_count)
        {
            switch (scan_params[ch].direction)
            {
            case SCAN_DIR_FORWARD:
            case SCAN_DIR_REVERSE:
                // ???????k????????
                scan_params[ch].enabled = 0;
                break;
            case SCAN_DIR_BIDIR:
                // ???????k????????
                scan_params[ch].is_reverse = !scan_params[ch].is_reverse;
                scan_params[ch].current_step = 0;
                break;
            }
        }
    }
}




