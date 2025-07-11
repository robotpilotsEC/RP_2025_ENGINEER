/******************************************************************************
 * @brief        
 * 
 * @file         sys_controller_link.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-04-05
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "sys_controller_link.hpp"

namespace my_engineer {

#define I_AM_CONTROLLER 0 // 当前板子是控制器

// 实例化一个控制器通信系统
CSystemControllerLink SysControllerLink;

/**
 * @brief 初始化控制器通信系统
 * 
 * @param pStruct 
 * @return EAppStatus 
 */
EAppStatus CSystemControllerLink::InitSystem(SSystemInitParam_Base *pStruct) {
	
	// 检查参数及ID是否为空
	if (pStruct == nullptr) return APP_ERROR;
	if (pStruct->systemID == ESystemID::SYS_NULL) return APP_ERROR;

	// 类型转换
	auto &param = *reinterpret_cast<SSystemInitParam_ControllerLink *>(pStruct);

	// 初始化控制器通信设备
	systemID = param.systemID;
	pcontrollerLink_ = static_cast<CDevControllerLink *>(DeviceIDMap.at(param.controllerLinkDevID));

	// 注册系统
	RegisterSystem_();

	systemStatus = APP_ERROR;
	return APP_OK;
}

/**
 * @brief 更新处理
 * 
 */
void CSystemControllerLink::UpdateHandler_() {
	// 检查系统状态
	if (systemStatus != APP_OK) return;

	static uint8_t delay = 40;
	delay -= 4;
	if (delay > 0) return;
	delay = 40;

	#if I_AM_CONTROLLER == 0
		// 更新控制器信息
		UpdateControllerLinkInfo_();
		// 更新发送数据包
		UpdateRobotDataPkg_();
		// 发送机器人信息
		pcontrollerLink_->SendPackage(CDevControllerLink::ID_ROBOT_DATA, pcontrollerLink_->robotData_info_pkg.header);
	#else
		// 更新机器人信息
		UpdateRobotInfo_();
		// 更新发送数据包
		UpdateControllerDataPkg_();
		// 发送控制器信息
		pcontrollerLink_->SendPackage(CDevControllerLink::ID_CONTROLLER_DATA, pcontrollerLink_->controllerData_info_pkg.header);
	#endif

}

/**
 * @brief 更新控制器信息
 * 
 */
void CSystemControllerLink::UpdateControllerLinkInfo_() {
	if (systemStatus != APP_OK) return;

	// 更新控制器信息
	controllerInfo.controller_OK = pcontrollerLink_->controllerData_info_pkg.controller_OK;
	controllerInfo.return_success = pcontrollerLink_->controllerData_info_pkg.return_success;
	controllerInfo.Rocker_X = pcontrollerLink_->controllerData_info_pkg.rocker_X;
	controllerInfo.Rocker_Y = pcontrollerLink_->controllerData_info_pkg.rocker_Y;
	controllerInfo.Rocker_Key = static_cast<KEY_STATUS>(pcontrollerLink_->controllerData_info_pkg.rocker_Key);
	controllerInfo.posit_stretch = pcontrollerLink_->controllerData_info_pkg.posit_stretch;
	controllerInfo.posit_traverse = pcontrollerLink_->controllerData_info_pkg.posit_traverse;
	controllerInfo.angle_yaw = pcontrollerLink_->controllerData_info_pkg.angle_yaw;
	controllerInfo.speed_roll = pcontrollerLink_->controllerData_info_pkg.speed_roll;
}

/**
 * @brief 更新机器人信息
 * 
 */
void CSystemControllerLink::UpdateRobotInfo_() {
	if (systemStatus != APP_OK) return;

	// 更新机器人信息
	robotInfo.ask_reset_flag = pcontrollerLink_->robotData_info_pkg.ask_reset_flag;
	robotInfo.controlled_by_controller = pcontrollerLink_->robotData_info_pkg.controlled_by_controller;
	robotInfo.ask_return_flag = pcontrollerLink_->robotData_info_pkg.ask_return_flag;
	robotInfo.posit_traverse = pcontrollerLink_->robotData_info_pkg.posit_traverse;
	robotInfo.posit_stretch = pcontrollerLink_->robotData_info_pkg.posit_stretch;
	robotInfo.posit_lift = pcontrollerLink_->robotData_info_pkg.posit_lift;
	robotInfo.angle_yaw = pcontrollerLink_->robotData_info_pkg.angle_yaw;
	robotInfo.angle_roll = pcontrollerLink_->robotData_info_pkg.angle_roll;
	robotInfo.angle_pitch = pcontrollerLink_->robotData_info_pkg.angle_pitch;

}

/**
 * @brief 更新发送数据包 RobotData
 */
void CSystemControllerLink::UpdateRobotDataPkg_() {
	if (systemStatus != APP_OK) return;

	// 更新发送包的信息
	pcontrollerLink_->robotData_info_pkg.ask_reset_flag = robotInfo.ask_reset_flag;
	pcontrollerLink_->robotData_info_pkg.controlled_by_controller = robotInfo.controlled_by_controller;
	pcontrollerLink_->robotData_info_pkg.ask_return_flag = robotInfo.ask_return_flag;
	pcontrollerLink_->robotData_info_pkg.posit_traverse = robotInfo.posit_traverse;
	pcontrollerLink_->robotData_info_pkg.posit_stretch = robotInfo.posit_stretch;
	pcontrollerLink_->robotData_info_pkg.posit_lift = robotInfo.posit_lift;
	pcontrollerLink_->robotData_info_pkg.angle_yaw = robotInfo.angle_yaw;
	pcontrollerLink_->robotData_info_pkg.angle_roll = robotInfo.angle_roll;
	pcontrollerLink_->robotData_info_pkg.angle_pitch = robotInfo.angle_pitch;
}

/**
 * @brief 更新发送数据包 ControllerData
 */
void CSystemControllerLink::UpdateControllerDataPkg_() {
	if (systemStatus != APP_OK) return;

	// 更新发送包的信息
	pcontrollerLink_->controllerData_info_pkg.controller_OK = controllerInfo.controller_OK;
	pcontrollerLink_->controllerData_info_pkg.return_success = controllerInfo.return_success;
	pcontrollerLink_->controllerData_info_pkg.rocker_X = controllerInfo.Rocker_X;
	pcontrollerLink_->controllerData_info_pkg.rocker_Y = controllerInfo.Rocker_Y;
	pcontrollerLink_->controllerData_info_pkg.rocker_Key = static_cast<uint8_t>(controllerInfo.Rocker_Key);
	pcontrollerLink_->controllerData_info_pkg.posit_stretch = controllerInfo.posit_stretch;
	pcontrollerLink_->controllerData_info_pkg.posit_traverse = controllerInfo.posit_traverse;
	pcontrollerLink_->controllerData_info_pkg.angle_yaw = controllerInfo.angle_yaw;
	pcontrollerLink_->controllerData_info_pkg.speed_roll = controllerInfo.speed_roll;
}

/**
 * @brief 心跳处理
 * 
 */
void CSystemControllerLink::HeartbeatHandler_() {
	systemStatus = APP_OK;
}

} // namespace my_engineer
