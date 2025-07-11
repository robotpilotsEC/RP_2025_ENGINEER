/**
 * @file conf_interface.cpp
 * @author Zoe
 * @brief 完成所有设备的配置
 * @email 2328339747@qq.com
 * @date 2024-11-01
 * 
 * @details
 */

#include "conf_device.hpp"
#include "Device.hpp"

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim12;

namespace my_engineer {

/**
 * @brief 配置并初始化所有设备
 * @return APP_OK - 初始化成功
 * @return APP_ERROR - 初始化失败
 */
EAppStatus InitAllDevice(){

    // Bmi088
    static CMemsBmi088 bmi088;
    CMemsBmi088::SMemsInitParam_Bmi088 bmi088_initparam;
    bmi088_initparam.deviceID = EDeviceID::DEV_MEMS_BMI088;
    bmi088_initparam.interfaceID = EInterfaceID::INF_SPI2;
    bmi088_initparam.AccelUnitCsPort = BMI_ACC_CS_GPIO_Port;
    bmi088_initparam.AccelUnitCsPin = BMI_ACC_CS_Pin;
    bmi088_initparam.GyroUnitCsPort = BMI_GYRO_CS_GPIO_Port;
    bmi088_initparam.GyroUnitCsPin = BMI_GYRO_CS_Pin;
    bmi088_initparam.useTempControl = false;
    bmi088_initparam.tempTarget = 40.0f;
    bmi088_initparam.halTimHandle = &htim3;
    bmi088_initparam.halTimChannel = TIM_CHANNEL_4;
    bmi088_initparam.tempPidParam.kp = 10.0f;
    bmi088_initparam.tempPidParam.ki = 3.0f;
    bmi088_initparam.tempPidParam.kd = 0.5f;
    bmi088_initparam.tempPidParam.maxIntegral = 20;
    bmi088_initparam.tempPidParam.maxOutput = 100;
    bmi088.InitDevice(&bmi088_initparam);

    // 摇杆
    static CDevRocker rocker;
    CDevRocker::SDevInitParam_Rocker rocker_initparam;
    rocker_initparam.deviceID = EDeviceID::DEV_ROCKER;
    rocker_initparam.interfaceID = EInterfaceID::INF_ADC1;
    rocker_initparam.X_channel = CInfADC::EAdcChannel::CHANNEL_14;
    rocker_initparam.Y_channel = CInfADC::EAdcChannel::CHANNEL_16;
    rocker_initparam.halGpioPort = rocker_KEY_GPIO_Port;
    rocker_initparam.halGpioPin = rocker_KEY_Pin;
    rocker.InitDevice(&rocker_initparam);

    // 蜂鸣器
    static CDevBuzzer buzzer;
    CDevBuzzer::SDevInitParam_Buzzer buzzer_initparam;
    buzzer_initparam.deviceID = EDeviceID::DEV_BUZZER;
    buzzer_initparam.halTimerHandle = &htim12;
    buzzer_initparam.channel = TIM_CHANNEL_2;
    buzzer_initparam.base_frequency = 137500000 / 1375;
    buzzer.InitDevice(&buzzer_initparam);

    // 与机器人通信设备
    static CDevControllerLink controllerLink;
    CDevControllerLink::SDevInitParam_ControllerLink controllerLink_initparam;
    controllerLink_initparam.deviceID = EDeviceID::DEV_CONTROLLER_LINK;
    controllerLink_initparam.interfaceID = EInterfaceID::INF_UART7;
    controllerLink.InitDevice(&controllerLink_initparam);

    // 自定义控制器电机
    // Traverse
    static CDevMtrM2006 controllerMotor_Traverse;
    CDevMtrM2006::SMtrInitParam_M2006 controllerMotor_Traverse_initparam;
    controllerMotor_Traverse_initparam.deviceID = EDeviceID::DEV_CONTROLLER_MTR_TRAVERSE;
    controllerMotor_Traverse_initparam.interfaceID = EInterfaceID::INF_CAN2;
    controllerMotor_Traverse_initparam.djiMtrID = CDevMtrDJI::EDjiMtrID::ID_2;
    controllerMotor_Traverse_initparam.useAngleToPosit = true;
    controllerMotor_Traverse_initparam.useStallMonit = true;
    controllerMotor_Traverse_initparam.stallMonitDataSrc = CDevMtr::DATA_TORQUE;
    controllerMotor_Traverse.InitDevice(&controllerMotor_Traverse_initparam);

    // Stretch
    static CDevMtrM2006 controllerMotor_Stretch;
    CDevMtrM2006::SMtrInitParam_M2006 controllerMotor_Stretch_initparam;
    controllerMotor_Stretch_initparam.deviceID = EDeviceID::DEV_CONTROLLER_MTR_STRETCH;
    controllerMotor_Stretch_initparam.interfaceID = EInterfaceID::INF_CAN2;
    controllerMotor_Stretch_initparam.djiMtrID = CDevMtrDJI::EDjiMtrID::ID_1;
    controllerMotor_Stretch_initparam.useAngleToPosit = true;
    controllerMotor_Stretch_initparam.useStallMonit = true;
    controllerMotor_Stretch_initparam.stallMonitDataSrc = CDevMtr::DATA_TORQUE;
    controllerMotor_Stretch.InitDevice(&controllerMotor_Stretch_initparam);

    // Yaw
    static CDevMtrM2006 controllerMotor_Yaw;
    CDevMtrM2006::SMtrInitParam_M2006 controllerMotor_Yaw_initparam;
    controllerMotor_Yaw_initparam.deviceID = EDeviceID::DEV_CONTROLLER_MTR_YAW;
    controllerMotor_Yaw_initparam.interfaceID = EInterfaceID::INF_CAN2;
    controllerMotor_Yaw_initparam.djiMtrID = CDevMtrDJI::EDjiMtrID::ID_3;
    controllerMotor_Yaw_initparam.useAngleToPosit = true;
    controllerMotor_Yaw_initparam.useStallMonit = true;
    controllerMotor_Yaw_initparam.stallMonitDataSrc = CDevMtr::DATA_TORQUE;
    controllerMotor_Yaw.InitDevice(&controllerMotor_Yaw_initparam);

    // Roll
    static CDevMtrM2006 controllerMotor_Roll;
    CDevMtrM2006::SMtrInitParam_M2006 controllerMotor_Roll_initparam;
    controllerMotor_Roll_initparam.deviceID = EDeviceID::DEV_CONTROLLER_MTR_ROLL;
    controllerMotor_Roll_initparam.interfaceID = EInterfaceID::INF_CAN2;
    controllerMotor_Roll_initparam.djiMtrID = CDevMtrDJI::EDjiMtrID::ID_4;
    controllerMotor_Roll_initparam.useAngleToPosit = true;
    controllerMotor_Roll_initparam.useStallMonit = true;
    controllerMotor_Roll_initparam.stallMonitDataSrc = CDevMtr::DATA_TORQUE;
    controllerMotor_Roll.InitDevice(&controllerMotor_Roll_initparam);


    return APP_OK;
}

} // namespace my_engineer
