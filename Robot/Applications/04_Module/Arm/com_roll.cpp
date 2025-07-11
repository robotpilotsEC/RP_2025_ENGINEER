/******************************************************************************
 * @brief        
 * 
 * @file         com_roll.cpp
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
 * @brief 初始化机械臂Roll组件
 * 
 * @param param 
 * @return EAppStatus 
 */
EAppStatus CModArm::CComRoll::InitComponent(SModInitParam_Base &param) {
	if (param.moduleID == EModuleID::MOD_NULL) return APP_ERROR;

	auto armParam = static_cast<SModInitParam_Arm &>(param);

	motor = MotorIDMap.at(armParam.MotorID_Roll);
	mtrCanTxNode = armParam.MotorTxNode_Roll;

	armParam.RollPosPidParam.threadNum = 1;
	pidPosCtrl.InitPID(&armParam.RollPosPidParam);

	armParam.RollSpdPidParam.threadNum = 1;
	pidSpdCtrl.InitPID(&armParam.RollSpdPidParam);

	mtrOutputBuffer.fill(0);

	Component_FSMFlag_ = FSM_RESET;
	componentStatus = APP_OK;

	return APP_OK;
}

/**
 * @brief 更新Roll组件
 * 
 * @return EAppStatus 
 */
EAppStatus CModArm::CComRoll::UpdateComponent() {
	if (componentStatus == APP_RESET) {
		return APP_ERROR;
	}

	rollInfo.posit = motor->motorData[CDevMtr::DATA_POSIT] * ARM_ROLL_MOTOR_DIR;
	rollInfo.isPositArrived = abs(rollInfo.posit - rollCmd.setPosit) < 1000;

	switch (Component_FSMFlag_) {

		case FSM_RESET: {
			mtrOutputBuffer.fill(0);
			pidPosCtrl.ResetPidController();
			pidSpdCtrl.ResetPidController();
			return APP_OK;
		}

		case FSM_PREINIT: {
			rollCmd.setPosit = 0;
			motor->motorData[CDevMtr::DATA_POSIT] = 0;
			mtrOutputBuffer.fill(0);
			pidPosCtrl.ResetPidController();
			pidSpdCtrl.ResetPidController();
			Component_FSMFlag_ = FSM_INIT;
			return APP_OK;
		}

		case FSM_INIT: {
			if (motor->motorStatus == CDevMtr::EMotorStatus::STALL) {
				rollCmd.setPosit = ARM_ROLL_MOTOR_OFFSET;
				motor->motorData[CDevMtr::DATA_POSIT] = (static_cast<int32_t>(0.05 * 8192) + rangeLimit) * ARM_ROLL_MOTOR_DIR;
				pidPosCtrl.ResetPidController();
				pidSpdCtrl.ResetPidController();
				Component_FSMFlag_ = FSM_CTRL;
				componentStatus = APP_OK;
				return APP_OK;
			}
			rollCmd.setPosit += 200;
			return _UpdateOutput(static_cast<float_t>(rollCmd.setPosit));
		}

		case FSM_CTRL: {
			rollCmd.setPosit = std::clamp(rollCmd.setPosit, static_cast<int32_t>(0), rangeLimit);
			return _UpdateOutput(static_cast<float_t>(rollCmd.setPosit));
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
int32_t CModArm::CComRoll::PhyPositToMtrPosit(float_t phyPosit) {
	const int32_t zeroOffset = ARM_ROLL_MOTOR_OFFSET;
	const float_t scale = ARM_ROLL_MOTOR_RATIO;

	return static_cast<int32_t>((phyPosit * scale) + zeroOffset);
}

// 电机位置转换为物理位置
float_t CModArm::CComRoll::MtrPositToPhyPosit(int32_t mtrPosit) {
	const int32_t zeroOffset = ARM_ROLL_MOTOR_OFFSET;
	const float_t scale = ARM_ROLL_MOTOR_RATIO;

	return (static_cast<float_t>(mtrPosit - zeroOffset) / scale);
}

// 输出更新函数
EAppStatus CModArm::CComRoll::_UpdateOutput(float_t posit) {
	DataBuffer<float_t> Pos = { 
		static_cast<float_t>(posit * ARM_ROLL_MOTOR_DIR) 
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
