#ifndef NUMERIC_CONTROLLER_H
#define NUMERIC_CONTROLLER_H

#include <stdint.h>

/**
 * @brief 简化版数值控制器结构体
 */
typedef struct
{
    void *value_ptr;        // 数值指针，指向实际存储的位置
    uint8_t value_size;     // 数值大小（字节）
    uint8_t selected_digit; // 当前选中的数字位（0-7，从左到右）
    uint8_t max_digits;     // 最大位数（包括可能的前导零）
    uint32_t min_value;     // 最小值限制
    uint32_t max_value;     // 最大值限制
} NumericController_t;

/**
 * @brief 初始化数值控制器
 */
void initController(NumericController_t *controller, void *value_ptr, uint8_t value_size,
                    uint32_t min_value, uint32_t max_value, uint8_t max_digits);

/**
 * @brief 获取控制器当前的数值
 */
uint32_t getControllerValue(NumericController_t *controller);

/**
 * @brief 设置控制器的数值（会自动限制在min/max范围内）
 */
void setControllerValue(NumericController_t *controller, uint32_t value);

/**
 * @brief 增加当前选中位的数字（0-9循环）
 */
void incrementDigit(NumericController_t *controller);

/**
 * @brief 减少当前选中位的数字（0-9循环）
 */
void decrementDigit(NumericController_t *controller);

/**
 * @brief 选择下一个数字位（向右移动）
 */
void selectNextDigit(NumericController_t *controller);

/**
 * @brief 选择上一个数字位（向左移动）
 */
void selectPrevDigit(NumericController_t *controller);

/**
 * @brief 计算数字位数 (辅助工具)
 */
uint8_t getDigitCount(uint32_t value);

/**
 * @brief 获取指定位置的数字 (辅助工具，用于显示)
 */
uint8_t getDigitAtPosition(uint32_t value, uint8_t position, uint8_t max_digits);

#endif // NUMERIC_CONTROLLER_H
