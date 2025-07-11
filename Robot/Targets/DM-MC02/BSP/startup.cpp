/**
 * @file    startup.cpp
 * @author  Morthine Xiang (xiang@morthine.com)
 * @brief   Startup file for DM-MC02 board.
 * @version 1.0
 * @date    2024-02-28
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "main.h"
#include "Configuration.hpp"

#include "FreeRTOS.h"
#include "task.h"

/* Import functions */
extern "C" {
  void SystemClock_Config();
  void PeriphCommonClock_Config(void);

  void MX_IWDG1_Init();
  void MX_GPIO_Init();
  void MX_DMA_Init();
  void MX_ADC1_Init();
  void MX_ADC3_Init();
  void MX_CRC_Init();
  void MX_TIM2_Init();
  void MX_TIM3_Init();
  void MX_TIM12_Init();
  void MX_SPI2_Init();
  void MX_SPI6_Init();
  void MX_OCTOSPI2_Init();
  void MX_UART5_Init();
  void MX_UART7_Init();
  void MX_USART1_UART_Init();
  void MX_USART2_UART_Init();
  void MX_USART3_UART_Init();
  void MX_USART10_UART_Init();
  void MX_FDCAN1_Init();
  void MX_FDCAN2_Init();
  void MX_FDCAN3_Init();
  void MX_USB_OTG_HS_PCD_Init(void);
  
}


/**
 * @brief  The main() function
 *
 * @return (int) None
 */
int main() {
  // __disable_irq();  // 禁用全局中断
  /* Enable CPU Cache */
  SCB_EnableICache();

  #ifdef rp_USE_DCACHE
  SCB_EnableDCache();
  #endif

  /* HAL Library Initialize */
  HAL_Init();

  /* System Clock Initialize */
  SystemClock_Config();
  PeriphCommonClock_Config();

  /* Peripheral Initialize */
  // MX_IWDG1_Init();
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_ADC3_Init();
  // MX_CRC_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM12_Init();
  MX_SPI2_Init();
  MX_SPI6_Init();
  // MX_OCTOSPI2_Init();
  MX_UART5_Init();
  MX_UART7_Init();
  MX_USART1_UART_Init();
  // MX_USART2_UART_Init();
  // MX_USART3_UART_Init();
  MX_USART10_UART_Init();
  MX_FDCAN1_Init();
  MX_FDCAN2_Init();
  MX_FDCAN3_Init();
  MX_USB_OTG_HS_PCD_Init();

  // __enable_irq();  // 所有外设初始化完成后重新启用全局中断

  /* User Application Entry Point */
  robotpilots::ApplicationEntryPoint();

  /* You Should NEVER Get Here! */
  Error_Handler();
  return -1;
}
