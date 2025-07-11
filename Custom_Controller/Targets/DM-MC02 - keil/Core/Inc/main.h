/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32h7xx_hal.h"

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
#define PWR_OUT2_EN_Pin GPIO_PIN_13
#define PWR_OUT2_EN_GPIO_Port GPIOC
#define PWR_OUT1_EN_Pin GPIO_PIN_14
#define PWR_OUT1_EN_GPIO_Port GPIOC
#define PWR_5V_EN_Pin GPIO_PIN_15
#define PWR_5V_EN_GPIO_Port GPIOC
#define BMI_ACC_CS_Pin GPIO_PIN_0
#define BMI_ACC_CS_GPIO_Port GPIOC
#define BMI_MOSI_Pin GPIO_PIN_1
#define BMI_MOSI_GPIO_Port GPIOC
#define BMI_MISO_Pin GPIO_PIN_2
#define BMI_MISO_GPIO_Port GPIOC
#define BMI_GYRO_CS_Pin GPIO_PIN_3
#define BMI_GYRO_CS_GPIO_Port GPIOC
#define ADC1_rocker_Y_Pin GPIO_PIN_0
#define ADC1_rocker_Y_GPIO_Port GPIOA
#define ADC1_rocker_X_Pin GPIO_PIN_2
#define ADC1_rocker_X_GPIO_Port GPIOA
#define RGB_SCK_Pin GPIO_PIN_5
#define RGB_SCK_GPIO_Port GPIOA
#define RGB_MOSI_Pin GPIO_PIN_7
#define RGB_MOSI_GPIO_Port GPIOA
#define ADC1_VBAT_Pin GPIO_PIN_4
#define ADC1_VBAT_GPIO_Port GPIOC
#define BMI_TEMP_PWM_Pin GPIO_PIN_1
#define BMI_TEMP_PWM_GPIO_Port GPIOB
#define BMI_ACCEL_INT_Pin GPIO_PIN_10
#define BMI_ACCEL_INT_GPIO_Port GPIOE
#define BMI_ACCEL_INT_EXTI_IRQn EXTI15_10_IRQn
#define BMI_GYRO_INT_Pin GPIO_PIN_12
#define BMI_GYRO_INT_GPIO_Port GPIOE
#define BMI_GYRO_INT_EXTI_IRQn EXTI15_10_IRQn
#define rocker_KEY_Pin GPIO_PIN_13
#define rocker_KEY_GPIO_Port GPIOE
#define BMI_SCK_Pin GPIO_PIN_13
#define BMI_SCK_GPIO_Port GPIOB
#define BUZZER_Pin GPIO_PIN_15
#define BUZZER_GPIO_Port GPIOB
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define KEY_INT_Pin GPIO_PIN_15
#define KEY_INT_GPIO_Port GPIOA
#define DBUS_RX_Pin GPIO_PIN_2
#define DBUS_RX_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
