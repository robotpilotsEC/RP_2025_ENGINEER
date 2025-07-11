/**
 * @file Core.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 系统核心源文件
 * @version 1.0
 * @date 2024-11-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "Core.hpp"

namespace my_engineer {

// 实例化系统核心
CSystemCore SystemCore;

/**
 * @brief 初始化系统核心
 * 
 * @return EAppStatus 
 */
EAppStatus CSystemCore::InitSystemCore() {
    
    // 启动通信接口发送
    for (const auto &item : InterfaceIDMap) {
        item.second->StartTransfer();
    }

    // 启动BMI088设备
    DeviceIDMap.at(EDeviceID::DEV_MEMS_BMI088)->StartDevice();

    // 初始化所有系统
    CSystemControllerLink::SSystemInitParam_ControllerLink controllerLinkInitParam;
    controllerLinkInitParam.systemID = ESystemID::SYS_CONTROLLERLINK;
    controllerLinkInitParam.controllerLinkDevID = EDeviceID::DEV_CONTROLLER_LINK;
    SysControllerLink.InitSystem(&controllerLinkInitParam);


    // 获取模块的指针
    pcontroller_ = reinterpret_cast<CModController *>(ModuleIDMap.at(EModuleID::MOD_CONTROLLER));

    proc_waitMs(1000); // 等待系统初始化完成

    coreStatus = APP_OK;

    return APP_OK;
}

/**
 * @brief 更新处理
 * 
 */
void CSystemCore::UpdateHandler_() {
    
    if(!pcontroller_->ControllerInfo.isModuleAvailable 
        && pcontroller_->moduleStatus == APP_OK) {
        pcontroller_->StartModule();
    }

    SysControllerLink.controllerInfo.controller_OK = pcontroller_->ControllerInfo.isModuleAvailable;
    SysControllerLink.controllerInfo.return_success = pcontroller_->ControllerInfo.isReturnSuccess;
    SysControllerLink.controllerInfo.Rocker_X = pcontroller_->ControllerInfo.rocker_X;
    SysControllerLink.controllerInfo.Rocker_Y = pcontroller_->ControllerInfo.rocker_Y;
    SysControllerLink.controllerInfo.Rocker_Key = static_cast<CSystemControllerLink::KEY_STATUS>(pcontroller_->ControllerInfo.rocker_Key);
    SysControllerLink.controllerInfo.angle_yaw = pcontroller_->ControllerInfo.angle_yaw;
    SysControllerLink.controllerInfo.posit_stretch = pcontroller_->ControllerInfo.posit_stretch;
    SysControllerLink.controllerInfo.posit_traverse = pcontroller_->ControllerInfo.posit_traverse;
    SysControllerLink.controllerInfo.speed_roll = pcontroller_->ControllerInfo.speed_roll;

    if (SysControllerLink.robotInfo.ask_reset_flag) {
        // 复位整个系统
        RESET_SYSTEM();
    }
    pcontroller_->ControllerCmd.StartControl = SysControllerLink.robotInfo.controlled_by_controller;
    pcontroller_->ControllerCmd.cmd_stretch = SysControllerLink.robotInfo.posit_stretch;
    pcontroller_->ControllerCmd.cmd_yaw = SysControllerLink.robotInfo.angle_yaw;
    pcontroller_->ControllerCmd.cmd_traverse = SysControllerLink.robotInfo.posit_traverse;

}

/**
 * @brief 心跳处理
 * 
 */
void CSystemCore::HeartbeatHandler_() {
    
}

// 复位
void CSystemCore::RESET_SYSTEM() {

    
    __set_FAULTMASK(1);
    NVIC_SystemReset();
}

}   // namespace my_engineer
