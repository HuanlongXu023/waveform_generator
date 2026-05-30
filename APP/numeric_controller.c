#include "numeric_controller.h"
#include "usart.h"

// 计算数字位数
uint8_t getDigitCount(uint32_t value)
{
    if (value >= 10000000)
        return 8;
    if (value >= 1000000)
        return 7;
    if (value >= 100000)
        return 6;
    if (value >= 10000)
        return 5;
    if (value >= 1000)
        return 4;
    if (value >= 100)
        return 3;
    if (value >= 10)
        return 2;
    return 1;
}
 
// 获取指定位置的数字
uint8_t getDigitAtPosition(uint32_t value, uint8_t position, uint8_t max_digits)
{
    uint32_t divisor = 1;
    uint8_t actual_digits = getDigitCount(value);

    // 计算有效位置（处理前导零显示）
    int16_t effective_position = position - (max_digits - actual_digits);

    // 如果是前导零或者超出范围，返回0
    if (effective_position < 0 || effective_position >= actual_digits)
    {
        return 0;
    }

    // 计算除数
    for (uint8_t i = 0; i < actual_digits - effective_position - 1; i++)
    {
        divisor *= 10;
    }

    return (value / divisor) % 10;
}

// 内部静态函数：设置指定位置的数字（不对外暴露）
//static void setDigitAtPositionInternal(uint32_t *value, uint8_t position, uint8_t digit, uint8_t max_digits)
//{
//    uint32_t divisor = 1;
//    // uint8_t actual_digits = getDigitCount(*value); // 这里不需要实际位数，直接按最大位数算

//    // 计算除数
//    for (uint8_t i = 0; i < max_digits - position - 1; i++)
//    {
//        divisor *= 10;
//    }

//    // 简单的加减法来置位
//    // 注意：为了简化逻辑，这里不做复杂的位数擦除，直接通过 setControllerValue 来保证范围
//    *value += ((int32_t)digit - (int32_t)getDigitAtPosition(*value, position, max_digits)) * divisor;
//}

// 获取控制器当前值
uint32_t getControllerValue(NumericController_t *controller)
{
    switch (controller->value_size)
    {
    case 1:
        return *(uint8_t *)controller->value_ptr;
    case 2:
        return *(uint16_t *)controller->value_ptr;
    case 4:
        return *(uint32_t *)controller->value_ptr;
    default:
        return 0;
    }
}

// 设置控制器值
void setControllerValue(NumericController_t *controller, uint32_t value)
{
    // 限制范围
    if (value < controller->min_value)
        value = controller->min_value;
    else if (value > controller->max_value)
        value = controller->max_value;

    // 根据类型写入
    switch (controller->value_size)
    {
    case 1:
        *(uint8_t *)controller->value_ptr = (uint8_t)value;
        break;
    case 2:
        *(uint16_t *)controller->value_ptr = (uint16_t)value;
        break;
    case 4:
        *(uint32_t *)controller->value_ptr = (uint32_t)value;
        break;
    }
}

// 初始化控制器
void initController(NumericController_t *controller, void *value_ptr, uint8_t value_size,
                    uint32_t min_value, uint32_t max_value, uint8_t max_digits)
{
    controller->value_ptr = value_ptr;
    controller->value_size = value_size;
    controller->selected_digit = 0;
    controller->max_digits = max_digits;
    controller->min_value = min_value;
    controller->max_value = max_value;

    // 确保初始值在合法范围内
    uint32_t current_value = getControllerValue(controller);
    setControllerValue(controller, current_value);

    // 移动selected_digit到第一个非0的数字
    if (current_value > 0)
    {
        for (uint8_t i = 0; i < max_digits; i++)
        {
            uint8_t digit = getDigitAtPosition(current_value, i, max_digits);
            printf("i: %d digit: %d\n", i, digit);

            if (digit != 0)
            {
                printf("selected_digit: %d\n", i);
                controller->selected_digit = i;
                break;
            }
        }
    }
}
 
// 增加选中的数字
// void incrementDigit(NumericController_t *controller)
// {
//     uint32_t value = getControllerValue(controller);
//     uint8_t digit = getDigitAtPosition(value, controller->selected_digit, controller->max_digits);

//     digit = (digit + 1) % 10;

//     // 使用内部函数修改
//     setDigitAtPositionInternal(&value, controller->selected_digit, digit, controller->max_digits);

//     // 写入并限制范围
//     setControllerValue(controller, value);
// }

void incrementDigit(NumericController_t *controller)
{
    uint32_t value = getControllerValue(controller);
    uint8_t position = controller->selected_digit;
    uint8_t max_digits = controller->max_digits;
    // 检查是否已经是最大值
    if (value >= controller->max_value)
    {
        return; // 已经是最大值，不再增加
    }
    // 计算当前位的权重
    uint32_t divisor = 1;
    for (uint8_t i = 0; i < max_digits - position - 1; i++)
    {
        divisor *= 10;
    }

    // 当前位加1
    value += divisor;

    // 写入并限制范围
    setControllerValue(controller, value);
}

// 减少选中的数字
// void decrementDigit(NumericController_t *controller)
// {
//     uint32_t value = getControllerValue(controller);
//     uint8_t digit = getDigitAtPosition(value, controller->selected_digit, controller->max_digits);

//     digit = (digit + 9) % 10; // 等价于 -1

//     setDigitAtPositionInternal(&value, controller->selected_digit, digit, controller->max_digits);
//     setControllerValue(controller, value);
// }

void decrementDigit(NumericController_t *controller)
{
    uint32_t value = getControllerValue(controller);
    uint8_t position = controller->selected_digit;
    uint8_t max_digits = controller->max_digits;
    // 检查是否已经是最小值
    if (value <= controller->min_value)
    {
        return; // 已经是最小值，不再减少
    }
    // 计算当前位的权重
    uint32_t divisor = 1;
    for (uint8_t i = 0; i < max_digits - position - 1; i++)
    {
        divisor *= 10;
    }

    // 当前位减1
    value -= divisor;

    // 写入并限制范围
    setControllerValue(controller, value);
}

// 选择下一个数字位
void selectNextDigit(NumericController_t *controller)
{
    if (controller->selected_digit < controller->max_digits - 1)
        controller->selected_digit++;
    else
        controller->selected_digit = 0;
}

// 选择上一个数字位
void selectPrevDigit(NumericController_t *controller)
{
    if (controller->selected_digit > 0)
        controller->selected_digit--;
    else
        controller->selected_digit = controller->max_digits - 1;
}
