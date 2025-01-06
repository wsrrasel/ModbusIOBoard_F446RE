/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DI_0_Pin GPIO_PIN_0
#define DI_0_GPIO_Port GPIOA
#define DI_1_Pin GPIO_PIN_1
#define DI_1_GPIO_Port GPIOA
#define DBUG_TX_U2_Pin GPIO_PIN_2
#define DBUG_TX_U2_GPIO_Port GPIOA
#define DBUG_RX_U2_Pin GPIO_PIN_3
#define DBUG_RX_U2_GPIO_Port GPIOA
#define DI_2_Pin GPIO_PIN_4
#define DI_2_GPIO_Port GPIOA
#define SA_3_Pin GPIO_PIN_2
#define SA_3_GPIO_Port GPIOB
#define RS485_RX_LED_Pin GPIO_PIN_14
#define RS485_RX_LED_GPIO_Port GPIOB
#define RS485_TX_LED_Pin GPIO_PIN_15
#define RS485_TX_LED_GPIO_Port GPIOB
#define RS485_TX_U6_Pin GPIO_PIN_6
#define RS485_TX_U6_GPIO_Port GPIOC
#define RS485_RX_U6_Pin GPIO_PIN_7
#define RS485_RX_U6_GPIO_Port GPIOC
#define DO_0_Pin GPIO_PIN_8
#define DO_0_GPIO_Port GPIOA
#define DO_1_Pin GPIO_PIN_9
#define DO_1_GPIO_Port GPIOA
#define DO_2_Pin GPIO_PIN_10
#define DO_2_GPIO_Port GPIOA
#define DO_3_Pin GPIO_PIN_11
#define DO_3_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SA_2_Pin GPIO_PIN_15
#define SA_2_GPIO_Port GPIOA
#define SW_0_Pin GPIO_PIN_10
#define SW_0_GPIO_Port GPIOC
#define PWR_LED_Pin GPIO_PIN_11
#define PWR_LED_GPIO_Port GPIOC
#define STATUS_LED_Pin GPIO_PIN_12
#define STATUS_LED_GPIO_Port GPIOC
#define RS485_DIR_Pin GPIO_PIN_2
#define RS485_DIR_GPIO_Port GPIOD
#define SA_1_Pin GPIO_PIN_3
#define SA_1_GPIO_Port GPIOB
#define DI_3_Pin GPIO_PIN_8
#define DI_3_GPIO_Port GPIOB
#define SA_0_Pin GPIO_PIN_9
#define SA_0_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
