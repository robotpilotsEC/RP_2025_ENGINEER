/******************************************************************************
 * @brief        
 * 
 * @file         com_traverse.cpp
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
 * @brief    初始化自定义控制器横移电机模块
 ******************************************************************************/
EAppStatus CModController::CComTraverse::InitComponent(SModInitParam_Base &param){
	// 检查param是否正确
	if (param.moduleID == EModuleID::MOD_NULL) return APP_ERROR;

	// 类型转换
	auto controllerParam = static_cast<SModInitParam_Controller &>(param);

	// 保存电机指针
	motor[0] = MotorIDMap.at(controllerParam.traverse_id);

	// 设置发送节点
	mtrCanTxNode_[0] = controllerParam.traverseTxNode;

	// 初始化PID控制器
	controllerParam.traversePosPidParam.threadNum = 1;
	pidPosCtrl.InitPID(&controllerParam.traversePosPidParam);

	controllerParam.traverseSpdPidParam.threadNum = 1;
	pidSpdCtrl.InitPID(&controllerParam.traverseSpdPidParam);

	// 初始化电机数据输出缓冲区
	mtrOutputBuffer.fill(0);	

	Component_FSMFlag_ = FSM_RESET;
	componentStatus = APP_OK;

	return APP_OK;
}

/******************************************************************************
 * @brief    更新组件
 ******************************************************************************/
EAppStatus CModController::CComTraverse::UpdateComponent() {
	// 检查组件状态
	if (componentStatus == APP_RESET) return APP_ERROR;

	// 更新组件信息
	traverseInfo.posit = motor[0]->motorData[CDevMtr::DATA_POSIT] * CONTROLLER_TRAVERSE_MOTOR_DIR;
	traverseInfo.isPositArrived = (abs(traverseCmd.setPosit - traverseInfo.posit) < 8192 * 0.15);

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
			traverseCmd.setPosit = static_cast<int32_t>(rangeLimit * 1.2);
			motor[0]->motorData[CDevMtr::DATA_POSIT] = traverseCmd.setPosit * CONTROLLER_TRAVERSE_MOTOR_DIR;
			mtrOutputBuffer.fill(0);
			pidPosCtrl.ResetPidController();
			pidSpdCtrl.ResetPidController();
			Component_FSMFlag_ = FSM_INIT;
			return APP_OK;
		}

		case FSM_INIT: {
			// 电机堵转,说明初始化完成
			if (motor[0]->motorStatus == CDevMtr::EMotorStatus::STALL) {
				traverseCmd = STraverseCmd();
				// 补偿超出限位的值
				motor[0]->motorData[CDevMtr::DATA_POSIT] = -static_cast<int32_t>(0.2 * 8192) * CONTROLLER_TRAVERSE_MOTOR_DIR;
				pidPosCtrl.ResetPidController();
				pidSpdCtrl.ResetPidController();
				Component_FSMFlag_ = FSM_CTRL;
				componentStatus = APP_OK;
				return APP_OK;
			}
			traverseCmd.setPosit -= 400;
			return _UpdateOutput(static_cast<float_t>(traverseCmd.setPosit));
		}

		case FSM_CTRL: {
			if (traverseCmd.isFree) {
				#ifdef CONTROLLER_ASSIST_ENABLE
					static int32_t lastPosit = traverseInfo.posit;  // 上次位置
					static int8_t sign_record = 0; // 记录上次位置的符号
					static uint32_t sign_record_cnt = 0; // 同一符号连续出现的次数
					static uint32_t sign_record_judge = 3; // 判断方向变化的次数阈值
					static int8_t sign = 0; // 当前方向
					static float_t judge = 1.0f; // 判断位置变化的编码器阈值
					static float_t output_quiet = 700; // 静止时的输出
					static float_t output_move = 200; // 移动时的输出
					static uint32_t output_cnt = 0; // quiet输出计数器
					static uint32_t output_move_judge = 20; // quiet输出时长

					// 获取当前符号
					int8_t current_sign;
					if (abs(lastPosit - traverseInfo.posit) >= judge) {
						current_sign = (traverseInfo.posit - lastPosit) > 0 ? 1 : -1;
					} else {
						current_sign = 0;
					}

					// 判断符号变化
					if (current_sign != sign_record) {
						sign_record = current_sign;
						sign_record_cnt = 0;
					} else {
						sign_record_cnt++;
						// 确定方向发生变化，重置输出方向及计数器
						if (sign_record_cnt >= sign_record_judge && current_sign != sign) {
							sign = current_sign;
							output_cnt = 0;
						}
					}

					// 根据计数器判断输出
					if (output_cnt < output_move_judge) {
						mtrOutputBuffer[0] = static_cast<int16_t>(output_quiet * sign);
					} else {
						mtrOutputBuffer[0] = static_cast<int16_t>(output_move * sign);
					}

					// 进行更新
					output_cnt++;
					lastPosit = traverseInfo.posit;

					return APP_OK;
				#else
					// 电机数据输出缓冲区始终为0
					mtrOutputBuffer.fill(0);
					return APP_OK;
				#endif
			}
			// 限制位置
			traverseCmd.setPosit = std::clamp(traverseCmd.setPosit, static_cast<int32_t>(0), rangeLimit);
			return _UpdateOutput(static_cast<float_t>(traverseCmd.setPosit));
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
 ******************************************************************************/
int32_t CModController::CComTraverse::PhyPositToMtrPosit(float_t phyPosit){
	const int32_t zeroOffset = 0;
	const float_t scale = CONTROLLER_TRAVERSE_MOTOR_RATIO;

	return (static_cast<int32_t>(phyPosit * scale) + zeroOffset);
}

/******************************************************************************
 * @brief    电机位置转换为物理位置
 ******************************************************************************/
float_t CModController::CComTraverse::MtrPositToPhyPosit(int32_t mtrPosit){
	const int32_t zeroOffset = 0;
	const float_t scale = CONTROLLER_TRAVERSE_MOTOR_RATIO;

	return (static_cast<float_t>(mtrPosit - zeroOffset) / scale);
}

/******************************************************************************
 * @brief    输出更新函数
 ******************************************************************************/
EAppStatus CModController::CComTraverse::_UpdateOutput(float_t posit){

	// 位置环
	DataBuffer<float_t> traversePos = {
		static_cast<float_t>(posit) * CONTROLLER_TRAVERSE_MOTOR_DIR,
	};

	DataBuffer<float_t> traversePosMeasure = {
		static_cast<float_t>(motor[0]->motorData[CDevMtr::DATA_POSIT]),
	};

	auto traverseSpd = 
		pidPosCtrl.UpdatePidController(traversePos, traversePosMeasure);

	// 速度环
	DataBuffer<float_t> traverseSpdMeasure = {
		static_cast<float_t>(motor[0]->motorData[CDevMtr::DATA_SPEED]),
	};

	auto output = 
		pidSpdCtrl.UpdatePidController(traverseSpd, traverseSpdMeasure);

	mtrOutputBuffer = {
		static_cast<int16_t>(output[0]),
	};

	return APP_OK;
}

} // namespace my_engineer
