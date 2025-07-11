/******************************************************************************
 * @brief        
 * 
 * @file         dev_servo.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-04-16
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "dev_servo.hpp"

namespace my_engineer {

/**
 * @brief 初始化舵机设备
 * 
 * @param pStructInitParam 舵机初始化参数结构体
 * @return EAppStatus 
 */
EAppStatus CDevServo::InitDevice(const SDevInitParam_Base *pStructInitParam) {
	// 检查参数是否为空
	if (pStructInitParam == nullptr) return APP_ERROR;
	// 检查设备ID
	if (pStructInitParam->deviceID == EDeviceID::DEV_NULL) return APP_ERROR;

	// 类型转换并解引用
	auto &param = *static_cast<const SDevInitParam_Servo *>(pStructInitParam);

	// 变量初始化
	deviceID = pStructInitParam->deviceID;
	servoHalTimHandle_ = param.servoHalTimHandle;
	servoTimChannel_ = param.servoTimChannel;

	RegisterDevice_();

	deviceStatus = APP_OK;

	return APP_OK;
}

/**
 * @brief 更新舵机设备
 * 
 */
void CDevServo::UpdateHandler_() {

	// 检查设备状态
	if (deviceStatus == APP_RESET) return;

	static bool last_enable = false;
	// 检查舵机是否使能
	if (servoCmd.enable != last_enable) {
		if (servoCmd.enable) {
			HAL_TIM_PWM_Start(servoHalTimHandle_, servoTimChannel_);
		} else {
			HAL_TIM_PWM_Stop(servoHalTimHandle_, servoTimChannel_);
		}
	}
	last_enable = servoCmd.enable;

	servoCmd.set_angle = std::clamp(servoCmd.set_angle, 0.f, 180.f); // 限制舵机角度范围

	// PWM输出
	uint16_t pulse = static_cast<uint16_t>(servoCmd.set_angle/180.f * 200 + 50);
	__HAL_TIM_SET_COMPARE(servoHalTimHandle_, servoTimChannel_, pulse);

}

/**
 * @brief 心跳处理
 * 
 */
void CDevServo::HeartbeatHandler_() {
}

} // namespace my_engineer
