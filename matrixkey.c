#include "matrixkey.h"

// 行
GPIO_TypeDef *port_row[4] = {ROW0_GPIO_Port, ROW1_GPIO_Port, ROW2_GPIO_Port, ROW3_GPIO_Port};
uint32_t gpio_row[4] = {ROW0_Pin, ROW1_Pin, ROW2_Pin, ROW3_Pin};

// 列
GPIO_TypeDef *port_col[4] = {COL0_GPIO_Port, COL1_GPIO_Port, COL2_GPIO_Port, COL3_GPIO_Port};
uint32_t gpio_col[4] = {COL0_Pin, COL1_Pin, COL2_Pin, COL3_Pin};

// uint8_t key_scan(void)
// {
//   uint8_t i, j, key_val = 0;

//   for (i = 0; i < 4; i++)
//   {
//     // 选中第i列（低电平有效）
//     HAL_GPIO_WritePin(port_col[i], gpio_col[i], GPIO_PIN_RESET);
//     // 检测行的状态，确定按键是否被按下
//     for (j = 0; j < 4; j++)
//     {
//       // 如果该行被拉低
//       if (HAL_GPIO_ReadPin(port_row[j], gpio_row[j]) == GPIO_PIN_RESET)
//       {
//         // 计算按键编号
//         key_val = i * 4 + j + 1;

//         break;
//       }
//     }

//     // 恢复第i列的输出状态
//     HAL_GPIO_WritePin(port_col[i], gpio_col[i], GPIO_PIN_SET);

//     // 如果按键被按下，则直接退出扫描
//     if (key_val)
//       break;
//   }
//   // 返回按键编号，0表示无按键被按下
//   return key_val;
// }

// 按键状态机定义
typedef enum
{
    KEY_STATE_IDLE,        // 0. 空闲状态，等待按键按下
    KEY_STATE_DEBOUNCE,    // 1. 消抖状态
    KEY_STATE_PRESSED,     // 2. 按键确认按下
    KEY_STATE_WAIT_RELEASE // 3. 等待按键释放
} KeyState_t;

int8_t key_scan(void)
{
    static KeyState_t current_state = KEY_STATE_IDLE;
    static uint32_t tick_start = 0;
    static uint8_t cached_key = 0;
    const uint32_t debounce_time = 20; // 消抖时间 20ms

    uint8_t i, j;
    uint8_t key_val = 0;
    uint8_t is_key_detected = 0; // 0: 未检测到, 1: 检测到

    // 1. 扫描硬件：逐列拉低，检测行
    for (i = 0; i < 4; i++)
    {
        HAL_GPIO_WritePin(port_col[i], gpio_col[i], GPIO_PIN_RESET); // 拉低当前列

        for (j = 0; j < 4; j++)
        {
            if (HAL_GPIO_ReadPin(port_row[j], gpio_row[j]) == GPIO_PIN_RESET)
            {
                key_val = i * 4 + j; // 计算键值 (1-16)

                is_key_detected = 1; // 标记检测到按键
                break;               // 找到按键，跳出行循环
            }
        }

        HAL_GPIO_WritePin(port_col[i], gpio_col[i], GPIO_PIN_SET); // 恢复当前列

        if (is_key_detected == 1)
            break; // 找到按键，跳出列循环
    }

    // 2. 状态机逻辑处理
    switch (current_state)
    {
        case KEY_STATE_IDLE:
        {
            if (is_key_detected == 1)
            {
                cached_key = key_val;       // 缓存当前键值
                tick_start = HAL_GetTick(); // 记录时间戳
                current_state = KEY_STATE_DEBOUNCE;
            }
            break;
        }

        case KEY_STATE_DEBOUNCE:
        {
            // 等待消抖时间过去
            if ((HAL_GetTick() - tick_start) > debounce_time)
            {
                // 再次检测按键是否仍然按下
                if ((is_key_detected == 1) && (key_val == cached_key))
                {
                    current_state = KEY_STATE_PRESSED;
                }
                else
                {
                    // 抖动或误触，回到空闲
                    current_state = KEY_STATE_IDLE;
                }
            }
            break;
        }

        case KEY_STATE_PRESSED:
        {
            // 在此状态返回一次键值，然后立即进入等待释放状态
            current_state = KEY_STATE_WAIT_RELEASE;
            return cached_key; // 核心：只在这里返回有效键值
        }

        case KEY_STATE_WAIT_RELEASE:
        {
            // 等待按键释放（所有行都检测不到低电平）
            if (is_key_detected == 0)
            {
                current_state = KEY_STATE_IDLE;
            }
            break;
        }
    }

    return -1; // 无按键事件或未到触发时机时返回 -1
}

extern osMessageQueueId_t KeyVelQueueHandle;


void matrixkey_task(void)
{
	int8_t key_val = key_scan();
	if (key_val != -1)
	{
		        osMessageQueuePut(KeyVelQueueHandle, &key_val, 0, 0);

	}
}
