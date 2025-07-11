/******************************************************************************
 * @brief        
 * 
 * @file         com_pitch1.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-07-10
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "mod_arm.hpp"

namespace my_engineer {

/**
 * @brief 初始化机械臂Pitch1组件
 * 
 * @param param 
 * @return EAppStatus 
 */
EAppStatus CModArm::CComPitch1::InitComponent(SModInitParam_Base &param) {
	// 检查param是否正确
	if (param.moduleID == EModuleID::MOD_NULL) return APP_ERROR;

	// 类型转换
	auto armParam = static_cast<SModInitParam_Arm &>(param);

	// 保存电机指针
	motor = MotorIDMap.at(armParam.MotorID_Pitch1);

	// 设置发送节点
	mtrCanTxNode = armParam.MotorTxNode_Pitch1;

	// 初始化PID控制器
	armParam.Pitch1PosPidParam.threadNum = 1;
	pidPosCtrl.InitPID(&armParam.Pitch1PosPidParam);

	armParam.Pitch1SpdPidParam.threadNum = 1;
	pidSpdCtrl.InitPID(&armParam.Pitch1SpdPidParam);

	mtrOutputBuffer.fill(0);

	Component_FSMFlag_ = FSM_RESET;
	componentStatus = APP_OK;

	return APP_OK;
}

/**
 * @brief 更新Pitch1组件
 * 
 * @return EAppStatus 
 */
EAppStatus CModArm::CComPitch1::UpdateComponent() {
	if (componentStatus == APP_RESET) {
		return APP_ERROR;
	}

	pitch1Info.posit = motor->motorData[CDevMtr::DATA_POSIT] * ARM_PITCH1_MOTOR_DIR;
	pitch1Info.isPositArrived = abs(pitch1Info.posit - pitch1Cmd.setPosit) < 1000;

	switch (Component_FSMFlag_) {

		case FSM_RESET: {
			mtrOutputBuffer.fill(0);
			pidPosCtrl.ResetPidController();
			pidSpdCtrl.ResetPidController();
			return APP_OK;
		}

		case FSM_PREINIT: {
			pitch1Cmd.setPosit = 0;
			motor->motorData[CDevMtr::DATA_POSIT] = 0;
			mtrOutputBuffer.fill(0);
			pidPosCtrl.ResetPidController();
			pidSpdCtrl.ResetPidController();
			Component_FSMFlag_ = FSM_INIT;
			return APP_OK;
		}

		case FSM_INIT: {
			if (motor->motorStatus == CDevMtr::EMotorStatus::STALL) {
				pitch1Cmd.setPosit = ARM_PITCH1_MOTOR_OFFSET;
				motor->motorData[CDevMtr::DATA_POSIT] = (static_cast<int32_t>(0.05 * 8192) + rangeLimit) * ARM_PITCH1_MOTOR_DIR;
				pidPosCtrl.ResetPidController();
				pidSpdCtrl.ResetPidController();
				Component_FSMFlag_ = FSM_CTRL;
				componentStatus = APP_OK;
				return APP_OK;
			}
			pitch1Cmd.setPosit += 200;
			return _UpdateOutput(static_cast<float_t>(pitch1Cmd.setPosit));
		}

		case FSM_CTRL: {
			pitch1Cmd.setPosit = std::clamp(pitch1Cmd.setPosit, static_cast<int32_t>(0), rangeLimit);
			return _UpdateOutput(static_cast<float_t>(pitch1Cmd.setPosit));
		}

		default: {
			StopComponent();
			mtrOutputBuffer.fill(0);
			pidPosCtrl.ResetPidController();
			pidSpdCtrl.ResetPidController();
			componentStatus = APP_ERROR;
			return APP_ERROR;
		}
	}

	return APP_OK;
}

// 物理位置转换为电机位置
int32_t CModArm::CComPitch1::PhyPositToMtrPosit(float_t phyPosit) {
	const int32_t zeroOffset = ARM_PITCH1_MOTOR_OFFSET;
	const float_t scale = ARM_PITCH1_MOTOR_RATIO;

	return static_cast<int32_t>((phyPosit * scale) + zeroOffset);
}

// 电机位置转换为物理位置
float_t CModArm::CComPitch1::MtrPositToPhyPosit(int32_t mtrPosit) {
	const int32_t zeroOffset = ARM_PITCH1_MOTOR_OFFSET;
	const float_t scale = ARM_PITCH1_MOTOR_RATIO;

	return (static_cast<float_t>(mtrPosit - zeroOffset) / scale);
}

// 输出更新函数
EAppStatus CModArm::CComPitch1::_UpdateOutput(float_t posit) {
	DataBuffer<float_t> Pos = { 
		static_cast<float_t>(posit * ARM_PITCH1_MOTOR_DIR) 
	};

	DataBuffer<float_t> PosMeasure = {
		static_cast<float_t>(motor->motorData[CDevMtr::DATA_POSIT])
	};

	auto Spd = pidPosCtrl.UpdatePidController(Pos, PosMeasure);

	DataBuffer<float_t> SpdMeasure = {
		static_cast<float_t>(motor->motorData[CDevMtr::DATA_SPEED])
	};

	auto output = pidSpdCtrl.UpdatePidController(Spd, SpdMeasure);

	mtrOutputBuffer = { static_cast<int16_t>(output[0]) };

	return APP_OK;
}

} // namespace my_engineer

