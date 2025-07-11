/**
 * @file proc_chassis.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 底盘任务
 * @version 1.0
 * @date 2024-11-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "mod_chassis.hpp"

namespace my_engineer {

/**
 * @brief 创建底盘任务
 * 
 * @param argument 
 */
void CModChassis::StartChassisModuleTask(void *argument) {

    // 要求参数为CModChassis类的实例，如果传入为空则删除任务并返回
    if (argument == nullptr) proc_return();

    // 类型转换
    auto &chassis = *static_cast<CModChassis *>(argument);

    // 任务循环
    while (true) {
        // FSM
        switch (chassis.Module_FSMFlag_) {
            
            case FSM_RESET: {

                chassis.chassisInfo.isModuleAvailable = false;
                chassis.comWheelset_.StopComponent();

                proc_waitMs(20);
                continue; // 跳过下面的代码，直接进入下一次循环
            }

            case FSM_INIT: {

                chassis.comWheelset_.StartComponent();

                chassis.chassisCmd = SChassisCmd();
                chassis.chassisInfo.isModuleAvailable = true;
                chassis.Module_FSMFlag_ = FSM_CTRL;
                chassis.moduleStatus = APP_OK;

                continue;
            }

            case FSM_CTRL: {

                // 限制控制量
                chassis.RestrictChassisCommand_();

                // 更新底盘控制量
                // 乘以一个较大值方便调参
                chassis.comWheelset_.wheelsetCmd.speed_X = chassis.chassisCmd.speed_X * 80;
                chassis.comWheelset_.wheelsetCmd.speed_Y = chassis.chassisCmd.speed_Y * 80;
                chassis.comWheelset_.wheelsetCmd.speed_W = chassis.chassisCmd.speed_W * 40;

                
                proc_waitMs(1); // 1000Hz
                continue;
            }

            default: { chassis.StopModule(); }

        }
    }

    // 任务退出
    chassis.moduleTaskHandle = nullptr;
    proc_return();
}

}   // namespace my_engineer
