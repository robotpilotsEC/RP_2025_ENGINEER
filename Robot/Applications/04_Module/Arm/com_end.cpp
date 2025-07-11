/**
 * @file com_end.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 机械臂末端组件
 * @version 1.0
 * @date 2025-01-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */


#include "mod_arm.hpp"

namespace my_engineer {

/**
 * @brief 初始化机械臂末端组件
 * 
 * @param param 
 * @return EAppStatus 
 */
EAppStatus CModArm::CComEnd::InitComponent(SModInitParam_Base &param) {
	if (param.moduleID == EModuleID::MOD_NULL) return APP_ERROR;

	auto armParam = static_cast<SModInitParam_Arm &>(param);

	motor[L] = MotorIDMap.at(armParam.MotorID_End_L);
	motor[R] = MotorIDMap.at(armParam.MotorID_End_R);

	mtrCanTxNode[L] = armParam.MotorTxNode_End_L;
	mtrCanTxNode[R] = armParam.MotorTxNode_End_R;

	armParam.endPosPidParam.threadNum = 2;
	pidPosCtrl.InitPID(&armParam.endPosPidParam);

	armParam.endSpdPidParam.threadNum = 2;
	pidSpdCtrl.InitPID(&armParam.endSpdPidParam);

	mtrOutputBuffer.fill(0);

	Component_FSMFlag_ = FSM_RESET;
	componentStatus = APP_OK;

	return APP_OK;
}

/**
 * @brief 更新组件
 * 
 */
EAppStatus CModArm::CComEnd::UpdateComponent() {
	if (componentStatus == APP_RESET) return APP_ERROR;

	endInfo.posit_Pitch = 
		(motor[L]->motorData[CDevMtr::DATA_POSIT] * ARM_END_PITCH_MOTOR_L_DIR +
		motor[R]->motorData[CDevMtr::DATA_POSIT] * ARM_END_PITCH_MOTOR_R_DIR) / 2;
	endInfo.posit_Roll = 
		(motor[L]->motorData[CDevMtr::DATA_POSIT] * ARM_END_ROLL_MOTOR_L_DIR +
		motor[R]->motorData[CDevMtr::DATA_POSIT] * ARM_END_ROLL_MOTOR_R_DIR) / 2;
	endInfo.isPositArrived_Pitch = (abs(endCmd.setPosit_Pitch - endInfo.posit_Pitch) < 8192 * 2);
	endInfo.isPositArrived_Roll = (abs(endCmd.setPosit_Roll - endInfo.posit_Roll) < 8192 * 2);

	switch (Component_FSMFlag_) {

		case FSM_RESET: {
			mtrOutputBuffer.fill(0);
			pidPosCtrl.ResetPidController();
			pidSpdCtrl.ResetPidController();
			return APP_OK;
		}

		case FSM_PREINIT: {
			endCmd.setPosit_Pitch = 0;
			motor[L]->motorData[CDevMtr::DATA_POSIT] = 0;
			motor[R]->motorData[CDevMtr::DATA_POSIT] = 0;
			mtrOutputBuffer.fill(0);
			pidPosCtrl.ResetPidController();
			pidSpdCtrl.ResetPidController();
			Component_FSMFlag_ = FSM_INIT;
			return APP_OK;
		}

		case FSM_INIT: {
			if (motor[L]->motorStatus == CDevMtr::EMotorStatus::STALL || motor[R]->motorStatus == CDevMtr::EMotorStatus::STALL) {
				endCmd.setPosit_Pitch = ARM_END_PITCH_MOTOR_OFFSET;
				motor[L]->motorData[CDevMtr::DATA_POSIT] = (static_cast<int32_t>(0.5 * 8192) + rangeLimit_Pitch) * ARM_END_PITCH_MOTOR_L_DIR;
				motor[R]->motorData[CDevMtr::DATA_POSIT] = (static_cast<int32_t>(0.5 * 8192) + rangeLimit_Pitch) * ARM_END_PITCH_MOTOR_R_DIR;
				pidPosCtrl.ResetPidController();
				pidSpdCtrl.ResetPidController();
				componentStatus = APP_OK;
				Component_FSMFlag_ = FSM_CTRL;
				return APP_OK;
			}
			endCmd.setPosit_Pitch += 200;
			return _UpdateOutput(static_cast<float_t>(endCmd.setPosit_Pitch), 
								static_cast<float_t>(0));
		}

		case FSM_CTRL: {
			endCmd.setPosit_Pitch = std::clamp(endCmd.setPosit_Pitch, static_cast<int32_t>(0), rangeLimit_Pitch);
			return _UpdateOutput(static_cast<float_t>(endCmd.setPosit_Pitch), 
								static_cast<float_t>(endCmd.setPosit_Roll));
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

/**
 * @brief 物理位置转换为电机位置
 * 
 * @param phyPosit 
 * @return int32_t 
 */
int32_t CModArm::CComEnd::PhyPositToMtrPosit_Pitch(float_t phyPosit) {
	const int32_t zeroOffset = ARM_END_PITCH_MOTOR_OFFSET;
	const float_t scale = ARM_END_PITCH_MOTOR_RATIO;

	return (static_cast<int32_t>(phyPosit * scale) + zeroOffset);
}

int32_t CModArm::CComEnd::PhyPositToMtrPosit_Roll(float_t phyPosit) {
	const int32_t zeroOffset = 0;
	const float_t scale = ARM_END_ROLL_MOTOR_RATIO;

	return (static_cast<int32_t>(phyPosit * scale) + zeroOffset);
}

/**
 * @brief 电机位置转换为物理位置
 * 
 * @param mtrPosit 
 * @return float_t 
 */
float_t CModArm::CComEnd::MtrPositToPhyPosit_Pitch(int32_t mtrPosit) {
	const int32_t zeroOffset = ARM_END_PITCH_MOTOR_OFFSET;
	const float_t scale = ARM_END_PITCH_MOTOR_RATIO;

	return (static_cast<float_t>(mtrPosit - zeroOffset) / scale);
}

float_t CModArm::CComEnd::MtrPositToPhyPosit_Roll(int32_t mtrPosit) {
	const int32_t zeroOffset = 0;
	const float_t scale = ARM_END_ROLL_MOTOR_RATIO;

	return (static_cast<float_t>(mtrPosit - zeroOffset) / scale);
}

/**
 * @brief 输出更新函数
 * 
 * @param posit_Pitch 
 * @param posit_Roll 
 * @return EAppStatus 
 */
EAppStatus CModArm::CComEnd::_UpdateOutput(float_t posit_Pitch, float_t posit_Roll) {
	DataBuffer<float_t> endPos = {
		static_cast<float_t>(posit_Pitch * ARM_END_PITCH_MOTOR_L_DIR + posit_Roll * ARM_END_ROLL_MOTOR_L_DIR),
		static_cast<float_t>(posit_Pitch * ARM_END_PITCH_MOTOR_R_DIR + posit_Roll * ARM_END_ROLL_MOTOR_R_DIR)
	};

	DataBuffer<float_t> endPosMeasure = {
		static_cast<float_t>(motor[L]->motorData[CDevMtr::DATA_POSIT]),
		static_cast<float_t>(motor[R]->motorData[CDevMtr::DATA_POSIT])
	};

	auto endSpd = pidPosCtrl.UpdatePidController(endPos, endPosMeasure);

	DataBuffer<float_t> endSpdMeasure = {
		static_cast<float_t>(motor[L]->motorData[CDevMtr::DATA_SPEED]),
		static_cast<float_t>(motor[R]->motorData[CDevMtr::DATA_SPEED])
	};

	auto output = pidSpdCtrl.UpdatePidController(endSpd, endSpdMeasure);

	mtrOutputBuffer = {
		static_cast<int16_t>(output[L]),
		static_cast<int16_t>(output[R])
	};

	return APP_OK;
}

} // namespace my_engineer
