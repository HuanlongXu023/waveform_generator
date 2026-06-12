#ifndef __UI_SYNC_H
#define __UI_SYNC_H

#include "main.h"

// 同步数据结构体
typedef struct
{
    uint8_t wave : 1;   // 波形同步标志
    uint8_t freq : 1;   // 频率同步标志
    uint8_t amp : 1;    // 幅度同步标志
    uint8_t offset : 1; // 偏置同步标志
    uint8_t duty : 1;   // 占空比同步标志
} SyncData_t;

// 外部变量声明
extern SyncData_t sync_data;

//// 同步菜单函数
//void sync_menu_set(uint8_t index, uint8_t en);
//void sync_menu(void);
//void sync_menu_set_refresh(uint8_t index);

//// 同步功能函数
//void sync_copy(void);
//void sync_mode_init(void);

#endif