/******************************************************************************
 * @brief        
 * 
 * @file         dev_rocker.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-03-29
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "dev_rocker.hpp"

namespace my_engineer {

CDevRocker *pDevRocker_test;

/**
 * @brief 初始化摇杆设备
 * 
 * @param pStructInitParam 
 * @return EAppStatus 
 */
EAppStatus CDevRocker::InitDevice(const SDevInitParam_Base *pStructInitParam){

	// 检查param是否正确
	if (pStructInitParam == nullptr) return APP_ERROR;
	if (pStructInitParam->deviceID == EDeviceID::DEV_NULL) return APP_ERROR;

	// 类型转换
	auto &rockerParam = *static_cast<const SDevInitParam_Rocker *>(pStructInitParam);
	deviceID = rockerParam.deviceID;
	adcInterface_ = reinterpret_cast<CInfADC *>(InterfaceIDMap.at(rockerParam.interfaceID));
	X_channel_ = rockerParam.X_channel;
	Y_channel_ = rockerParam.Y_channel;
	halGpioPort_ = rockerParam.halGpioPort;
	halGpioPin_ = rockerParam.halGpioPin;

	RegisterDevice_();

	deviceStatus = APP_OK;

	pDevRocker_test = this;

	return APP_OK;
}

/******************************************************************************
 * @brief    更新处理
 ******************************************************************************/
void CDevRocker::UpdateHandler_(){
	
	if (deviceStatus == APP_RESET) return;

	// 获取ADC值
	rockerValues.X = static_cast<int32_t>(adcInterface_->Read(X_channel_));
	rockerValues.Y = static_cast<int32_t>(adcInterface_->Read(Y_channel_));

	// if (HAL_GPIO_ReadPin(halGpioPort_, halGpioPin_) == GPIO_PIN_RESET) {
	// 	rockerValues.key = 1;
	// }
	// else {
	// 	rockerValues.key = 0;
	// }

	// 若按键值有变化，更新时间戳
	if (HAL_GPIO_ReadPin(halGpioPort_, halGpioPin_) != key_new_value_) {
		key_new_value_ = HAL_GPIO_ReadPin(halGpioPort_, halGpioPin_);
		key_udapte_time_ = HAL_GetTick();
	}
	// 若按键值在一定时间内没有变化，更新按键值
	if (HAL_GetTick() - key_udapte_time_ > 10) {
		rockerValues.key = key_new_value_ == GPIO_PIN_RESET ? 1 : 0;
	}

	
}

/******************************************************************************
 * @brief    心跳处理
 ******************************************************************************/
void CDevRocker::HeartbeatHandler_(){
}

} // namespace my_engineer
