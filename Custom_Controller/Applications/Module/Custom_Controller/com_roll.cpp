/******************************************************************************
 * @brief        
 * 
 * @file         com_roll.cpp
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
EAppStatus CModController::CComRoll::InitComponent(SModInitParam_Base &param) {
	// 检查param是否正确
	if (param.moduleID == EModuleID::MOD_NULL) return APP_ERROR;

	// 类型转换
	auto controllerParam = static_cast<SModInitParam_Controller &>(param);

	// 保存电机指针
	motor[0] = MotorIDMap.at(controllerParam.roll_id);

	// // 设置发送节点
	// mtrCanTxNode_[0] = controllerParam.rollTxNode;

	// // 初始化PID控制器
	// controllerParam.rollPosPidParam.threadNum = 1;
	// pidPosCtrl.InitPID(&controllerParam.rollPosPidParam);

	// controllerParam.rollSpdPidParam.threadNum = 1;
	// pidSpdCtrl.InitPID(&controllerParam.rollSpdPidParam);

	// 初始化电机数据输出缓冲区
	// mtrOutputBuffer.fill(0);

	Component_FSMFlag_ = FSM_RESET;
	componentStatus = APP_OK;

	return APP_OK;
}

/******************************************************************************
 * @brief    更新组件
 ******************************************************************************/
EAppStatus CModController::CComRoll::UpdateComponent() {
	// 检查组件状态
	if (componentStatus == APP_RESET) return APP_ERROR;

	// 更新组件信息
	rollInfo.speed = motor[0]->motorData[CDevMtr::DATA_SPEED];
	if (rollInfo.speed > CONTROLLER_ROLL_SPEED_MAX) {
		rollInfo.speed = CONTROLLER_ROLL_SPEED_MAX;
	} else if (rollInfo.speed < -CONTROLLER_ROLL_SPEED_MAX) {
		rollInfo.speed = -CONTROLLER_ROLL_SPEED_MAX;
	}
	
	componentStatus = APP_OK;

	return APP_OK;

}

// /******************************************************************************
//  * @brief    物理位置转换为电机位置
//  * 
//  * @param    phyPosit 
//  * @return   int32_t 
//  ******************************************************************************/
// int32_t CModController::CComRoll::PhyPositToMtrPosit(float_t phyPosit) {
// 	const int32_t zeroOffset = CONTROLLER_ROLL_MOTOR_OFFSET;
// 	const float_t scale = CONTROLLER_ROLL_MOTOR_RATIO;

// 	return (static_cast<int32_t>(phyPosit * scale) + zeroOffset);
// }

// /******************************************************************************
//  * @brief    电机位置转换为物理位置
//  * 
//  * @param    mtrPosit 
//  * @return   float_t 
//  ******************************************************************************/
// float_t CModController::CComRoll::MtrPositToPhyPosit(int32_t mtrPosit) {
// 	const int32_t zeroOffset = CONTROLLER_ROLL_MOTOR_OFFSET;
// 	const float_t scale = CONTROLLER_ROLL_MOTOR_RATIO;

// 	return (static_cast<float_t>(mtrPosit - zeroOffset) / scale);
// }

// /******************************************************************************
//  * @brief    输出更新函数
//  ******************************************************************************/
// EAppStatus CModController::CComRoll::_UpdateOutput(float_t posit) {
// 	// 位置环
// 	DataBuffer<float_t> rollPos = {
// 		static_cast<float_t>(posit) * CONTROLLER_ROLL_MOTOR_DIR,
// 	};

// 	DataBuffer<float_t> rollPosMeasure = {
// 		static_cast<float_t>(motor[0]->motorData[CDevMtr::DATA_POSIT]),
// 	};

// 	auto rollSpd = 
// 		pidPosCtrl.UpdatePidController(rollPos, rollPosMeasure);

// 	// 速度环
// 	DataBuffer<float_t> rollSpdMeasure = {
// 		static_cast<float_t>(motor[0]->motorData[CDevMtr::DATA_SPEED]),
// 	};

// 	auto output = 
// 		pidSpdCtrl.UpdatePidController(rollSpd, rollSpdMeasure);

// 	mtrOutputBuffer = {
// 		static_cast<int16_t>(output[0]),
// 	};

// 	return APP_OK;
// }


} // namespace my_engineer
