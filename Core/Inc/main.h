/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include <stdio.h>
#include <stdlib.h>

#include "cmsis_os.h"
#include "tim.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

// Í¨µÀÊý
enum
{
    DAC_CH1,
    DAC_CH2,
    DAC_CH_MAX,
};

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
uint64_t GetusTimeTicks(void);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define COL0_Pin GPIO_PIN_2
#define COL0_GPIO_Port GPIOE
#define TP_INT_Pin GPIO_PIN_3
#define TP_INT_GPIO_Port GPIOE
#define TP_INT_EXTI_IRQn EXTI3_IRQn
#define SPI_FLASH_CS_Pin GPIO_PIN_4
#define SPI_FLASH_CS_GPIO_Port GPIOE
#define ROW2_Pin GPIO_PIN_5
#define ROW2_GPIO_Port GPIOE
#define ROW3_Pin GPIO_PIN_6
#define ROW3_GPIO_Port GPIOE
#define KEY3_Pin GPIO_PIN_13
#define KEY3_GPIO_Port GPIOC
#define ROW1_Pin GPIO_PIN_1
#define ROW1_GPIO_Port GPIOC
#define WKUP_Pin GPIO_PIN_0
#define WKUP_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_2
#define LED_GPIO_Port GPIOB
#define TP_RST_Pin GPIO_PIN_12
#define TP_RST_GPIO_Port GPIOB
#define COL2_Pin GPIO_PIN_13
#define COL2_GPIO_Port GPIOB
#define EC11_A_Pin GPIO_PIN_12
#define EC11_A_GPIO_Port GPIOD
#define EC11_A_EXTI_IRQn EXTI15_10_IRQn
#define EC11_B_Pin GPIO_PIN_13
#define EC11_B_GPIO_Port GPIOD
#define COL1_Pin GPIO_PIN_6
#define COL1_GPIO_Port GPIOC
#define COL3_Pin GPIO_PIN_8
#define COL3_GPIO_Port GPIOA
#define SD_CARD_DET_Pin GPIO_PIN_3
#define SD_CARD_DET_GPIO_Port GPIOD
#define ROW0_Pin GPIO_PIN_6
#define ROW0_GPIO_Port GPIOD
#define LCD_CS_Pin GPIO_PIN_5
#define LCD_CS_GPIO_Port GPIOB
#define LCD_BLK_PWM_Pin GPIO_PIN_8
#define LCD_BLK_PWM_GPIO_Port GPIOB
#define LCD_DC_Pin GPIO_PIN_9
#define LCD_DC_GPIO_Port GPIOB
#define LCD_RST_Pin GPIO_PIN_1
#define LCD_RST_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */
#define SFUD_USING_EN

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
