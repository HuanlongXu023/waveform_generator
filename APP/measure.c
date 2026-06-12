#include "measure.h"

MeasureData_t MeasureData;

static uint8_t first_cycle_done = 0;
  
void Measure_Handler(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_0)
    {
        // 1. 先把时间戳读出来，越靠前越好，减少误差
        uint32_t current_time = GetusTimeTicks();
        GPIO_PinState pin_state = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_0);

        if (pin_state == GPIO_PIN_SET)
        {
            // ================= 上升沿处理 =================
            // 保存上一次上升沿时间
            MeasureData.LastHigh = MeasureData.High;
            // 更新当前上升沿时间
            MeasureData.High = current_time;

            // 计算负脉宽 (低电平时间) = 当前上升沿 - 上一次下降沿
            // 只有当上一次下降沿有效时才计算
            if (MeasureData.LowTime != 0)
            {
                MeasureData.Negative = MeasureData.High - MeasureData.LowTime;
            }
        }
        else
        {
            // ================= 下降沿处理 (主要计算在这里) =================
            // 保存上一次下降沿时间
            MeasureData.LastLow = MeasureData.LowTime;
            // 更新当前下降沿时间
            MeasureData.LowTime = current_time;

            // 1. 计算周期 (两个下降沿之间的时间)
            if (MeasureData.LastLow != 0)
            {
                MeasureData.Period = MeasureData.LowTime - MeasureData.LastLow;
            }

            // 2. 计算正脉宽 (高电平时间) = 当前下降沿 - 当前上升沿
            if (MeasureData.High != 0)
            {
                MeasureData.Positive = MeasureData.LowTime - MeasureData.High;
            }

            // 3. 计算占空比 (只有在周期和脉宽都有效，且过了初始阶段后)
            if (MeasureData.Period != 0 && MeasureData.Positive != 0 && first_cycle_done)
            {
                // 安全检查：脉宽必须小于周期，否则是毛刺，丢弃
                if (MeasureData.Positive < MeasureData.Period)
                {
                    // 【关键修复】强制转换为 uint64_t 防止乘法溢出
                    MeasureData.duty = (uint8_t)((uint64_t)MeasureData.Positive * 100 / MeasureData.Period);
                }
            }

            // 标记第一个周期已过，从下一个周期开始数据可信
            first_cycle_done = 1;

            static uint64_t HisCount, CurCount;
            uint64_t Count;
            HisCount = CurCount;
            CurCount = __HAL_TIM_GetCounter(&htim2);
            Count = CurCount - HisCount;
            MeasureData.Count += Count;
        }
    }
}