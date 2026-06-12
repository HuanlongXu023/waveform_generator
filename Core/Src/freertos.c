/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "debug.h"
#include "lcd.h"
#include "key.h"
//#include "test_ui.h"
#include "waveform_generator.h"
#include "save_load.h"
#include "host_device.h"
//#include "buzzer.h"
//#include "matrixkey.h"
#include "measure.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticQueue_t osStaticMessageQDef_t;
typedef StaticSemaphore_t osStaticMutexDef_t;
typedef StaticSemaphore_t osStaticSemaphoreDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for btu_Task */
osThreadId_t btu_TaskHandle;
uint32_t btu_TaskBuffer[128];
osStaticThreadDef_t btu_TaskControlBlock;
const osThreadAttr_t btu_Task_attributes = {
    .name = "btu_Task",
    .cb_mem = &btu_TaskControlBlock,
    .cb_size = sizeof(btu_TaskControlBlock),
    .stack_mem = &btu_TaskBuffer[0],
    .stack_size = sizeof(btu_TaskBuffer),
    .priority = (osPriority_t)osPriorityAboveNormal,
};
/* Definitions for UI_Task */
osThreadId_t UI_TaskHandle;
uint32_t UI_TaskBuffer[1024];
osStaticThreadDef_t UI_TaskControlBlock;
const osThreadAttr_t UI_Task_attributes = {
    .name = "UI_Task",
    .cb_mem = &UI_TaskControlBlock,
    .cb_size = sizeof(UI_TaskControlBlock),
    .stack_mem = &UI_TaskBuffer[0],
    .stack_size = sizeof(UI_TaskBuffer),
    .priority = (osPriority_t)osPriorityRealtime,
};
/* Definitions for Debug_Task */
osThreadId_t Debug_TaskHandle;
uint32_t Debug_TaskBuffer[128];
osStaticThreadDef_t Debug_TaskControlBlock;
const osThreadAttr_t Debug_Task_attributes = {
    .name = "Debug_Task",
    .cb_mem = &Debug_TaskControlBlock,
    .cb_size = sizeof(Debug_TaskControlBlock),
    .stack_mem = &Debug_TaskBuffer[0],
    .stack_size = sizeof(Debug_TaskBuffer),
    .priority = (osPriority_t)osPriorityLow,
};
/* Definitions for host_deviceTask */
osThreadId_t host_deviceTaskHandle;
uint32_t MeasureTask05Buffer[1024];
osStaticThreadDef_t MeasureTask05ControlBlock;
const osThreadAttr_t host_deviceTask_attributes = {
    .name = "host_deviceTask",
    .cb_mem = &MeasureTask05ControlBlock,
    .cb_size = sizeof(MeasureTask05ControlBlock),
    .stack_mem = &MeasureTask05Buffer[0],
    .stack_size = sizeof(MeasureTask05Buffer),
    .priority = (osPriority_t)osPriorityBelowNormal,
};
/* Definitions for FreqTask */
osThreadId_t FreqTaskHandle;
uint32_t FreqTaskBuffer[128];
osStaticThreadDef_t FreqTaskControlBlock;
const osThreadAttr_t FreqTask_attributes = {
    .name = "FreqTask",
    .cb_mem = &FreqTaskControlBlock,
    .cb_size = sizeof(FreqTaskControlBlock),
    .stack_mem = &FreqTaskBuffer[0],
    .stack_size = sizeof(FreqTaskBuffer),
    .priority = (osPriority_t)osPriorityRealtime7,
};
/* Definitions for KeyVelQueue */
osMessageQueueId_t KeyVelQueueHandle;
uint8_t KeyVelQueueBuffer[16 * sizeof(uint8_t)];
osStaticMessageQDef_t KeyVelQueueControlBlock;
const osMessageQueueAttr_t KeyVelQueue_attributes = {
    .name = "KeyVelQueue",
    .cb_mem = &KeyVelQueueControlBlock,
    .cb_size = sizeof(KeyVelQueueControlBlock),
    .mq_mem = &KeyVelQueueBuffer,
    .mq_size = sizeof(KeyVelQueueBuffer)};
/* Definitions for LCDMutex */
osMutexId_t LCDMutexHandle;
osStaticMutexDef_t LCDMutexControlBlock;
const osMutexAttr_t LCDMutex_attributes = {
    .name = "LCDMutex",
    .cb_mem = &LCDMutexControlBlock,
    .cb_size = sizeof(LCDMutexControlBlock),
};
/* Definitions for DebugRxBinarySem0 */
osSemaphoreId_t DebugRxBinarySem0Handle;
osStaticSemaphoreDef_t DebugRxBinarySem01ControlBlock;
const osSemaphoreAttr_t DebugRxBinarySem0_attributes = {
    .name = "DebugRxBinarySem0",
    .cb_mem = &DebugRxBinarySem01ControlBlock,
    .cb_size = sizeof(DebugRxBinarySem01ControlBlock),
};
/* Definitions for MeasureSem */
osSemaphoreId_t MeasureSemHandle;
osStaticSemaphoreDef_t MeasureSemControlBlock;
const osSemaphoreAttr_t MeasureSem_attributes = {
    .name = "MeasureSem",
    .cb_mem = &MeasureSemControlBlock,
    .cb_size = sizeof(MeasureSemControlBlock),
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

// uint32_t g_measured_freq = 0; // 全局频率变量
// uint8_t g_gate_time = 1;      // 闸门时间，单位：秒，默认1秒

void TIM_Freq_Get(void)
{
  static uint64_t HisCount, CurCount;
  uint64_t Count;
  HisCount = CurCount;
  CurCount = __HAL_TIM_GetCounter(&htim2);
  Count = CurCount - HisCount;
  // MeasureData.Count += Count;
  // g_measured_freq = Count / g_gate_time;
  // MeasureData.Frequency = Count / g_gate_time;
  MeasureData.Frequency = Count / get_measure_gate();

  // printf("g_measured_freq = %d\r\n", g_measured_freq);
  // printf("MeasureData.Frequency = %d\r\n", MeasureData.Frequency);
}

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void btuTask(void *argument);
void UITask(void *argument);
void DebugTask(void *argument);
void host_device_Task(void *argument);
void Freq_Task(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of LCDMutex */
  LCDMutexHandle = osMutexNew(&LCDMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of DebugRxBinarySem0 */
  DebugRxBinarySem0Handle = osSemaphoreNew(1, 1, &DebugRxBinarySem0_attributes);

  /* creation of MeasureSem */
  MeasureSemHandle = osSemaphoreNew(1, 1, &MeasureSem_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of KeyVelQueue */
  KeyVelQueueHandle = osMessageQueueNew(16, sizeof(uint8_t), &KeyVelQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of btu_Task */
  btu_TaskHandle = osThreadNew(btuTask, NULL, &btu_Task_attributes);

  /* creation of UI_Task */
  UI_TaskHandle = osThreadNew(UITask, NULL, &UI_Task_attributes);

  /* creation of Debug_Task */
  Debug_TaskHandle = osThreadNew(DebugTask, NULL, &Debug_Task_attributes);

  /* creation of host_deviceTask */
  host_deviceTaskHandle = osThreadNew(host_device_Task, NULL, &host_deviceTask_attributes);

  /* creation of FreqTask */
  FreqTaskHandle = osThreadNew(Freq_Task, NULL, &FreqTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */

  // HAL_TIM_Base_Start(&htim2);
  // TickType_t xLastWakeTime;
  // xLastWakeTime = xTaskGetTickCount();
  // HAL_TIM_Base_Start_IT(&htim11);
  // osDelay(1000);

  /* Infinite loop */
  for (;;)
  {
    osDelay(1000);
    // TIM_Freq_Get();
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

    //    char myStr[64] = {0};                             // 定义一个数组，用于存放要输出的字符串
    //    sprintf(myStr, "<<<<<<APP\r\r");                  // 格式化字符串
    //    CDC_Transmit_FS((uint8_t *)myStr, strlen(myStr)); // 发送字符串

    // refresh_measure_data();
    // vTaskDelayUntil(&xLastWakeTime, get_measure_gate() * 1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_btuTask */
/**
 * @brief Function implementing the btu_Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_btuTask */
void btuTask(void *argument)
{
  /* USER CODE BEGIN btuTask */
  /* Infinite loop */
  for (;;)
  {
    Waveform_ScanUpdate();
		Refresh_Start_Value();
    matrixkey_task();
    Buzzer_Process();
    osDelay(5);
  }
  /* USER CODE END btuTask */
}

/* USER CODE BEGIN Header_UITask */
/**
 * @brief Function implementing the UI_Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_UITask */
void UITask(void *argument)
{
  /* USER CODE BEGIN UITask */

  ui_init();
  /* Infinite loop */
  for (;;)
  {
    ui_task();
    // refresh_measure_data();
    osDelay(1);
  }
  /* USER CODE END UITask */
}

/* USER CODE BEGIN Header_DebugTask */
/**
 * @brief Function implementing the Debug_Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_DebugTask */
void DebugTask(void *argument)
{
  /* USER CODE BEGIN DebugTask */
#define HARDWARE_VERSION "V1.0.0"
#define SOFTWARE_VERSION "V0.1.0"

  debug_init();
  cm_backtrace_init("project", HARDWARE_VERSION, SOFTWARE_VERSION);
  /* Infinite loop */
  for (;;)
  {

    debug_task();
    osDelay(1);
  }
  /* USER CODE END DebugTask */
}
 
/* USER CODE BEGIN Header_host_device_Task */
/**
 * @brief Function implementing the host_deviceTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_host_device_Task */
void host_device_Task(void *argument)
{
  /* USER CODE BEGIN host_device_Task */
  host_device_task_init();
  /* Infinite loop */
  for (;;)
  {
    // host_device_task_init();
    host_task();
    osDelay(1);
  }
  /* USER CODE END host_device_Task */
}

/* USER CODE BEGIN Header_Freq_Task */
/**
 * @brief Function implementing the FreqTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_Freq_Task */
void Freq_Task(void *argument)
{
  /* USER CODE BEGIN Freq_Task */
  osDelay(1000);  // 先等1s,让系统各模块初始化完毕

  HAL_TIM_Base_Start(&htim2);
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  HAL_TIM_Base_Start_IT(&htim11); // 和频率测量无关，测量脉宽和占空比
  // HAL_TIM_Base_Start_IT(&htim9);

  /* Infinite loop */
  for (;;)
  {
    TIM_Freq_Get();
    // HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    // refresh_measure_data();
    vTaskDelayUntil(&xLastWakeTime, get_measure_gate() * 1000);  // 让出cpu,进入阻塞态

    // osDelay(1);
  }
  /* USER CODE END Freq_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
