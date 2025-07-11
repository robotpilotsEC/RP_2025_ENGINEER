/**
 * @file sys_task.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 在这里进行所有层任务的汇总
 * @version 1.0
 * @date 2024-11-10
 * 
 * @copyright Copyright (c) 2024
 * @details
 * 当然，一些层中内置的任务不会在这里进行汇总，比如BMI088的初始化任务，模块的控制任务等
 */

#include "Core.hpp"
#include "conf_CanTxNode.hpp"

extern IWDG_HandleTypeDef hiwdg1;

namespace my_engineer {

/**
 * @brief 监测任务
 * 
 */
void StartMonitorTask(void *argument) {

    // auto *uart = reinterpret_cast<CInfUART *>(InterfaceIDMap.at(EInterfaceID::INF_UART10));
    // while (true) {

    //     uart->FormatTransmit("Hahahaha\r\n");

    //     proc_waitMs(500);
    // }
    auto *usb = reinterpret_cast<CInfUSB_CDC *>(InterfaceIDMap.at(EInterfaceID::INF_USB_CDC));
    auto *controller = reinterpret_cast<CModController *>(ModuleIDMap.at(EModuleID::MOD_CONTROLLER));
    while (true) {
        usb->FormatTransmit(
                "---------------------------------------------\r\n"
                "REAL_X: %d , REAL_Y: %d\r\n"
                "Rocker_X: %d , Rocker_Y: %d\r\n"
                "Rocker_Key: %d\r\n"
                "posit_stretch: %.2f , posit_traverse: %.2f\r\n"
                "angle_yaw: %.2f , speed_roll: %d\r\n"
                "controlled_by_controller: %d\r\n",
                controller->get_rocker_x(),
                controller->get_rocker_y(),
                SysControllerLink.controllerInfo.Rocker_X,
                SysControllerLink.controllerInfo.Rocker_Y,
                static_cast<int8_t>(SysControllerLink.controllerInfo.Rocker_Key),
                SysControllerLink.controllerInfo.posit_stretch,
                SysControllerLink.controllerInfo.posit_traverse,
                SysControllerLink.controllerInfo.angle_yaw,
                SysControllerLink.controllerInfo.speed_roll,
                static_cast<int8_t>(SysControllerLink.robotInfo.controlled_by_controller)
            );
        proc_waitMs(500);
    }
}

/**
 * @brief 系统层更新任务
 * @note 与其它层独立开来是因为频率不需要那么高
 */
void StartSystemUpdateTask(void *argument) {

    while (true) {

        for (const auto &item : SystemIDMap) {
            item.second->UpdateHandler_();
        }

        proc_waitMs(40); // 25Hz
    }
}


/**
 * @brief 其余层的更新任务以及系统核心更新任务
 * 
 */
void StartUpdateTask(void *argument) {
    
    // 初始化系统核心
    SystemCore.InitSystemCore();

    while (true) {
        
        // 更新所有设备
        for (const auto &item : DeviceIDMap) {
            item.second->UpdateHandler_();
        }

        // 更新系统核心
        SystemCore.UpdateHandler_();

        // SystemIDMap.at(ESystemID::SYS_BOARDLINK)->UpdateHandler_();

        // 更新所有模块
        for (const auto &item : ModuleIDMap) {
            item.second->UpdateHandler_();
        }

        // 执行can发送
        TxNode_Can1_280.Transmit();
        TxNode_Can2_280.Transmit();
        TxNode_Can1_200.Transmit();
        TxNode_Can2_200.Transmit();
        // TxNode_Can2_1FF.Transmit();

        proc_waitMs(1); // 1000Hz

    }
}

/**
 * @brief 心跳任务以及看门狗
 * 
 */
void StartHeartbeatTask(void *argument) {

    while (true) {

        // 通信接口心跳
        for (const auto &item : InterfaceIDMap) {
            item.second->HeartbeatHandler_();
        }

        // 设备心跳
        for (const auto &item : DeviceIDMap) {
            item.second->HeartbeatHandler_();
        }

        // 系统心跳
        for (const auto &item : SystemIDMap) {
            item.second->HeartbeatHandler_();
        }

        // 系统心跳
        // for (const auto &item : SystemIDMap) {
        //     item.second->HeartbeatHandler_();
        // }

        // 心跳和更新都把核心放在Module前面是为什么？
        // 更新系统核心心跳
        SystemCore.HeartbeatHandler_();


        // 模块心跳
        for (const auto &item : ModuleIDMap) {
            item.second->HeartbeatHandler_();
        }

        // 喂狗
        HAL_IWDG_Refresh(&hiwdg1);

        proc_waitMs(10); // 100Hz
    }
}

}   // namespace my_engineer
