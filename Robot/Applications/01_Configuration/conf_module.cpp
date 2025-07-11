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

namespace my_engineer {

EAppStatus InitAllModule() {

    /******初始化机械臂模块******/
    CModArm::SModInitParam_Arm armInitParam;
    armInitParam.moduleID = EModuleID::MOD_ARM;
    armInitParam.MotorID_Yaw = EDeviceID::DEV_ARM_MTR_YAW;
    armInitParam.MotorID_Pitch1 = EDeviceID::DEV_ARM_MTR_PITCH1;
    armInitParam.MotorID_Pitch2 = EDeviceID::DEV_ARM_MTR_PITCH2;
    armInitParam.MotorID_Roll = EDeviceID::DEV_ARM_MTR_ROLL;
    armInitParam.MotorID_End_L = EDeviceID::DEV_ARM_MTR_END_L;
    armInitParam.MotorID_End_R = EDeviceID::DEV_ARM_MTR_END_R;
    // 设置can发送节点
    armInitParam.MotorTxNode_Pitch1 = &TxNode_Can1_200;
    armInitParam.MotorTxNode_Pitch2 = &TxNode_Can1_200;
    armInitParam.MotorTxNode_Roll = &TxNode_Can1_200;
    armInitParam.MotorTxNode_End_L = &TxNode_Can1_200;
    armInitParam.MotorTxNode_End_R = &TxNode_Can1_200;
    // 初始化 mitCtrl_Yaw 的成员
    armInitParam.MIT_Yaw_kp = 20.0f;
    armInitParam.MIT_Yaw_kd = 1.0f;
    // 初始化 Pitch1PosPidParam 的成员
    armInitParam.Pitch1PosPidParam.kp = 0.15f;
    armInitParam.Pitch1PosPidParam.ki = 0.0f;
    armInitParam.Pitch1PosPidParam.kd = 0.2f;
    armInitParam.Pitch1PosPidParam.maxIntegral = 0.0f;
    armInitParam.Pitch1PosPidParam.maxOutput = 5000.0f;
    // 初始化 Pitch1SpdPidParam 的成员
    armInitParam.Pitch1SpdPidParam.kp = 1.5f;
    armInitParam.Pitch1SpdPidParam.ki = 0.5f;
    armInitParam.Pitch1SpdPidParam.kd = 0.0f;
    armInitParam.Pitch1SpdPidParam.maxIntegral = 6000.0f;
    armInitParam.Pitch1SpdPidParam.maxOutput = 8000.0f;
    // 初始化 Pitch2PosPidParam 的成员
    armInitParam.Pitch2PosPidParam.kp = 0.15f;
    armInitParam.Pitch2PosPidParam.ki = 0.0f;
    armInitParam.Pitch2PosPidParam.kd = 0.2f;
    armInitParam.Pitch2PosPidParam.maxIntegral = 0.0f;
    armInitParam.Pitch2PosPidParam.maxOutput = 5000.0f;
    // 初始化 Pitch2SpdPidParam 的成员
    armInitParam.Pitch2SpdPidParam.kp = 1.5f;
    armInitParam.Pitch2SpdPidParam.ki = 0.5f;
    armInitParam.Pitch2SpdPidParam.kd = 0.0f;
    armInitParam.Pitch2SpdPidParam.maxIntegral = 6000.0f;
    armInitParam.Pitch2SpdPidParam.maxOutput = 8000.0f;
    // 初始化 RollPosPidParam 的成员
    armInitParam.RollPosPidParam.kp = 0.15f;
    armInitParam.RollPosPidParam.ki = 0.0f;
    armInitParam.RollPosPidParam.kd = 0.2f;
    armInitParam.RollPosPidParam.maxIntegral = 0.0f;
    armInitParam.RollPosPidParam.maxOutput = 5000.0f;
    // 初始化 RollSpdPidParam 的成员
    armInitParam.RollSpdPidParam.kp = 1.5f;
    armInitParam.RollSpdPidParam.ki = 0.5f;
    armInitParam.RollSpdPidParam.kd = 0.0f;
    armInitParam.RollSpdPidParam.maxIntegral = 6000.0f;
    armInitParam.RollSpdPidParam.maxOutput = 8000.0f;
    // 初始化 endPosPidParam 的成员
    armInitParam.endPosPidParam.kp = 0.2f;
    armInitParam.endPosPidParam.ki = 0.0f;
    armInitParam.endPosPidParam.kd = 0.5f;
    armInitParam.endPosPidParam.maxOutput = 4000.0f;
    // 初始化 endSpdPidParam 的成员
    armInitParam.endSpdPidParam.kp = 1.5f;
    armInitParam.endSpdPidParam.ki = 0.5f;
    armInitParam.endSpdPidParam.kd = 0.0f;
    armInitParam.endSpdPidParam.maxIntegral = 6000.0f;
    armInitParam.endSpdPidParam.maxOutput = 5000.0f;
    // 使用初始化后的参数创建 armModule 实例
    static auto armModule = CModArm(armInitParam);


    /******初始化子龙门模块******/
    CModSubGantry::SModInitParam_SubGantry subGantryInitParam;
    subGantryInitParam.moduleID = EModuleID::MOD_SUBGANTRY;
    subGantryInitParam.liftMotorID_L = EDeviceID::DEV_SUBGANTRY_MTR_LIFT_L;
    subGantryInitParam.liftMotorID_R = EDeviceID::DEV_SUBGANTRY_MTR_LIFT_R;
    subGantryInitParam.stretchMotorID_L = EDeviceID::DEV_SUBGANTRY_MTR_STRETCH_L;
    subGantryInitParam.stretchMotorID_R = EDeviceID::DEV_SUBGANTRY_MTR_STRETCH_R;
    subGantryInitParam.liftMotorTxNode_L = &TxNode_Can2_200;
    subGantryInitParam.liftMotorTxNode_R = &TxNode_Can1_200;
    subGantryInitParam.stretchMotorTxNode_L = &TxNode_Can2_200;
    subGantryInitParam.stretchMotorTxNode_R = &TxNode_Can1_200;
    subGantryInitParam.LeftPumpPort = LeftPump_GPIO_Port;
    subGantryInitParam.LeftPumpPin = LeftPump_Pin;
    subGantryInitParam.RightPumpPort = RightPump_GPIO_Port;
    subGantryInitParam.RightPumpPin = RightPump_Pin;
    subGantryInitParam.ArmPumpPort = ArmPump_GPIO_Port;
    subGantryInitParam.ArmPumpPin = ArmPump_Pin;
    // 设置PID参数
    subGantryInitParam.liftPosPidParam.kp = 0.1f;
    subGantryInitParam.liftPosPidParam.ki = 0.0f;
    subGantryInitParam.liftPosPidParam.kd = 0.1f;
    subGantryInitParam.liftPosPidParam.maxIntegral = 0.0f;
    subGantryInitParam.liftPosPidParam.maxOutput = 5000.0f;
    subGantryInitParam.liftSpdPidParam.kp = 1.0f;
    subGantryInitParam.liftSpdPidParam.ki = 0.5f;
    subGantryInitParam.liftSpdPidParam.kd = 0.0f;
    subGantryInitParam.liftSpdPidParam.maxIntegral = 5000.0f;
    subGantryInitParam.liftSpdPidParam.maxOutput = 8000.f;//8000.0f;
    subGantryInitParam.stretchPosPidParam.kp = 0.1f;
    subGantryInitParam.stretchPosPidParam.ki = 0.0f;
    subGantryInitParam.stretchPosPidParam.kd = 0.1f;
    subGantryInitParam.stretchPosPidParam.maxIntegral = 0.0f;
    subGantryInitParam.stretchPosPidParam.maxOutput = 5000.0f;
    subGantryInitParam.stretchSpdPidParam.kp = 1.2f;
    subGantryInitParam.stretchSpdPidParam.ki = 0.5f;
    subGantryInitParam.stretchSpdPidParam.kd = 0.0f;
    subGantryInitParam.stretchSpdPidParam.maxIntegral = 5000.0f;
    subGantryInitParam.stretchSpdPidParam.maxOutput = 8000.0f;
    // 使用初始化后的参数创建 subGantryModule 实例
    static auto subGantryModule = CModSubGantry(subGantryInitParam);

    /******初始化云台模块******/
    CModGimbal::SModInitParam_Gimbal gimbalInitParam;
    gimbalInitParam.moduleID = EModuleID::MOD_GIMBAL;
    gimbalInitParam.liftMotorID = EDeviceID::DEV_GIMBAL_MTR_LIFT;
    // 设置can发送节点
    gimbalInitParam.liftMotorTxNode = &TxNode_Can2_1FF;
    // 初始化 liftPosPidParam 的成员
    gimbalInitParam.liftPosPidParam.kp = 0.3f;
    gimbalInitParam.liftPosPidParam.ki = 0.0f;
    gimbalInitParam.liftPosPidParam.kd = 0.3f;
    gimbalInitParam.liftPosPidParam.maxOutput = 8000.0f;
    // 初始化 liftSpdPidParam 的成员
    gimbalInitParam.liftSpdPidParam.kp = 1.0f;
    gimbalInitParam.liftSpdPidParam.ki = 2.0f;
    gimbalInitParam.liftSpdPidParam.kd = 0.0f;
    gimbalInitParam.liftSpdPidParam.maxIntegral = 1000.0f;
    gimbalInitParam.liftSpdPidParam.maxOutput = 6000.0f;
    // 使用初始化后的参数创建 gimbalModule 实例
    static auto gimbalModule = CModGimbal(gimbalInitParam);

    /******初始化底盘模块******/
    CModChassis::SModInitParam_Chassis chassisInitParam;
    chassisInitParam.moduleID = EModuleID::MOD_CHASSIS;
    chassisInitParam.memsDevID = EDeviceID::DEV_MEMS_BMI088;
    chassisInitParam.wheelsetMotorID_LF = EDeviceID::DEV_CHAS_MTR_LF;
    chassisInitParam.wheelsetMotorID_RF = EDeviceID::DEV_CHAS_MTR_RF;
    chassisInitParam.wheelsetMotorID_LB = EDeviceID::DEV_CHAS_MTR_LB;
    chassisInitParam.wheelsetMotorID_RB = EDeviceID::DEV_CHAS_MTR_RB;
    // 设置can发送节点
    chassisInitParam.wheelsetMotorTxNode_LF = &TxNode_Can2_200;
    chassisInitParam.wheelsetMotorTxNode_RF = &TxNode_Can2_200;
    chassisInitParam.wheelsetMotorTxNode_LB = &TxNode_Can2_200;
    chassisInitParam.wheelsetMotorTxNode_RB = &TxNode_Can2_200;
    // 设置PID参数
    chassisInitParam.yawCorrectionPidParam.kp = 10.0f;
    chassisInitParam.yawCorrectionPidParam.ki = 20.0f;
    chassisInitParam.yawCorrectionPidParam.kd = 0.0f;
    chassisInitParam.yawCorrectionPidParam.Input_deadband = 1.0f;
    chassisInitParam.yawCorrectionPidParam.maxIntegral = 50.0f;
    chassisInitParam.yawCorrectionPidParam.maxOutput = 5000.0f;
    chassisInitParam.lineCorrectionPidParam.kp = 0.0f;
    chassisInitParam.lineCorrectionPidParam.ki = 0.0f;
    chassisInitParam.lineCorrectionPidParam.kd = 0.0f;
    chassisInitParam.lineCorrectionPidParam.maxIntegral = 0.0f;
    chassisInitParam.lineCorrectionPidParam.maxOutput = 0.0f;
    chassisInitParam.wheelsetSpdPidParam.kp = 8.0f;
    chassisInitParam.wheelsetSpdPidParam.ki = 5.0f;
    chassisInitParam.wheelsetSpdPidParam.kd = 0.0f;
    chassisInitParam.wheelsetSpdPidParam.maxIntegral = 1000.0f;
    chassisInitParam.wheelsetSpdPidParam.maxOutput = 8000.0f;
    // 使用初始化后的参数创建 chassisModule 实例
    static auto chassisModule = CModChassis(chassisInitParam);




    return APP_OK;
}

} // namespace my_engineer
