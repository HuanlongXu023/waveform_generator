#include "EC11.h"

#include "key.h"

int16_t Encoder_Count;
 
extern osMessageQueueId_t KeyVelQueueHandle;
 
void EC11_RotateHandler(uint16_t GPIO_Pin)
{
    static uint8_t A_cnt = 0;
    static uint8_t B_Value = 0;

    if (GPIO_Pin == EC11_A_Pin)
    {
        if ((HAL_GPIO_ReadPin(EC11_A_GPIO_Port, EC11_A_Pin) == GPIO_PIN_RESET) && (A_cnt == 0)) //
        {
            A_cnt++;
            B_Value = 0;
            if (HAL_GPIO_ReadPin(EC11_B_GPIO_Port, EC11_B_Pin) == GPIO_PIN_SET)
            {
                B_Value = 1;
            }
        }
        else if ((HAL_GPIO_ReadPin(EC11_A_GPIO_Port, EC11_A_Pin) == GPIO_PIN_SET) && (A_cnt == 1)) //
        {
            A_cnt = 0;
            A_cnt = 0;
            if ((B_Value == 1) && (HAL_GPIO_ReadPin(EC11_B_GPIO_Port, EC11_B_Pin) == GPIO_PIN_RESET))
            {
                Encoder_Count++;
            }
            if ((B_Value == 0) && (HAL_GPIO_ReadPin(EC11_B_GPIO_Port, EC11_B_Pin) == GPIO_PIN_SET))
            {
                Encoder_Count--;
            }
        }
    }
}
// 
/**
 * @brief  处理编码器计数值的变化
 * @note   建议在主循环 while(1) 中持续调用此函数
 */
void EC11_ProcessCountChange(void)
{
    static int16_t Last_Encoder_Count = 0;
    uint8_t key_val = 0;
    // 检测计数值是否发生变化
    if (Encoder_Count != Last_Encoder_Count)
    {
        // 判断旋转方向
        if (Encoder_Count > Last_Encoder_Count)
        {
            // 顺时针旋转
            // printf("+\r\n");
            key_val = KEYVAL_EC11_ADD;
        }
        else
        {
            // 逆时针旋转
            // printf("-\r\n");
            key_val = KEYVAL_EC11_SUB;
        }
        osMessageQueuePut(KeyVelQueueHandle, &key_val, 0, 0);
        // 更新记录值，等待下一次变化
        Last_Encoder_Count = Encoder_Count;
    }
}







