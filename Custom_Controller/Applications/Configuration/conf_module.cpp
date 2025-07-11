/**
 * @file conf_module.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 完成所有模块的配置
 * @version 1.0
 * @date 2024-11-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "conf_module.hpp"
#include "Module.hpp"

extern TIM_HandleTypeDef htim2;

namespace my_engineer {

EAppStatus InitAllModule() {

    /******初始化自定义控制器模块******/
    CModController::SModInitParam_Controller controllerInitParam;
    controllerInitParam.moduleID = EModuleID::MOD_CONTROLLER;
    controllerInitParam.rocker_id = EDeviceID::DEV_ROCKER;
    controllerInitParam.buzzer_id = EDeviceID::DEV_BUZZER;
    controllerInitParam.traverse_id = EDeviceID::DEV_CONTROLLER_MTR_TRAVERSE;
    controllerInitParam.stretch_id = EDeviceID::DEV_CONTROLLER_MTR_STRETCH;
    controllerInitParam.yaw_id = EDeviceID::DEV_CONTROLLER_MTR_YAW;
    controllerInitParam.roll_id = EDeviceID::DEV_CONTROLLER_MTR_ROLL;
    // 设置can发送节点
    controllerInitParam.traverseTxNode = &TxNode_Can2_200;
    controllerInitParam.stretchTxNode = &TxNode_Can2_200;
    controllerInitParam.yawTxNode = &TxNode_Can2_200;
    controllerInitParam.rollTxNode = &TxNode_Can2_200;
    // 设置PID参数
    controllerInitParam.traversePosPidParam.kp = 0.1f;
    controllerInitParam.traversePosPidParam.ki = 0.0f;
    controllerInitParam.traversePosPidParam.kd = 0.1f;
    controllerInitParam.traversePosPidParam.maxIntegral = 20;
    controllerInitParam.traversePosPidParam.maxOutput = 5000;
    controllerInitParam.traverseSpdPidParam.kp = 2.0f;
    controllerInitParam.traverseSpdPidParam.ki = 5.0f;
    controllerInitParam.traverseSpdPidParam.kd = 0.0f;
    controllerInitParam.traverseSpdPidParam.maxIntegral = 200;
    controllerInitParam.traverseSpdPidParam.maxOutput = 5000;
    controllerInitParam.stretchPosPidParam.kp = 0.1f;
    controllerInitParam.stretchPosPidParam.ki = 0.0f;
    controllerInitParam.stretchPosPidParam.kd = 0.1f;
    controllerInitParam.stretchPosPidParam.maxIntegral = 20;
    controllerInitParam.stretchPosPidParam.maxOutput = 5000;
    controllerInitParam.stretchSpdPidParam.kp = 2.0f;
    controllerInitParam.stretchSpdPidParam.ki = 5.0f;
    controllerInitParam.stretchSpdPidParam.kd = 0.0f;
    controllerInitParam.stretchSpdPidParam.maxIntegral = 200;
    controllerInitParam.stretchSpdPidParam.maxOutput = 5000;
    controllerInitParam.rollPosPidParam.kp = 0.1f;
    controllerInitParam.rollPosPidParam.ki = 0.0f;
    controllerInitParam.rollPosPidParam.kd = 0.1f;
    controllerInitParam.rollPosPidParam.maxIntegral = 20;
    controllerInitParam.rollPosPidParam.maxOutput = 5000;
    controllerInitParam.rollSpdPidParam.kp = 2.0f;
    controllerInitParam.rollSpdPidParam.ki = 5.0f;
    controllerInitParam.rollSpdPidParam.kd = 0.0f;
    controllerInitParam.rollSpdPidParam.maxIntegral = 200;
    controllerInitParam.rollSpdPidParam.maxOutput = 5000;
    controllerInitParam.yawPosPidParam.kp = 0.1f;
    controllerInitParam.yawPosPidParam.ki = 0.0f;
    controllerInitParam.yawPosPidParam.kd = 0.1f;
    controllerInitParam.yawPosPidParam.maxIntegral = 20;
    controllerInitParam.yawPosPidParam.maxOutput = 5000;
    controllerInitParam.yawSpdPidParam.kp = 2.0f;
    controllerInitParam.yawSpdPidParam.ki = 100.0f;
    controllerInitParam.yawSpdPidParam.kd = 0.0f;
    controllerInitParam.yawSpdPidParam.maxIntegral = 50;
    controllerInitParam.yawSpdPidParam.maxOutput = 8000;
    // 使用初始化后的参数创建 controllerModule 实例
    static auto controllerModule = CModController(controllerInitParam);

    return APP_OK;
}

} // namespace my_engineer
