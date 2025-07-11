/******************************************************************************
 * @brief        
 * 
 * @file         sys_vison.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-03-13
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "Core.hpp"

namespace my_engineer{

CSystemVision SysVision;

/**
 * @brief 初始化视觉系统
 * 
 * @param param 
 * @return EAppStatus 
 */
EAppStatus CSystemVision::InitSystem(SSystemInitParam_Base *param) {
	
	// 检查参数及ID是否为空
	if (param == nullptr) return APP_ERROR;
	if (param->systemID == ESystemID::SYS_NULL) return APP_ERROR;

	// 类型转换
	auto &visionParam = *reinterpret_cast<SSystemInitParam_Vision *>(param);

	// 初始化视觉设备
	systemID = visionParam.systemID;
	pVision_ = static_cast<CDevVision *>(DeviceIDMap.at(visionParam.visionDevID));

	// 注册系统
	RegisterSystem_();

	systemStatus = APP_ERROR;
	return APP_OK;
}

/******************************************************************************
 * @brief    更新处理
 ******************************************************************************/
void CSystemVision::UpdateHandler_() {

	// 发送数据
	CDevVision::SRaceinfoPkg raceInfo;

// 	 if (SysReferee.systemState == RP_OK) {
//     raceInfo.raceCamp = SysReferee.refereeInfo.robot.robotCamp;
//     raceInfo.raceState = SysReferee.refereeInfo.race.raceStage;
//   } else {
//     raceInfo.raceCamp = 0;
//     raceInfo.raceState = 0;
//   }
	raceInfo.raceCamp = 2;
	raceInfo.raceState = 1;

  raceInfo.exchangeState = 1;
    // (SystemCore.currentAutoCtrlProcess_ == CSystemCore::EAutoCtrlProcess::EXCHANGE) ? 1 : 0;

  SysVision.pVision_->SendPackage(CDevVision::ID_RACE_INFO, raceInfo.header);

	// 检查系统状态
	if (systemStatus != APP_OK) return;

	// 更新接收包
	UpdateOreTankInfo_();
	UpdateUiPointInfo_();
}

/******************************************************************************
 * @brief    心跳处理
 ******************************************************************************/
void CSystemVision::HeartbeatHandler_() {

	// 检查系统状态
	if (systemStatus == APP_RESET) return;

	if (pVision_->deviceStatus == APP_OK) {
		systemStatus = APP_OK;
	} else {
		systemStatus = APP_ERROR;
	}
}

/******************************************************************************
 * @brief    更新矿槽信息
 ******************************************************************************/
EAppStatus CSystemVision::UpdateOreTankInfo_() {

	if (pVision_->visionStatus != CDevVision::EVisionStatus::ONLINE) {
		visionInfo.oreTank = SOreTankInfo();
		return APP_ERROR;
	}

	// visionInfo.oreTank.isFoundOreTank = 1;
	// visionInfo.oreTank.atti_YAW = 120.0f;
	// visionInfo.oreTank.atti_PITCH = -30.0f;
	// visionInfo.oreTank.atti_ROLL = 45.0f;
	// visionInfo.oreTank.posit_X = 20.0f;
	// visionInfo.oreTank.posit_Y = -30.0f;
	// visionInfo.oreTank.posit_Z = 300.0f;

	visionInfo.oreTank.isFoundOreTank = pVision_->oretankinfoPkg.isFoundOreTank;
	visionInfo.oreTank.atti_YAW = pVision_->oretankinfoPkg.atti[0];
	visionInfo.oreTank.atti_PITCH = pVision_->oretankinfoPkg.atti[1];
	visionInfo.oreTank.atti_ROLL = pVision_->oretankinfoPkg.atti[2];
	visionInfo.oreTank.posit_X = pVision_->oretankinfoPkg.posit[0];
	visionInfo.oreTank.posit_Y = pVision_->oretankinfoPkg.posit[1];
	visionInfo.oreTank.posit_Z = pVision_->oretankinfoPkg.posit[2];

	return APP_OK;
}

/******************************************************************************
 * @brief    更新UI信息
 ******************************************************************************/
EAppStatus CSystemVision::UpdateUiPointInfo_() {

	if (pVision_->visionStatus != CDevVision::EVisionStatus::ONLINE) {
		visionInfo.uiPoint = SUiPointInfo();
		return APP_ERROR;
	}

	visionInfo.uiPoint.isFoundOreTank = pVision_->oretankinfoPkg.isFoundOreTank;
	for (int i = 0; i < 5; i++) {
		visionInfo.uiPoint.pointPosit_X[i] = pVision_->oretankinfoPkg.uiPoint_X[i];
		visionInfo.uiPoint.pointPosit_Y[i] = pVision_->oretankinfoPkg.uiPoint_Y[i];
	}

	return APP_OK;
}

} // namespace my_engineer



