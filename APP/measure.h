#ifndef __MEASURE
#define __MEASURE

#include "main.h"

//void buttons_init(void);


typedef struct
{
    // 高电平时间
    uint64_t High;
    // 上次高电平时间
    uint64_t LastHigh;
    // 低电平时间
    uint64_t LowTime;
    // 上次低电平时间
    uint64_t LastLow;
    // 周期时间
    uint64_t Period;
    // 正脉宽时间
    uint64_t Positive;
    // 负脉宽时间
    uint64_t Negative;
    // 脉空比
    uint8_t duty;
    // 频率
    uint32_t Frequency;
    // 计数
    uint32_t Count;

} MeasureData_t;

extern MeasureData_t MeasureData;

#endif
