/**
 * @file conf_process.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 完成sys_task.cpp中所有任务的创建
 * @version 1.0
 * @date 2024-11-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "conf_process.hpp"

namespace my_engineer {

// 定义任务句柄
TaskHandle_t MonitorTaskHandle;
TaskHandle_t UpdateTaskHandle;
TaskHandle_t HeartbeatTaskHandle;
TaskHandle_t SystemUpdateTaskHandle;

void StartUpdateTask(void* argument);
void StartHeartbeatTask(void* argument);
void StartMonitorTask(void* argument);
void StartSystemUpdateTask(void* argument);

void InitProcess() {
    
  /* Create Update Task */
  xTaskCreate(StartUpdateTask, "Update Task",
              4096, nullptr, proc_UpdateTaskPriority,
              &UpdateTaskHandle);

  /* Create Heartbeat Task */
  xTaskCreate(StartHeartbeatTask, "Heartbeat Task",
              1024, nullptr, proc_HeartbeatTaskPriority,
              &HeartbeatTaskHandle);

  /* Create System Update Task */
  xTaskCreate(StartSystemUpdateTask, "System Update Task",
              1024, nullptr, proc_SystemTaskPriority,
              &SystemUpdateTaskHandle);
}

}   // namespace my_engineer
