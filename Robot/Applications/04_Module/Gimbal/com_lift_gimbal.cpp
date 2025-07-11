/**
 * @file com_lift.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 云台升降组件
 * @version 1.0
 * @date 2025-01-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "mod_gimbal.hpp"

namespace my_engineer {

/**
 * @brief 初始化云台升降组件
 * 
 * @param param 
 * @return EAppStatus 
 */
EAppStatus CModGimbal::CComLift::InitComponent(SModInitParam_Base &param){
	// 检查param是否正确
	if (param.moduleID == EModuleID::MOD_NULL) return APP_ERROR;

	// 类型转换
	auto gimbalParam = static_cast<SModInitParam_Gimbal &>(param);

	// 保存电机指针
	motor = MotorIDMap.at(gimbalParam.liftMotorID);

	// 设置发送节点
	mtrCanTxNode = gimbalParam.liftMotorTxNode;

	// 初始化PID控制器
	gimbalParam.liftPosPidParam.threadNum = 1;
	pidPosCtrl.InitPID(&gimbalParam.liftPosPidParam);

	gimbalParam.liftSpdPidParam.threadNum = 1;
	pidSpdCtrl.InitPID(&gimbalParam.liftSpdPidParam);

	// 初始化电机数据输出缓冲区
	mtrOutputBuffer.fill(0);

	Component_FSMFlag_ = FSM_RESET;
	componentStatus = APP_OK;

	return APP_OK;
}

/**
 * @brief 更新组件
 * 
 */
EAppStatus CModGimbal::CComLift::UpdateComponent() {
	// 检查组件状态
	if (componentStatus == APP_RESET) return APP_ERROR;

	// 更新组件信息
	liftInfo.posit = (motor->motorData[CDevMtr::DATA_POSIT] * GIMBAL_LIFT_MOTOR_DIR);
	liftInfo.isPositArrived = (abs(liftCmd.setPosit - liftInfo.posit) < 8192 * 1);

	switch (Component_FSMFlag_){
		case FSM_RESET: {
			// 重置状态下，电机数据输出缓冲区始终为0
			mtrOutputBuffer.fill(0);
			pidPosCtrl.ResetPidController();
			pidSpdCtrl.ResetPidController();
			return APP_OK;
		}

		case FSM_PREINIT: {
			// 预初始化状态下，把电机位置设定值设为1.2倍的范围限制
			// liftCmd.setPosit = static_cast<int32_t>(rangeLimit * 1.2);
			// motor->motorData[CDevMtr::DATA_POSIT] = liftCmd.setPosit * GIMBAL_LIFT_MOTOR_DIR;
			liftCmd.setPosit = static_cast<int32_t>(0);
			motor->motorData[CDevMtr::DATA_POSIT] = 0;
			mtrOutputBuffer.fill(0);
			pidPosCtrl.ResetPidController();
			pidSpdCtrl.ResetPidController();
			Component_FSMFlag_ = FSM_INIT;
			return APP_OK;
		}

		case FSM_INIT: {
			// 电机堵转，说明初始化完成
			if (motor->motorStatus == CDevMtr::EMotorStatus::STALL) {
				liftCmd = SLiftCmd();
				// 补偿超出限位的值
				motor->motorData[CDevMtr::DATA_POSIT] = static_cast<int32_t>(0.1 * 8192 + rangeLimit) * GIMBAL_LIFT_MOTOR_DIR;
				pidPosCtrl.ResetPidController();
				pidSpdCtrl.ResetPidController();
				Component_FSMFlag_ = FSM_CTRL;
				componentStatus = APP_OK;
			}
			liftCmd.setPosit += 400;
			return _UpdateOutput(static_cast<float_t>(liftCmd.setPosit));
		}

		case FSM_CTRL: {
			// 限制位置
			liftCmd.setPosit = std::clamp(liftCmd.setPosit, static_cast<int32_t>(0), rangeLimit);
			// 更新输出
			return _UpdateOutput(static_cast<float_t>(liftCmd.setPosit));
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
int32_t CModGimbal::CComLift::PhyPositToMtrPosit(float_t phyPosit){
	const int32_t zeroOffset = 0;
	const float_t scale      = GIMBAL_LIFT_MOTOR_RATIO;

	return (static_cast<int32_t>(phyPosit * scale) + zeroOffset);
}

/**
 * @brief 电机位置转换为物理位置
 * 
 * @param mtrPosit 
 * @return float_t 
 */
float_t CModGimbal::CComLift::MtrPositToPhyPosit(int32_t mtrPosit){
	const int32_t zeroOffset = 0;
	const float_t scale = GIMBAL_LIFT_MOTOR_RATIO;

	return (static_cast<float_t>(mtrPosit - zeroOffset) / scale);
}

/**
 * @brief 输出更新函数
 * 
 * @param posit 
 * @return EAppStatus 
 */
EAppStatus CModGimbal::CComLift::_UpdateOutput(float_t posit) {
	// 位置环
	DataBuffer<float_t> liftPos = {
		static_cast<float_t>(posit) * GIMBAL_LIFT_MOTOR_DIR,
	};

	DataBuffer<float_t> liftPosMeasure = {
		static_cast<float_t>(motor->motorData[CDevMtr::DATA_POSIT])
	};

	auto liftSpd = 
		pidPosCtrl.UpdatePidController(liftPos, liftPosMeasure);

	// 速度环
	DataBuffer<float_t> liftSpdMeasure = {
		static_cast<float_t>(motor->motorData[CDevMtr::DATA_SPEED])
	};

	auto output = 
		pidSpdCtrl.UpdatePidController(liftSpd, liftSpdMeasure);

	mtrOutputBuffer = {
		static_cast<int16_t>(output[0])
	};

	return APP_OK;
}

} // namespace my_engineer
