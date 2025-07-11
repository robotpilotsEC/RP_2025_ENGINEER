/**
 * @file    FreeRTOSConfig.c
 * @author  Morthine Xiang (xiang@morthine.com)
 * @brief
 * @version 1.0
 * @date    2023-12-16
 *
 * @copyright SZU-RobotPilots Copyright (c) 2023
 *
 */

#include "main.h"
#include "FreeRTOS.h"

/* Preallocate ucHeap */
#if (configAPPLICATION_ALLOCATED_HEAP == 1)
  // #error "Check address of ucHeap before use it! Then you can remove this line."

  /* Allocate the FreeRTOS Heap at CCM for performance optimization */
  volatile uint8_t ucHeap[configTOTAL_HEAP_SIZE];
#endif


/* Static allocation support */
#if (configSUPPORT_STATIC_ALLOCATION == 1)

  /* External Idle and Timer task static memory allocation functions */
  extern void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,  StackType_t **ppxIdleTaskStackBuffer,  uint32_t *pulIdleTaskStackSize);
  extern void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize);

  /*
    vApplicationGetIdleTaskMemory gets called when configSUPPORT_STATIC_ALLOCATION
    equals to 1 and is required for static memory allocation support.
  */
  void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) {
    /* Idle task control block and stack */
    static StaticTask_t Idle_TCB;
    static StackType_t  Idle_Stack[configMINIMAL_STACK_SIZE];

    *ppxIdleTaskTCBBuffer   = &Idle_TCB;
    *ppxIdleTaskStackBuffer = &Idle_Stack[0];
    *pulIdleTaskStackSize   = (uint32_t)configMINIMAL_STACK_SIZE;
  }

  /*
    vApplicationGetTimerTaskMemory gets called when configSUPPORT_STATIC_ALLOCATION
    equals to 1 and is required for static memory allocation support.
  */
  void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize) {
    /* Timer task control block and stack */
    static StaticTask_t Timer_TCB;
    static StackType_t  Timer_Stack[configTIMER_TASK_STACK_DEPTH];

    *ppxTimerTaskTCBBuffer   = &Timer_TCB;
    *ppxTimerTaskStackBuffer = &Timer_Stack[0];
    *pulTimerTaskStackSize   = (uint32_t)configTIMER_TASK_STACK_DEPTH;
  }

#endif // configSUPPORT_STATIC_ALLOCATION


/* Run time stats generate support */
#if (configGENERATE_RUN_TIME_STATS == 1)

  extern TIM_HandleTypeDef HAL_TIMEBASE_HANLDER;
  static uint64_t osRunTimeStartTick;

  void configureTimerForRunTimeStats(void) {
    osRunTimeStartTick = HAL_GetTick() *1000 + (HAL_TIMEBASE_HANLDER.Instance->CNT);
  }

  unsigned long getRunTimeCounterValue(void) {
    uint64_t currentTick = HAL_GetTick() *1000 + (HAL_TIMEBASE_HANLDER.Instance->CNT);
    return (unsigned long)(currentTick - osRunTimeStartTick);
  }

#endif // configGENERATE_RUN_TIME_STATS
