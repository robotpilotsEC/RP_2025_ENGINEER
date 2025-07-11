/******************************************************************************
 * @brief        
 * 
 * @file         sys_referee.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-04-17
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "System.hpp"
#include "Module.hpp"

namespace my_engineer {

// 实例化裁判系统
CSystemReferee SysReferee;

/**
 * @brief 初始化裁判系统
 * 
 * @param pStruct 
 * @return EAppStatus 
 */
EAppStatus CSystemReferee::InitSystem(SSystemInitParam_Base *pStruct) {
	
	// 检查参数及ID是否为空
	if (pStruct == nullptr) return APP_ERROR;
	if (pStruct->systemID == ESystemID::SYS_NULL) return APP_ERROR;

	// 类型转换
	auto &param = *reinterpret_cast<SSystemInitParam_Referee *>(pStruct);

	// 初始化裁判设备
	systemID = param.systemID;
	pRefereeDev_ = static_cast<CDevReferee *>(DeviceIDMap.at(param.refereeDevID));
	pInterface_ = reinterpret_cast<CInfUART *>(InterfaceIDMap.at(EInterfaceID::INF_UART10));

	if (systemTaskHandle != nullptr) {
		vTaskDelete(systemTaskHandle);
	}
	xTaskCreate(StartSysRefereeUiTask, "SysReferee Task",
		512, nullptr, proc_SystemTaskPriority,
		&systemTaskHandle);
	

	// 注册系统
	RegisterSystem_();

	systemStatus = APP_OK;
	return APP_OK;
}

/**
 * @brief 更新处理
 * 
 */
void CSystemReferee::UpdateHandler_() {

	// 检查系统状态
	if (systemStatus != APP_OK) return;

	UpdateRaceInfo_();
	UpdateRobotInfo_();
}

/**
 * @brief 心跳处理
 * 
 */
void CSystemReferee::HeartbeatHandler_() {

	// 检查系统状态
	if (systemStatus == APP_RESET) return;

	if (pRefereeDev_->refereeState == CDevReferee::ERefereeStatus::ONLINE)
		systemStatus = APP_OK;
	else
		systemStatus = APP_ERROR;
}

/**
 * @brief 更新比赛信息
 * 
 * @return EAppStatus 
 */
EAppStatus CSystemReferee::UpdateRaceInfo_() {

	refereeInfo.unixTimestamp =
		pRefereeDev_->raceStatusPkg.timestamp;

	refereeInfo.race.raceType =
		pRefereeDev_->raceStatusPkg.raceType;

	refereeInfo.race.raceStage =
		pRefereeDev_->raceStatusPkg.raceStage;

	return APP_OK;
}

/**
 * @brief 更新机器人信息
 * 
 * @return EAppStatus 
 */
EAppStatus CSystemReferee::UpdateRobotInfo_() {

	if (pRefereeDev_->robotStatusPkg.robotId == 0) {
		refereeInfo.robot.robotCamp = 0;
	}
	else {
		refereeInfo.robot.robotCamp =
			pRefereeDev_->robotStatusPkg.robotId < 100 ? 1 : 2;
	}
	refereeInfo.robot.robotID =
		pRefereeDev_->robotStatusPkg.robotId % 100;

	return APP_OK;
}

}	// namespace my_engineer
