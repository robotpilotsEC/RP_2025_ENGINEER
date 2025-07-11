/******************************************************************************
 * @brief        
 * 
 * @file         com_yaw.cpp
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
 * @brief    初始化滚轴电机模块
 ******************************************************************************/
EAppStatus CModController::CComYaw::InitComponent(SModInitParam_Base &param) {
	// 检查param是否正确
	if (param.moduleID == EModuleID::MOD_NULL) return APP_ERROR;

	// 类型转换
	auto controllerParam = static_cast<SModInitParam_Controller &>(param);

	// 保存电机指针
	motor[0] = MotorIDMap.at(controllerParam.yaw_id);

	// 设置发送节点
	mtrCanTxNode_[0] = controllerParam.yawTxNode;

	// 初始化PID控制器
	controllerParam.yawPosPidParam.threadNum = 1;
	pidPosCtrl.InitPID(&controllerParam.yawPosPidParam);

	controllerParam.yawSpdPidParam.threadNum = 1;
	pidSpdCtrl.InitPID(&controllerParam.yawSpdPidParam);

	// 初始化电机数据输出缓冲区
	mtrOutputBuffer.fill(0);

	Component_FSMFlag_ = FSM_RESET;
	componentStatus = APP_OK;

	return APP_OK;
}

/******************************************************************************
 * @brief    更新组件
 ******************************************************************************/
EAppStatus CModController::CComYaw::UpdateComponent() {
	// 检查组件状态
	if (componentStatus == APP_RESET) return APP_ERROR;

	// 更新组件信息
	yawInfo.posit = motor[0]->motorData[CDevMtr::DATA_POSIT] * CONTROLLER_YAW_MOTOR_DIR;
	yawInfo.isPositArrived = (abs(yawCmd.setPosit - yawInfo.posit) < 8192 * 0.02);

	switch (Component_FSMFlag_) {
		case FSM_RESET: {
			StopComponent();
			mtrOutputBuffer.fill(0);
			pidPosCtrl.ResetPidController();
			pidSpdCtrl.ResetPidController();
			componentStatus = APP_OK;
			return APP_OK;
		}

		case FSM_PREINIT: {
			yawCmd.setPosit = static_cast<int32_t>(rangeLimit * 1.2);
			motor[0]->motorData[CDevMtr::DATA_POSIT] = yawCmd.setPosit * CONTROLLER_YAW_MOTOR_DIR;
			mtrOutputBuffer.fill(0);
			pidPosCtrl.ResetPidController();
			pidSpdCtrl.ResetPidController();
			Component_FSMFlag_ = FSM_INIT;
			return APP_OK;
		}

		case FSM_INIT: {
			if (motor[0]->motorStatus == CDevMtr::EMotorStatus::STALL) {
				yawCmd = SYawCmd();
				motor[0]->motorData[CDevMtr::DATA_POSIT] = -static_cast<int32_t>(0.001 * 8192) * CONTROLLER_YAW_MOTOR_DIR;
				pidPosCtrl.ResetPidController();
				pidSpdCtrl.ResetPidController();
				Component_FSMFlag_ = FSM_CTRL;
				componentStatus = APP_OK;
				return APP_OK;
			}
			yawCmd.setPosit -= 100;
			return _UpdateOutput(static_cast<float_t>(yawCmd.setPosit));
		}

		case FSM_CTRL: {
			yawCmd.setPosit = std::clamp(yawCmd.setPosit, static_cast<int32_t>(0), rangeLimit);
			if (yawCmd.isFree) {
				mtrOutputBuffer.fill(0);
				return APP_OK;
			}
			return _UpdateOutput(static_cast<float_t>(yawCmd.setPosit));
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
}

/******************************************************************************
 * @brief    物理位置转换为电机位置
 * 
 * @param    phyPosit 
 * @return   int32_t 
 ******************************************************************************/
int32_t CModController::CComYaw::PhyPositToMtrPosit(float_t phyPosit) {
	const int32_t zeroOffset = CONTROLLER_YAW_MOTOR_OFFSET;
	const float_t scale = CONTROLLER_YAW_MOTOR_RATIO;

	return static_cast<int32_t>(phyPosit * scale + zeroOffset);
}

/******************************************************************************
 * @brief    电机位置转换为物理位置
 * 
 * @param    mtrPosit 
 * @return   float_t 
 ******************************************************************************/
float_t CModController::CComYaw::MtrPositToPhyPosit(int32_t mtrPosit) {
	const int32_t zeroOffset = CONTROLLER_YAW_MOTOR_OFFSET;
	const float_t scale = CONTROLLER_YAW_MOTOR_RATIO;

	return static_cast<float_t>(mtrPosit - zeroOffset) / scale;
}

/******************************************************************************
 * @brief    输出更新函数
 ******************************************************************************/
EAppStatus CModController::CComYaw::_UpdateOutput(float_t posit) {
	// 位置环
	DataBuffer<float_t> yawPos = {
		static_cast<float_t>(posit) * CONTROLLER_YAW_MOTOR_DIR,
	};

	DataBuffer<float_t> yawPosMeasure = {
		static_cast<float_t>(motor[0]->motorData[CDevMtr::DATA_POSIT]),
	};

	auto yawSpd = 
		pidPosCtrl.UpdatePidController(yawPos, yawPosMeasure);

	// 速度环
	DataBuffer<float_t> yawSpdMeasure = {
		static_cast<float_t>(motor[0]->motorData[CDevMtr::DATA_SPEED]),
	};

	auto output = 
		pidSpdCtrl.UpdatePidController(yawSpd, yawSpdMeasure);

	mtrOutputBuffer = {
		static_cast<int16_t>(output[0]),
	};

	return APP_OK;
}

}
