/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32wbxx_hal.h"

#include "stm32wbxx_nucleo.h"
#include <stdio.h>

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SYS_WKUP2_Pin GPIO_PIN_13
#define SYS_WKUP2_GPIO_Port GPIOC
#define RCC_OSC32_IN_Pin GPIO_PIN_14
#define RCC_OSC32_IN_GPIO_Port GPIOC
#define RCC_OSC32_OUT_Pin GPIO_PIN_15
#define RCC_OSC32_OUT_GPIO_Port GPIOC
#define Activate_Display_Pin GPIO_PIN_8
#define Activate_Display_GPIO_Port GPIOB
#define Data_Command_Pin GPIO_PIN_9
#define Data_Command_GPIO_Port GPIOB
#define Test_10ms_delay_Pin GPIO_PIN_3
#define Test_10ms_delay_GPIO_Port GPIOC
#define Rotary_Encoder_SCK_Pin GPIO_PIN_2
#define Rotary_Encoder_SCK_GPIO_Port GPIOA
#define Rotary_Encoder_SCK_EXTI_IRQn EXTI2_IRQn
#define Rotary_Encoder_DT_Pin GPIO_PIN_3
#define Rotary_Encoder_DT_GPIO_Port GPIOA
#define Rotary_Encoder_DT_EXTI_IRQn EXTI3_IRQn
#define Rotary_Encoder_SW_Pin GPIO_PIN_5
#define Rotary_Encoder_SW_GPIO_Port GPIOC
#define Rotary_Encoder_SW_EXTI_IRQn EXTI9_5_IRQn
#define ToF_interrupt_Pin GPIO_PIN_6
#define ToF_interrupt_GPIO_Port GPIOC
#define ToF_interrupt_EXTI_IRQn EXTI9_5_IRQn
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define JTMS_Pin GPIO_PIN_13
#define JTMS_GPIO_Port GPIOA
#define JTCK_Pin GPIO_PIN_14
#define JTCK_GPIO_Port GPIOA
#define JTDO_Pin GPIO_PIN_3
#define JTDO_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
