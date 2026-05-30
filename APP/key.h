#ifndef __KEY_H
#define __KEY_H

#include "main.h"

enum
{
    KEYVAL_0,
    KEYVAL_1,
    KEYVAL_2,
    KEYVAL_3,
    KEYVAL_4,
    KEYVAL_SINE,
    KEYVAL_SWEEP, // 扫描 6
    KEYVAL_CH1,

    KEYVAL_LEFT, // 左移

    KEYVAL_RECT,  // 矩形波
    KEYVAL_COUNT, // 计数
    KEYVAL_OK,
    KEYVAL_RIGHT, // 右移

    KEYVAL_ANY, // 任意波形
    KEYVAL_SYS, //
    KEYVAL_CH2, // 通道2

    KEYVAL_EC11_ADD, // ec11_加
    KEYVAL_EC11_SUB, // ec11_减
    KEYVAL_MAX,

};

#endif
