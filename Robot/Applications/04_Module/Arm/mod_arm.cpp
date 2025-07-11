/******************************************************************************
 * @brief        
 * 
 * @file         mod_arm.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-05-04
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "mod_arm.hpp"



namespace my_engineer {

CModArm *pArm_test = nullptr;

/**
 * @brief 初始化机械臂模块
 * 
 * @param param 
 * @return EAppStatus 
 */
EAppStatus CModArm::InitModule(SModInitParam_Base &param) {
	// 检查param是否正确
	if (param.moduleID == EModuleID::MOD_NULL) return APP_ERROR;

	// 类型转换
	auto armParam = static_cast<SModInitParam_Arm &>(param);
	moduleID = armParam.moduleID;

	comYaw_.InitComponent(param);
	comPitch1_.InitComponent(param);
	comPitch2_.InitComponent(param);
	comRoll_.InitComponent(param);
	comEnd_.InitComponent(param);

	// 创建任务并注册模块
	CreateModuleTask_();
	RegisterModule_();

	// test
	pArm_test = this;

	Module_FSMFlag_ = FSM_RESET;
	moduleStatus = APP_OK;

	return APP_OK;
}

/**
 * @brief 更新处理
 * 
 */
void CModArm::UpdateHandler_() {
	// 检查模块状态
	if (moduleStatus == APP_RESET) return;

	// 更新组件
	comYaw_.UpdateComponent();
	comPitch1_.UpdateComponent();
	comPitch2_.UpdateComponent();
	comRoll_.UpdateComponent();
	comEnd_.UpdateComponent();

	// 更新模块信息
	armInfo.angle_Yaw = comYaw_.MtrAngleToPhyAngle(comYaw_.yawInfo.angle);
	armInfo.angle_Pitch1 = comPitch1_.MtrPositToPhyPosit(comPitch1_.pitch1Info.posit);
	armInfo.angle_Pitch2 = comPitch2_.MtrPositToPhyPosit(comPitch2_.pitch2Info.posit);
	armInfo.angle_Roll = comRoll_.MtrPositToPhyPosit(comRoll_.rollInfo.posit);
	armInfo.angle_end_pitch =
		comEnd_.MtrPositToPhyPosit_Pitch(comEnd_.endInfo.posit_Pitch);
	armInfo.angle_end_roll =
		comEnd_.MtrPositToPhyPosit_Roll(comEnd_.endInfo.posit_Roll);
	armInfo.isAngleArrived_Yaw = comYaw_.yawInfo.isAngleArrived;
	armInfo.isAngleArrived_Pitch1 = comPitch1_.pitch1Info.isPositArrived;
	armInfo.isAngleArrived_Pitch2 = comPitch2_.pitch2Info.isPositArrived;
	armInfo.isAngleArrived_Roll = comRoll_.rollInfo.isPositArrived;
	armInfo.isAngleArrived_End_Pitch = comEnd_.endInfo.isPositArrived_Pitch;
	armInfo.isAngleArrived_End_Roll = comEnd_.endInfo.isPositArrived_Roll;

	// 填充电机发送缓冲区
	CDevMtrDJI::FillCanTxBuffer(comPitch1_.motor,
								comPitch1_.mtrCanTxNode->dataBuffer,
								comPitch1_.mtrOutputBuffer[0]);
	CDevMtrDJI::FillCanTxBuffer(comPitch2_.motor,
								comPitch2_.mtrCanTxNode->dataBuffer,
								comPitch2_.mtrOutputBuffer[0]);
	CDevMtrDJI::FillCanTxBuffer(comRoll_.motor,
								comRoll_.mtrCanTxNode->dataBuffer,
								comRoll_.mtrOutputBuffer[0]);
	CDevMtrDJI::FillCanTxBuffer(comEnd_.motor[CComEnd::L],
								comEnd_.mtrCanTxNode[CComEnd::L]->dataBuffer,
								comEnd_.mtrOutputBuffer[CComEnd::L]);
	CDevMtrDJI::FillCanTxBuffer(comEnd_.motor[CComEnd::R],
								comEnd_.mtrCanTxNode[CComEnd::R]->dataBuffer,
								comEnd_.mtrOutputBuffer[CComEnd::R]);

}

/**
 * @brief 心跳处理
 * 
 */
void CModArm::HeartbeatHandler_() {
	// 心跳处理逻辑
}

/**
 * @brief 创建模块任务
 * 
 */
EAppStatus CModArm::CreateModuleTask_() {
	// 任务已存在，删除任务
	if (moduleTaskHandle != nullptr) vTaskDelete(moduleTaskHandle);

	// 创建任务
	xTaskCreate(StartArmModuleTask, "Arm Task",
				512, this, proc_ModuleTaskPriority,
				&moduleTaskHandle);

	return APP_OK;
}

/**
 * @brief 限制机械臂模块的控制命令大小
 * 
 */
EAppStatus CModArm::RestrictArmCommand_() {
	// 检查模块状态
	if (moduleStatus == APP_RESET) {
		armCmd = SArmCmd();
		return APP_ERROR;
	}

	// 限制控制命令大小
	armCmd.set_angle_Yaw =
		std::clamp(armCmd.set_angle_Yaw, ARM_YAW_PHYSICAL_RANGE_MIN, ARM_YAW_PHYSICAL_RANGE_MAX);
	armCmd.set_angle_Pitch1 =
		std::clamp(armCmd.set_angle_Pitch1,
				   ARM_PITCH1_PHYSICAL_RANGE_MIN, ARM_PITCH1_PHYSICAL_RANGE_MAX);
	armCmd.set_angle_Pitch2 =
		std::clamp(armCmd.set_angle_Pitch2,
				   ARM_PITCH2_PHYSICAL_RANGE_MIN, ARM_PITCH2_PHYSICAL_RANGE_MAX);
	armCmd.set_angle_Roll =
		std::clamp(armCmd.set_angle_Roll,
				   ARM_ROLL_PHYSICAL_RANGE_MIN, ARM_ROLL_PHYSICAL_RANGE_MAX);
	armCmd.set_angle_end_pitch =
		std::clamp(armCmd.set_angle_end_pitch,
				   ARM_END_PITCH_PHYSICAL_RANGE_MIN, ARM_END_PITCH_PHYSICAL_RANGE_MAX);

	// 自动控制启用，则不继续做限制
	if (armCmd.isAutoCtrl) return APP_OK;

	return APP_OK;
}

} // namespace my_engineer
