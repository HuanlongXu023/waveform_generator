#include "buzzer.h"
#include "tim.h"

static uint32_t g_stop_time = 0; //
static uint8_t g_is_running = 0; //

void BUZZER_ON(void)
{
    HAL_TIM_PWM_Start(&htim13, TIM_CHANNEL_1); // 启动PWM输出
}

void BUZZER_OFF(void)
{
    HAL_TIM_PWM_Stop(&htim13, TIM_CHANNEL_1); // 停止PWM输出
}

void Buzzer_Start(uint32_t ms)
{
    BUZZER_ON();
    g_stop_time = HAL_GetTick() + ms; //
    g_is_running = 1;
}

void Buzzer_Process(void)
{
    if (g_is_running && (HAL_GetTick() >= g_stop_time))
    {
        BUZZER_OFF();
        g_is_running = 0;
    }
}
