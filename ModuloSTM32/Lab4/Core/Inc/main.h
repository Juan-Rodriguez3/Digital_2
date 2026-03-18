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
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define B1_EXTI_IRQn EXTI15_10_IRQn
#define LED0_Pin GPIO_PIN_0
#define LED0_GPIO_Port GPIOC
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOC
#define LED2_Pin GPIO_PIN_2
#define LED2_GPIO_Port GPIOC
#define LED3_Pin GPIO_PIN_3
#define LED3_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define LED4_Pin GPIO_PIN_4
#define LED4_GPIO_Port GPIOC
#define LED5_Pin GPIO_PIN_5
#define LED5_GPIO_Port GPIOC
#define G_Pin GPIO_PIN_0
#define G_GPIO_Port GPIOB
#define F_Pin GPIO_PIN_1
#define F_GPIO_Port GPIOB
#define E_Pin GPIO_PIN_2
#define E_GPIO_Port GPIOB
#define LED6_Pin GPIO_PIN_6
#define LED6_GPIO_Port GPIOC
#define LED7_Pin GPIO_PIN_7
#define LED7_GPIO_Port GPIOC
#define BTN_START_Pin GPIO_PIN_8
#define BTN_START_GPIO_Port GPIOC
#define BTN_START_EXTI_IRQn EXTI9_5_IRQn
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define BTN2_Pin GPIO_PIN_10
#define BTN2_GPIO_Port GPIOC
#define BTN2_EXTI_IRQn EXTI15_10_IRQn
#define BTN1_Pin GPIO_PIN_12
#define BTN1_GPIO_Port GPIOC
#define BTN1_EXTI_IRQn EXTI15_10_IRQn
#define SWO_pt_Pin GPIO_PIN_3
#define SWO_pt_GPIO_Port GPIOB
#define D_Pin GPIO_PIN_4
#define D_GPIO_Port GPIOB
#define C_Pin GPIO_PIN_5
#define C_GPIO_Port GPIOB
#define A_Pin GPIO_PIN_6
#define A_GPIO_Port GPIOB
#define B_Pin GPIO_PIN_7
#define B_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
