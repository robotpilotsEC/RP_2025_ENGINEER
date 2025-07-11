/******************************************************************************
 * @brief        
 * 
 * @file         com_rocker.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-03-30
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "mod_controller.hpp"

namespace my_engineer {

/******************************************************************************
 * @brief    初始化自定义控制器摇杆模块
 ******************************************************************************/
EAppStatus CModController::CComRocker::InitComponent(SModInitParam_Base &param) {
	
	// 检查param是否正确
	if (param.moduleID == EModuleID::MOD_NULL) return APP_ERROR;

	// 类型转换
	auto controllerParam = static_cast<SModInitParam_Controller &>(param);

	// 保存摇杆指针
	rocker = static_cast<CDevRocker *>(DeviceIDMap.at(controllerParam.rocker_id));

	Component_FSMFlag_ = FSM_RESET;
	componentStatus = APP_OK;

	return APP_OK;
}

/******************************************************************************
 * @brief    更新组件
 ******************************************************************************/
EAppStatus CModController::CComRocker::UpdateComponent() {
	// 检查组件状态
	if (componentStatus == APP_RESET) return APP_ERROR;

	componentStatus = APP_OK;

	// 更新组件信息
	int32_t transform_value_x = rocker->rockerValues.X - CONTROLLER_ROCKER_RANGE / 2;
	if (abs(transform_value_x) < CONTROLLER_ROCKER_DEAD_ZONE) rockerInfo.X = 0;
	else rockerInfo.X = transform_value_x;

	int32_t transform_value_y = rocker->rockerValues.Y - CONTROLLER_ROCKER_RANGE / 2;
	if (abs(transform_value_y) < CONTROLLER_ROCKER_DEAD_ZONE) rockerInfo.Y = 0;
	else rockerInfo.Y = transform_value_y;

	if (rocker->rockerValues.key == 1) key_duration += HAL_GetTick() - last_time_stamp;
	else key_duration = 0;

	last_time_stamp = HAL_GetTick();

	if (key_duration == 0) rockerInfo.Key_status = KEY_STATUS::RELEASE;
	else if (key_duration < CONTROLLER_ROCKER_KEY_LONG_PRESS_DURATION) rockerInfo.Key_status = KEY_STATUS::PRESS;
	else rockerInfo.Key_status = KEY_STATUS::LONG_PRESS;

	return APP_OK;
}

} // namespace my_engineer
