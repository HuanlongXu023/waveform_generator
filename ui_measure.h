#ifndef __UI_MEASURE_H
#define __UI_MEASURE_H

#include "main.h"

typedef struct
{
    uint8_t mode;  // 模式 0:频率 1:计数
    uint8_t pause; // 暂停 0:不暂停 1:暂停
    uint8_t gate;  // 闸门设置 1s 10s 100s
} MeasureSetting_t;

extern MeasureSetting_t measure_setting;

#endif
