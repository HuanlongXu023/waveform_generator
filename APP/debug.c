#include "debug.h"
#include "key.h"
#include "lwrb.h"

#define RX_BUFFER_SIZE 16

uint32_t Rx_tick;

uint8_t RxDataBuff[RX_BUFFER_SIZE];

lwrb_t Serial_RxBuffer;

uint8_t RxData;
uint8_t Serial_RxFlag;
extern osSemaphoreId_t DebugRxBinarySem0Handle;
uint8_t key_val;
extern osMessageQueueId_t KeyVelQueueHandle;

const char *keyval_names[KEYVAL_MAX] = {
    "keyval_0",
    "keyval_1",
    "keyval_2",
    "keyval_3",
    "keyval_4",
    "keyval_sine",
    "keyval_count",
    "keyval_ch1",
    "keyval_rect",
    "keyval_sweep",
    "keyval_ok",
    "keyval_left",
    "keyval_any",
    "keyval_sys",
    "keyval_ch2",
    "keyval_right",
    "keyval_add",
    "keyval_sub",

};

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart == &huart2)
  {
    Rx_tick = HAL_GetTick();
    lwrb_write(&Serial_RxBuffer, &RxData, 1);
    HAL_UART_Receive_IT(&huart2, &RxData, 1);
  }
}

uint8_t Serial_GetRxFlag(void)
{
  if (Serial_RxFlag == 1)
  {
    Serial_RxFlag = 0;
    return 1;
  }
  return 0;
}

uint8_t Serial_GetRxData(void)
{
  return RxData;
}

void debug_task(void)
{
  uint16_t len = lwrb_get_full(&Serial_RxBuffer);
  if (len > 0 && (HAL_GetTick() - Rx_tick) > 10)
  {
    char data[RX_BUFFER_SIZE];
    memset(data, 0, sizeof(data));
    lwrb_read(&Serial_RxBuffer, data, len);
    key_val = atoi(data);
    printf("key_val = %d, %s\r\n", key_val, keyval_names[key_val]);
    // osMessageQueuePut(KeyVelQueueHandle, &key_val, 0, 0);
  }
  else
  {
    osDelay(10);
  }
}

void debug_init(void)
{
  HAL_UART_Receive_IT(&huart2, &RxData, 1); //
  lwrb_init(&Serial_RxBuffer, RxDataBuff, sizeof(RxDataBuff));
}
